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

#include <qlayout.h>
#include <qcheckbox.h>

#include <klocale.h>
#include <kglobal.h>
#include <kparts/genericfactory.h>
#include <klistbox.h>
#include <klistview.h>
#include <knuminput.h>

#include "kcmatmosphere.h"
#include "kcmmain.h"
#include <atmosdriver.h>
#include <settings.h>

typedef KGenericFactory<kcmatmosphere, QWidget> kcmatmosphereFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_kcmatmosphere, kcmatmosphereFactory("kcmkcmatmosphere"))

/**
@brief Default constructor
@param parent Parent widget (passed to KCModule class)
@param name Widget name (passed to KCModule class)
@param sl Unknown, not used

This function create the KcmMain instance and loads the configuration file.
*/
kcmatmosphere::kcmatmosphere(QWidget *parent, const char *name, const QStringList& sl)
    : KCModule(parent, name), myAboutData(0)
{
	lay = new QBoxLayout(this, QBoxLayout::TopToBottom);
	lay->setAutoAdd(true);
	mw = new KcmMain(this);
	
	load();
};

kcmatmosphere::~kcmatmosphere()
{
}

//! Loads the preferences into the widgets fields
void kcmatmosphere::load()
{
	mw->listXMLs->clear();
	QStringList drivers = ATMOSsettings::externalDrivers();
	const QStringList::const_iterator enddrv = drivers.end();
	for(QStringList::const_iterator it = drivers.begin(); it != enddrv; ++it)
		mw->listXMLs->insertItem(*it);
	
	mw->loadDevices();
	
	mw->spinReauditSNMP->setValue( ATMOSsettings::reReadDevice() );
	mw->spinReauditIFStats->setValue( ATMOSsettings::reReadInterface() );
	mw->checkShowRawEvents->setChecked( ATMOSsettings::logRawEvents() );
	mw->checkTelnetKeepAlive->setChecked( ATMOSsettings::keepAliveTelnet() );
}

/**
@brief Loads the default values 
@todo Write it?
*/
void kcmatmosphere::defaults()
{
	// insert your default settings code here...
	emit changed(true);
}

//! Saves the settings and reload the drivers
void kcmatmosphere::save()
{
	ATMOSsettings::writeConfig();
	ATMOS::Driver::loadDrivers();
	mw->loadDevices();
	emit changed(true);
}

/*!
\brief Standard function to tell which buttons to show
\return KCModule::Apply
*/
int kcmatmosphere::buttons()
{
	return KCModule::Apply;
}

/// Function called when the configuration is changed
void kcmatmosphere::configChanged()
{
	mw->loadDevices();
	// insert your saving code here...
	emit changed(true);
}

/*!
\brief Standard function to tell the help message to use
\return the internationalized string of the KCM help
*/
QString kcmatmosphere::quickHelp() const
{
	return i18n("Configure here all your ATMOS driver peripherals, like routers, access points or switches");
}

/**
@brief DCOP Method for reload configuration file

This function is called when other pieces of atmosphere changed the settings.
@todo Need to be wrote :)
*/
void kcmatmosphere::reloadConfig()
{
}

#include "kcmatmosphere.moc"
