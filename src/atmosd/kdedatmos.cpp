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

#include "kdedatmos.h"

#include <ksystemtray.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kpopupmenu.h>
#include <dcopclient.h>

#include <qtimer.h>

#include <libksnmp/session.h>

#include <settings.h>
#include <atmosdevice.h>

KDEDAtmos::KDEDAtmos(const QCString& name) :
	KDEDModule(name),
	m_moduleName(name)
{
	kdDebug() << "KDEDAtmos::KDEDAtmos(" << name << ")" << endl;
	
	KSNMP::init("kdedatmosd");
	
	m_sysTray = new KSystemTray(NULL, "atmosd_systray");
	m_sysTray->setPixmap( m_sysTray->loadIcon("gear") );
	m_sysTray->contextMenu()->changeTitle(0, "ATMOSphere");
	connect(m_sysTray, SIGNAL(quitSelected()), this, SLOT(unload()));
	m_sysTray->show();
	
	m_checkTimer = new QTimer(this, "atmosd_checkdevicetimer");
	connect(m_checkTimer, SIGNAL(timeout()), this, SLOT(checkDevices()));
	m_checkTimer->start(ATMOSsettings::monitorCheckTimeout());
	
	openSNMP();
}

KDEDAtmos::~KDEDAtmos()
{
	kdDebug() << "KDEDAtmos::~KDEDAtmos()" << endl;
	delete m_sysTray;
	delete m_checkTimer;
	
	KSNMP::shutdown();
}

void KDEDAtmos::unload()
{
	kdDebug() << "KDEDAtmos::unload()" << endl;
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << m_moduleName;
	if ( ! KApplication::dcopClient()->send("kded", "kded", "unloadModule(QCString)", data) )
		kdWarning() << "Error requesting unload to KDED" << endl;
}

void KDEDAtmos::reloadSettings()
{
	kdDebug() << "KDEDAtmos::reloadSettings()" << endl;

	m_checkTimer->changeInterval(ATMOSsettings::monitorCheckTimeout());
}

void KDEDAtmos::checkDevices()
{
	kdDebug() << "KDEDAtmos::idle()" << endl;
	bool allup = true;

	const QPtrList<ATMOS::Device> &devices = ATMOS::Device::getDevices();
	const QPtrList<ATMOS::Device>::const_iterator devend = devices.end();
	for( QPtrList<ATMOS::Device>::const_iterator it = devices.begin(); it != devend; ++it )
	{
		if ( (*it)->hasSNMP() )
		{
			QVariant var; uint32_t bogus;
			if ( ! (*it)->snmpSession()->getVariable("sysUpTime.0", var, bogus) )
				allup = false;
			else
				kdDebug() << (*it)->IP() << " uptime " << var.toString() << endl;
		} else
			kdDebug() << "Device does not support SNMP." << endl;
	}
	
	if ( ! allup )
		m_sysTray->setPixmap( m_sysTray->loadIcon("stop") );
	else
		m_sysTray->setPixmap( m_sysTray->loadIcon("gear") );
}

void KDEDAtmos::openSNMP()
{
	const QPtrList<ATMOS::Device> &devices = ATMOS::Device::getDevices();
	const QPtrList<ATMOS::Device>::const_iterator devend = devices.end();
	for( QPtrList<ATMOS::Device>::const_iterator it = devices.begin(); it != devend; ++it )
	{
		if ( (*it)->hasSNMP() )
			(*it)->snmpOpenSession();
	}
}

extern "C"
{
    KDEDModule *create_atmosd( const QCString & name ) {
        return new KDEDAtmos( name );
    }
}

#include "kdedatmos.moc"
