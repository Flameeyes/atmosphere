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

#include "snmpthread.h"
#include "devicewidget.h"
#include <atmosdevice.h>
#include <settings.h>
#include <utils.h>

#include "uisnmpsummary.h"
#include "uisnmpinterfaces.h"
#include "uisnmpip.h"
#include "uisnmpipstats.h"

#include <libksnmp/mib1213.h>

#include <qlabel.h>
#include <qcheckbox.h>
#include <qapplication.h>

#include <klineedit.h>
#include <klocale.h>

/*!
\brief Default constructor
\param W dialog to fill with device information
\param D device to take the information from
*/
SNMPThread::SNMPThread(DeviceWidget *W, ATMOS::Device *D)
 : QThread(), dev(D), dw(W)
{
	stopped = true;
}

SNMPThread::~SNMPThread()
{
}

/*!
\brief Threaded method

This is the method run by the thread in parallel with the main application thread.
This function keep running as long as the SNMPThread::stopped variable is false, waiting
for the configured interval after each read.
Actually, the waits are divided in four, so we don't block for all the interval before
close the dialog.
*/
void SNMPThread::run()
{
	do
	{
		KSNMP::Device1213 *snmpDev = reinterpret_cast<KSNMP::Device1213 *>(dev->snmp());
		if ( ! snmpDev )
		{
			dw->labelStatus->setText(tr("Unable to open SNMP session"));
			return;
		}
		
		snmpDev->cacheSystem();
		snmpDev->cacheIpSettings();
		
		dw->snmpSummary->lineDescr->setText( snmpDev->requestValue("sysDescr.0").toString() );
		dw->snmpSummary->lineUptime->setText( humanizeTimeTicks( snmpDev->requestValue("sysUpTime.0").toUInt() ) );
		dw->snmpSummary->lineContact->setText( snmpDev->requestValue("sysDescr.0").toString() );
		dw->snmpSummary->lineName->setText( snmpDev->requestValue("sysName.0").toString() );
		dw->snmpSummary->lineLocation->setText( snmpDev->requestValue("sysLocation.0").toString() );
		
		dw->snmpIPSetup->lineIpTTL->setText( snmpDev->requestValue("ipDefaultTTL.0").toString() );
		dw->snmpIPSetup->checkIpForwarding->setChecked( snmpDev->requestValue("ipForwarding.0").toBool() );
		
		dw->mutexViewInterfaces->lock();
		dw->trueListInterfaces();
		dw->mutexViewInterfaces->unlock();
		
		dw->labelStatus->setText(i18n("Got SNMP data"));
		
		int interval = ATMOSsettings::reReadDevice()/4;
		msleep( interval ); if ( stopped ) return;
		msleep( interval ); if ( stopped ) return;
		msleep( interval ); if ( stopped ) return;
		msleep( interval );
	} while ( ! stopped );
}
