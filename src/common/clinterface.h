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
#ifndef CLINTERFACE_H
#define CLINTERFACE_H

#include "utils.h"

#include <qobject.h>
#include <qmutex.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qvaluelist.h>
#include <qhostaddress.h>

#include <stdint.h>

namespace ATMOS {
	class Device;
	
	/*!
	\brief Command Line Interface Class
	
	This class is inherited by SerialInterface and TelnetInterface, because
	these two classes shares a lot of miscellaneous code and command parser.
	*/
	class CLInterface : public QObject
	{
	Q_OBJECT
	public:
		~CLInterface();
	public slots:
		void sendLine(const QString &line);
		
//@{
/*!
\name tftp
\brief TFTP commands
*/
	public slots:
		void tftpConnect(const QHostAddress &addr);
		void tftpGetFile(const QString &remotename, const QString &localname = QString::null);
		void tftpPutFile(const QString &remotename, const QString &localname);
//@}
	signals:
		/*!
		\brief Signal emitted when new lines are available to read
		\param sl List of lines available to read
		
		Must be connected to get the new lines
		*/
		void newLines(const QStringList &sl);
		
		/*!
		\brief Signal emitted when a new informative message is found in the buffer
		\param msg Informative message found
		
		Must be connected to get info about timeouts, or other events
		*/
		void newInformative(const QString &msg);
		
		//! Signal emitted when the CLI is initialized correctly
		void initComplete();
		
		//! Signal emitted when the CLI fails to initalize (or it's disconnected)
		void initFailed();
		
	protected:
		CLInterface(Device *D);
		static const unsigned short bufLen = 4096;
			//!< Length of SerialInterface::buffer buffer
		
		Device *dev;	//!< Device to open the serial interface to
		QMutex mutex;		//!< Mutual exclusion for threading support
		char buffer[bufLen];	//!< Buffer to read the data to
		char *currPos;		//!< Current position in the buffer (pointer)
		QRegExpList infoExp;
			//!< List of informative messages (in form of regexp) which can be found in the buffer
		QString termination;	//!< Line-termination string
		
		void cleanBuffer();
		void cleanFromInfos(QStringList &lst);
		void goHome();
			
		/*!
		\brief Actually send the line over the interface
		\param line line to send (complete of \\r)
		
		This function is the one called by sendLine() parent function
		*/
		virtual void actualSendLine(const QString &line) = 0;
		
		//! Returns the free bytes in the buffer (trivial function)
		inline uint32_t bufferFreeBytes()
		{ return (uint32_t)(currPos-1-(currPos-buffer)); }
	};
};

#endif
