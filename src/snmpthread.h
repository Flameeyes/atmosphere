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
#ifndef SNMPTHREAD_H
#define SNMPTHREAD_H

#include <qthread.h>
#include <qobject.h>

namespace ATMOS {
	class Device;
};
class DeviceWidget;

/*!
\brief SNMP fetcher thread

This class is used by DeviceWidget to get the data form SNMP interface of a
device.
It's a QThread child, so it runs in a parallel way.
*/
class SNMPThread : public QThread, public QObject
{
public:
	SNMPThread(DeviceWidget *W, ATMOS::Device *D);
	~SNMPThread();
	
	bool stopped;		//!< If true, we should stop the thread ASAP
protected:
	void run();
	
	ATMOS::Device *dev;	//!< Device to take data from
	DeviceWidget *dw;	//!< Dialog to show data to
};

#endif
