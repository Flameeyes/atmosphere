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
#include "kcmmain.h"
#include "kcmatmosphere.h"
#include "adddevice.h"
#include <settings.h>
#include <atmosdevice.h>
#include <atmosdriver.h>
#include <atmosrouter.h>
#include <atmosmodem.h>
#include <accesspoint.h>


#include <klineedit.h>
#include <knuminput.h>
#include <kcombobox.h>
#include <kmessagebox.h>
#include <kurlrequester.h>
#include <klistbox.h>
#include <klistview.h>
#include <knuminput.h>
#include <klocale.h>

#include <qgroupbox.h>
#include <qdir.h>
#include <qcheckbox.h>

KcmMain::KcmMain(QWidget *parent, const char *name)
 : uikcmmain(parent, name)
{
	kcm = reinterpret_cast<kcmatmosphere*>(parent);
}

KcmMain::~KcmMain()
{
}

/*!
\brief Loads the ATMOS devices

This function is called to fill the list of devices with the devices read
from the config file
*/
void KcmMain::loadDevices()
{
	listDevices->clear();
	devicemap.clear();
	const QPtrList<ATMOS::Device> &devices = ATMOS::Device::getDevices();
	const QPtrList<ATMOS::Device>::const_iterator devend = devices.end();
	for( QPtrList<ATMOS::Device>::const_iterator it = devices.begin(); it != devend; ++it )
	{
		QString accessMethod;
		if ( (*it)->hasTelnet() ) accessMethod += i18n("Telnet ");
		if ( (*it)->hasSerial() ) accessMethod += i18n("Serial ");
		if ( (*it)->hasSNMP() ) accessMethod += i18n("SNMP");
		
		KListViewItem *lvi = new KListViewItem( listDevices, (*it)->IP(), (*it)->driver()->name(), accessMethod);
		devicemap[lvi] = *it;
	}
}

/**
@brief Add new ATMOS device

This function is called when the Add Push Button on the widget is
pushed.
*/
void KcmMain::addDevice()
{
	AddDevice *dialog = new AddDevice(this);
	if ( dialog->exec() )
	{
		ATMOS::Device *dev = NULL;
		
		const ATMOS::Driver *drv = ATMOS::Driver::getDriver(dialog->typeSelected());
		switch ( drv->deviceClass() )
		{
		case ATMOS::Driver::dcRouter:
			dev = new ATMOS::Router();
			break;
		case ATMOS::Driver::dcModem:
			dev = new ATMOS::Modem();
			break;
		case ATMOS::Driver::dcAccessPoint:
			dev = new ATMOS::AccessPoint();
			break;
		}
		
		dev->setType( dialog->typeSelected() );
		dev->setIP( dialog->lineDeviceIP->text() );
		if ( dialog->boxTelnet->isChecked() )
		{
			dev->setTelnetPort( dialog->spinTelnetPort->value() );
			dev->setTelnetPassword( dialog->lineTelnetPwd->text() );
			dev->setTelnet(true);
			dev->setTelnetHasPwd( dialog->checkTelnetPassword->isChecked() );
		}
		if ( dialog->boxSerial->isChecked() )
		{
			dev->setSerialDevice( dialog->urlSerialDevice->url() );
			dev->setSerial(true);
		}
		if ( dialog->boxSNMP->isChecked() )
		{
			dev->setSnmpPort( dialog->spinSNMPPort->value() );
			dev->setSnmpCommunity( dialog->lineSNMPCommunity->text() );
			dev->setSnmpCommunity( dialog->lineSNMPCommunity->text() );
			dev->setSNMP(true);
		}
		
		if ( ATMOS::Device::addDevice(dev) )
			kcm->configChanged();
	}
	delete dialog;
}

/*!
\brief Removes an ATMOS device

This function is called when the Remove Push Button on the widget is
clicked.
*/
void KcmMain::removeDevice()
{
	KListViewItem *lvi = reinterpret_cast<KListViewItem*>(listDevices->selectedItem());
	if ( ! lvi )
		return;
	DeviceMap::iterator it = devicemap.find(lvi);
	if ( it == devicemap.end() )
		delete lvi;
	else
	{
		ATMOS::Device::removeDevice(*it);
		kcm->configChanged();
	}
}

/*!
\brief Add an XML datafile

This function is called when the Add (xml) Push Button on the widget is
clicked.
*/
void KcmMain::addXML()
{
	QStringList xmldrivers = ATMOSsettings::externalDrivers();
	if ( xmldrivers.find(urlXML->url()) != xmldrivers.end() )
		return;
	
	xmldrivers.push_back(urlXML->url());
	listXMLs->insertItem(urlXML->url());
	
	ATMOSsettings::setExternalDrivers(xmldrivers);
	kcm->configChanged();
}

/*!
\brief Removes an XML datafile

This function is called when the Remove (xml) Push Button on the widget is
clicked.
*/
void KcmMain::removeXML()
{
	if ( listXMLs->currentText() == QString::null )
		return;
	
	QString toremove = listXMLs->currentText();
	listXMLs->removeItem( listXMLs->currentItem() );
	
	QStringList xmldrivers = ATMOSsettings::externalDrivers();
	QStringList::iterator it = xmldrivers.find( toremove );
	if ( it != xmldrivers.end() )
		xmldrivers.erase(it);
	
	ATMOSsettings::setExternalDrivers(xmldrivers);
	kcm->configChanged();
}

/*!
\brief XML-Url changed

This function is called when the URL requester for XMLs in the widget is
chagned
*/
void KcmMain::urlXMLChanged(const QString &url)
{
	btnAddXML->setEnabled(QDir::root().exists(url));
}

/*!
\brief Changed SNMP reaudit interval

This function is called whenever the reaudit spinbox is changed
*/
void KcmMain::snmpReauditChanged()
{
	ATMOSsettings::setReReadDevice(spinReauditSNMP->value());
	kcm->configChanged();
}

/*!
\brief Changed Interface Statistics reaudit interval

This function is called whenever the reaudit spinbox is changed
*/
void KcmMain::ifStatsReauditChanged()
{
	ATMOSsettings::setReReadInterface(spinReauditIFStats->value());
	kcm->configChanged();
}

/*!
\brief Toggled Show Raw Events Checkbox

This function is called whenever the 'Show Raw Events' checkbox is toggled
*/
void KcmMain::toggledShowRawEvents()
{
	ATMOSsettings::setLogRawEvents(checkShowRawEvents->isChecked());
	kcm->configChanged();
}

/*!
\brief Toggled Telnet Keep Alive Checkbox

This function is called whenever the 'Telnet Keep Alive' checkbox is toggled
*/
void KcmMain::toggledTelnetKeepAlive()
{
	ATMOSsettings::setKeepAliveTelnet(checkTelnetKeepAlive->isChecked());
	kcm->configChanged();
}

#include "kcmmain.moc"
