/***************************************************************************
 *   Copyright (C) 2004 by Diego "Flameeyes" Pettenò                       *
 *   dgp85@users.sourceforge.net                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "telnetinterface.h"
#include "atmosdevice.h"
#include "emissingsupport.h"

#include <qsocket.h>

namespace ATMOS {

/*!
\brief Default constructor
\param D Device to open the telnet connection to (it's passed to CLInterface)

\note This function aborts if called with a device which doesn't support
	telnet access.

This function opens the socket to the telnet interface and insert the password
to access the CLI
*/
TelnetInterface::TelnetInterface(Device *D)
 : CLInterface(D)
{
	if ( ! dev->hasTelnet() || ! dev->driver()->supportTelnet() )
		throw ATMOS::EMissingSupport(ATMOS::EMissingSupport::msTelnet);
	
	s = new QSocket(this, "socket");
	connect(s, SIGNAL(connected()), this, SLOT(socketConnected()));
	connect(s, SIGNAL(connectionClosed()), this, SLOT(socketClosed()));
	
	s->connectToHost( dev->IP(), dev->telnetPort() );

	infoExp.push_back(QRegExp("logged on; type `@close' to close connection.\\r*\\n*"));
	infoExp.push_back(QRegExp("\\*\\*\\* telnet session will close in \\d+s\\r*\\n*"));
	
	termination = "\r\n";
}

TelnetInterface::~TelnetInterface()
{
	delete s;
}

//! Slot triggered when the QSocket is connected
void TelnetInterface::socketConnected()
{
	s->readAll();
	s->writeBlock( dev->telnetPassword() + "\r\n", dev->telnetPassword().length()+2 );
	connect(s, SIGNAL(readyRead()), this, SLOT(telnetDataReceived()));
	
	emit initComplete();
}

/*!
\brief Slot triggered when the QSocket closes connection

This function set to NULL the telnetInterface of the device, and delete itself.
In this way, when the connection is closed, we can always reopen it the next time we open
the device widget
*/
void TelnetInterface::socketClosed()
{
	emit initFailed();
	deleteLater();
}

/*!
\brief Sends a line to the telnet interface
\param line line to send (with trailing \\r)
*/
void TelnetInterface::actualSendLine(const QString &line)
{
	s->writeBlock( line, line.length() );
}

/*!
\brief Data received from telnet interface

This function is called when data is sent by the router through the serial interface.
It calls CLInterface::cleanBuffer() function to clean up the buffer and emit proper signals.
*/
void TelnetInterface::telnetDataReceived()
{
	int r = s->readBlock(currPos, bufferFreeBytes() );
	currPos[r] = '\0';
	currPos += r;
	
	cleanBuffer();
}

};

#include "telnetinterface.moc"
