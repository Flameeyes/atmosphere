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

#include <atmosdevice.h>
#include <settings.h>
#include <utils.h>
#include <serialinterface.h>
#include <telnetinterface.h>

#include "devicewidget.h"
#include "snmpthread.h"
#include "interfacestats.h"
#include "cliwidget.h"
#include "eventslogwidget.h"

#include "uisnmpsummary.h"
#include "uisnmpinterfaces.h"
#include "uisnmpip.h"
#include "uisnmpipstats.h"
#include "uidevicelog.h"

#include <libksnmp/mib1213.h>

#include <klineedit.h>
#include <klistview.h>
#include <kmessagebox.h>
#include <ktextedit.h>
#include <kpushbutton.h>
#include <klocale.h>

#include <qcheckbox.h>
#include <qtabwidget.h>

/*!
\brief Default constructor
\param D device to open the dialog of
\param parent parent of the dialog (passed to inheritance class)
\param name name of the dialog (passed to inheritance class)

This function create the dialog for the device, and also opens the sessions to
the served access method of the device itself (snmp, serial, telnet).
*/
DeviceWidget::DeviceWidget(ATMOS::Device *D, QWidget *parent, const char *name)
 : uidevicewidget(parent, name)
{
	QWidget *dummy = tabs->page(0);

	t = NULL;
	telnet = NULL;
	serial = NULL;
	snmpSummary = NULL;
	snmpIfaces = NULL;
	snmpIPSetup = NULL;
	snmpIPStats = NULL;
	
	dev = D;
	if ( dev->hasSNMP() )
	{
		snmpSummary = new uiSnmpSummary(this, "snmp_summary");
		snmpIfaces = new uiSnmpInterfaces(this, "snmp_interfaces");
		connect( snmpIfaces->checkHideDownIFs, SIGNAL(toggled(bool)), this, SLOT(listInterfaces()) );
		connect( snmpIfaces->viewInterfaces, SIGNAL(executed(QListViewItem *)), this, SLOT(executedInterface(QListViewItem* )) );
		snmpIfaces->viewInterfaces->setSorting(-1);
		mutexViewInterfaces = new QMutex();
		
		snmpIPSetup = new uiSnmpIP(this, "snmp_ip");
		snmpIPStats = new uiSnmpIPStats(this, "snmp_ip_stats");
		
		tabs->insertTab( snmpSummary, i18n("Summary"), 0 );
		tabs->insertTab( snmpIfaces, i18n("Interfaces"), 1 );
		tabs->insertTab( snmpIPSetup, i18n("IP Settings"), 2 );
		tabs->insertTab( snmpIPStats, i18n("IP Statistics"), 3 );
		
		dev->snmpOpenSession();
		t = new SNMPThread(this, dev);
	}
	
	if ( dev->hasSerial() || dev->hasTelnet() )
	{
		// Log and avents are supported only if there's a command line interface accessible
		logEvents = new EventsLogWidget(this, "events_log");
		logDevice = new uiDeviceLog(this, "device_log");
		connect( logDevice->btnFetchLog, SIGNAL(clicked()), this, SLOT(fetchLog()) );
		
		tabs->addTab(logEvents, i18n("Event log"));
		tabs->addTab(logDevice, i18n("Device log"));
	}
	
	if ( dev->hasSerial() )
		serialOpenInterface();
	
	if ( dev->hasTelnet() )
		telnetOpenInterface();
	else
		delete btnOpenTelnet;

	if ( t ) t->start();
	
	delete dummy;
}

DeviceWidget::~DeviceWidget()
{
	delete mutexViewInterfaces;
	
	if ( t )
		t->stopped = true;
	
	delete telnet;
	delete serial;
}

void DeviceWidget::goBack()
{
}

//! Opens the serial interface
void DeviceWidget::serialOpenInterface()
{
	dev->serialOpenInterface();
	connect( dev->serialInterface(), SIGNAL(newInformative(const QString& )), this, SLOT(serialNewInformative(const QString& )) );
	
	// We need to know only if it's initialized correctly, not if fails to initialize
	// connect( dev->serialInterface(), SIGNAL(initComplete()), this, SLOT(serialInitComplete()) );
	// Actually, serial device is always initialized, as soon as i can think about it, so initialize it always!
	serialInitComplete();
}

//! Prepare the serial interface and adds it's CLIWidget instance
void DeviceWidget::serialInitComplete()
{
	dev->serialInterface()->sendLine(" ");
	serial = new CLIWidget(this, "serial");
	
	connect( dev->serialInterface(), SIGNAL(newLines(const QStringList& )), serial, SLOT(dataReceived(const QStringList& )) );
	connect( serial, SIGNAL(commandSent(const QString& )), dev->serialInterface(), SLOT(sendLine(const QString& )) );
	
	tabs->addTab( serial, i18n("Serial console") );
}

/*!
\brief Output a new event in the event output tab
\param msg Message to be output

\note This function is a slot form CLInterface::newInformative() signal
*/
void DeviceWidget::serialNewInformative(const QString &msg)
{
	logEvents->parseMessage(msg, EventsLogWidget::siTelnet);
}

//! Opens the telnet interface
void DeviceWidget::telnetOpenInterface()
{
	if ( dev->telnetInterface() )
		telnetInitComplete();
	
	btnOpenTelnet->setEnabled(false);
	
	dev->telnetOpenInterface();
	connect( dev->telnetInterface(), SIGNAL(newInformative(const QString& )), this, SLOT(telnetNewInformative(const QString& )) );
	
	connect( dev->telnetInterface(), SIGNAL(initComplete()), this, SLOT(telnetInitComplete()) );
	connect( dev->telnetInterface(), SIGNAL(initFailed()), this, SLOT(telnetInitFailed()) );
}

//! Prepare the telnet interface and its CLIWidget
void DeviceWidget::telnetInitComplete()
{
	telnet = new CLIWidget(this, "telnet");

	connect( dev->telnetInterface(), SIGNAL(newLines(const QStringList& )), telnet, SLOT(dataReceived(const QStringList& )) );
	connect( telnet, SIGNAL(commandSent(const QString& )), dev->telnetInterface(), SLOT(sendLine(const QString& )) );
	
	tabs->addTab( telnet, i18n("Telnet console") );
}

//! Removes the widget when the telnet interface is closed
void DeviceWidget::telnetInitFailed()
{
	logEvents->logMessage(i18n("Telnet&gt; session closed"), EventsLogWidget::wlWarning);
	btnOpenTelnet->setEnabled(true);
	delete telnet;
	telnet = NULL;
}

/*!
\brief Output a new event in the event output tab
\param msg Message to be output

\note This function is a slot form CLInterface::newInformative() signal
*/
void DeviceWidget::telnetNewInformative(const QString &msg)
{
	QRegExp re("\\*\\*\\* telnet session will close in \\d+s\\r*\\n*");
	if ( re.search(msg) != -1 )
		telnetKeepAlive();
	
	logEvents->parseMessage(msg, EventsLogWidget::siTelnet);
}

//! Check if we need to keep alive telnet sessions, and in this case sends
//! a foo command to the telnet interface
void DeviceWidget::telnetKeepAlive()
{
	// Shouldn't be called without these pointers
	if ( ! telnet || ! dev->telnetInterface() )
		return;

	if ( ATMOSsettings::keepAliveTelnet() )
	{
		QString prompt = telnet->output->text( telnet->output->paragraphs() -1 );
		QRegExp re( dev->IP() + " ?(.*)>" );
		
		// If the prompt isn't the default one, there's something strange
		// Note also that only serial console has DEBUG> prompt
		if ( re.search( prompt ) == -1 )
			return;
		
		dev->telnetInterface()->sendLine("version\r");
	}
}

/*!
\brief Do verbose output of message strings from the device
\param interface An identifier for the command line interface (Serial or Telnet)
\param msg The message string from the device

This function also take action for some kinds of messages (like telnet closing session).
All the verbose output is translated via tr()
*/
void DeviceWidget::verboseOutput(const QString &interface, const QString &msg)
{
}

void DeviceWidget::trueListInterfaces()
{
	// If not 1213 compliant simply doesn't display anything
	KSNMP::Device1213 *snmpDev = reinterpret_cast<KSNMP::Device1213 *>(dev->snmp());
	if ( ! snmpDev ) return;
	
	snmpIfaces->viewInterfaces->clear();
	int maxIf = snmpDev->requestValue("ifNumber.0").toInt();
	
	snmpDev->readInterfaces();
	
	for(int i = 1; i <= maxIf; i++)
	{
		KSNMP::Device1213::Interface IF = snmpDev->getInterface(i);
		new KListViewItem(
			snmpIfaces->viewInterfaces,
			snmpIfaces->viewInterfaces->lastItem(),
			QString::number(IF.ifIndex),
			IF.ifDescr,
			KSNMP::Device1213::operStatus(IF.ifOperStatus),
			humanizeSpeed(IF.ifSpeed),
			humanizeOctets(IF.ifInOctets),
			humanizeOctets(IF.ifOutOctets)
			);
	}
}

/*!
\brief Fill the viewInterfaces KListView with the data from the interfaces
*/
void DeviceWidget::listInterfaces()
{
	// If not 1213 compliant simply doesn't display anything
	KSNMP::Device1213 *snmpDev = reinterpret_cast<KSNMP::Device1213 *>(dev->snmp());
	if ( ! snmpDev ) return;
	
	mutexViewInterfaces->lock();

	trueListInterfaces();
	
	mutexViewInterfaces->unlock();
}

/*!
\brief Opens the Interface Statistics for the interface double-clicked
\note This function is called when an interface line in viewInterfaces is 
	clicked or double-clicked (depends on system preferences).
\param lwi Item executed
*/
void DeviceWidget::executedInterface(QListViewItem *lwi)
{
	InterfaceStats *is = new InterfaceStats(dev, lwi->text(0).toUInt(), this);
	is->exec();
	delete is;
}

void DeviceWidget::fetchLog()
{
	dev->fetchLog();
}

//! Called when the Get Statistics of IP Statistics page is clicked
void DeviceWidget::getIPStats()
{
	
}

#include "devicewidget.moc"
