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
#include "serialinterface.h"
#include "atmosdevice.h"
#include "emissingsupport.h"

#include <qsocketnotifier.h>

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace ATMOS {

/*!
\brief Default constructor
\param D Device to open the serial connection to (it's passed to CLInterface)

\note This function aborts if called with a device which doesn't support
	serial access.

This function opens the TTYs device as a descriptor, and sets the parameters
(9600N1 with RTS/CTS hardware control).
*/
SerialInterface::SerialInterface(Device *D)
 : CLInterface(D)
{
	if ( ! dev->hasSerial() || ! dev->driver()->supportSerial() )
		throw ATMOS::EMissingSupport(ATMOS::EMissingSupport::msSerial);
	
	fd = open( dev->serialDevice().latin1(), O_RDWR|O_NOCTTY|O_NONBLOCK );
	
	if ( fd == -1 )
	{
		qWarning("Unable to open serial device");
		emit initFailed();
		return;
	}
	struct termios options;
	tcgetattr(fd, &options);
	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);
	options.c_cflag |= (CLOCAL | CREAD | CS8 | CRTSCTS);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | PARENB | CSTOPB | CSIZE);
	tcsetattr(fd, TCSANOW, &options);
	
	n = new QSocketNotifier( fd, QSocketNotifier::Read, this );
	connect( n, SIGNAL(activated(int)), this, SLOT(serialDataReceived()) );
	
	infoExp.push_back(QRegExp("add_query_by_id\\(\\) query table is full!\\r*\\n*"));
	infoExp.push_back(QRegExp("sorry! No such name - .* !\\r*\\n*"));
	infoExp.push_back(QRegExp("Be Attacked:.*,src:ip:\\d+.\\d+.\\d+.\\d+,dst:port:\\d+\\r*\\n*"));
	infoExp.push_back(QRegExp("EVENT:Showtime\\r*\\n*Rate:.*Fast.*Interleave\\r*\\n*.*Down[ \\t]+\\d+[ \\t]+\\d+\\r*\\n*.*Up[ \\t]+(\\d+)[\\t ]+(\\d+)\\r*\\n*"));
	
	termination = "\n";
	emit initComplete();
}

SerialInterface::~SerialInterface()
{
	delete n;
}

/*!
\brief Sends a line to the serial interface
\param line line to send (with trailing \\r)
*/
void SerialInterface::actualSendLine(const QString &line)
{
	write(fd, line.latin1(), line.length());
}

/*!
\brief Data received from serial interface

This function is called when data is sent by the router through the serial interface.
It emits the SerialInterface::newLines() signal.
*/
void SerialInterface::serialDataReceived()
{
	int r = read( fd, currPos, bufferFreeBytes() );
	currPos[r] = '\0';
	currPos += r;
	
	cleanBuffer();
}

};

#include "serialinterface.moc"
