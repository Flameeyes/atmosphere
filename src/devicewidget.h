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
#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include "uidevicewidget.h"
#include "snmpthread.h"

// Internal classes
class ATMOS::Device;
class SNMPThread;

// Classes defined for tab widgets
class CLIWidget;
class uiSnmpSummary;
class uiSnmpInterfaces;
class uiSnmpIP;
class uiSnmpIPStats;
class uiDeviceLog;
class EventsLogWidget;

// QT classes
class QMutex;
class QListViewItem;

/*!
\brief Device management widget

This is the main class which shows the data of the device, allow the access
to the CLIs and shows snmp statistics.
*/
class DeviceWidget : public uidevicewidget
{
Q_OBJECT
friend class SNMPThread;
public:
	DeviceWidget(ATMOS::Device *D, QWidget *parent = 0, const char *name = 0);
	~DeviceWidget();
	
	void trueListInterfaces();
	
protected slots:
	void goBack();
	
	void serialInitComplete();
	void serialNewInformative(const QString &msg);
	
	void telnetOpenInterface();
	void telnetInitComplete();
	void telnetInitFailed();
	void telnetNewInformative(const QString &msg);
	void telnetKeepAlive();
	
	void listInterfaces();
	void executedInterface(QListViewItem *lwi);
	
	void fetchLog();
	void getIPStats();

protected:
	void serialOpenInterface();
	void verboseOutput(const QString &interface, const QString &msg);
	
	ATMOS::Device *dev;		//!< Device to which the dialog is referred
	SNMPThread *t;			//!< Thread which reads the variables from snmp
	QMutex *mutexViewInterfaces;	//!< Mutex protecting viewInterfaces KListView
	
	QBuffer *logBuffer;		//!< Eventlog buffer
	QTextStream *logStream;		//!< Eventlog streamed
	
	CLIWidget *serial;		//!< Serial CommandLine Interface Widget
	CLIWidget *telnet;		//!< Telnet CommandLine Interface Widget
	
	uiSnmpSummary *snmpSummary;	//!< SNMP Summary page
	uiSnmpInterfaces *snmpIfaces;	//!< SNMP Interfaces page
	uiSnmpIP *snmpIPSetup;		//!< SNMP IP Settings
	uiSnmpIPStats *snmpIPStats;	//!< SNMP IP Statistics
	
	uiDeviceLog *logDevice;		//!< Device's log
	EventsLogWidget *logEvents;	//!< Events' log
};

#endif
