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
#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H

#include "clinterface.h"

class QSocketNotifier;

namespace ATMOS {
	/*!
	\brief Serial Interace access
	
	This class allow ATMOSphere applications and libraries to access to the serial
	interface of a device.
	*/
	class SerialInterface : public CLInterface
	{
	Q_OBJECT
	public:
		SerialInterface(Device *D);
		~SerialInterface();
	protected slots:
		void serialDataReceived();
	protected:
		QSocketNotifier *n;	//!< Notifier which trigger when data on serial is ready
		int fd;			//!< Descriptor to the open serial
		void actualSendLine(const QString &line);
	};
};

#endif
