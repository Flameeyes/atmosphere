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
#include "atmosdriver.h"
#include "settings.h"
#include "einvaliddriver.h"

#include <qstringlist.h>
#include <qfile.h>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdebug.h>

namespace ATMOS {

Driver::Drivers Driver::drvs;

/*!
\brief Loads the XML drivers
*/
void Driver::loadDrivers()
{
	unloadDrivers();
	
	QStringList xmls = ATMOSsettings::externalDrivers();
	xmls += KGlobal::dirs()->findAllResources("data", "atmosphere/drivers/*.xml", false, true);
	
	for( QStringList::iterator it = xmls.begin(); it != xmls.end(); it++ )
	{
		kdDebug() << "Loading XML datafile: " << *it << endl;
		QDomDocument domDrivers("domdrivers");
		QFile file( *it );
		if ( !file.open( IO_ReadOnly ) )
			continue;
		if ( !domDrivers.setContent( &file ) )
		{
			file.close();
			continue;
		}
		file.close();
		
		QDomElement docElem = domDrivers.documentElement();
		
		QDomNode n = docElem.firstChild();
		while ( ! n.isNull() )
		{
			QDomElement e = n.toElement();
			if ( !e.isNull() )
			{
				try {
					kdDebug() << "Loading driver... ";
					Driver *newdrv = new Driver(e);
					drvs[ newdrv->code() ] = newdrv;
					kdDebug() << "loaded." << endl;
				} catch ( EInvalidDriver &e ) {
					kdDebug() << "found an invalid driver... skipping." << endl;
				}
			}
			
			n = n.nextSibling();
		}
	}
}

/*!
\brief Unload the drivers
*/
void Driver::unloadDrivers()
{
	for(Drivers::iterator it = drvs.begin(); it != drvs.end(); it++)
		delete *it;
	drvs.clear();
}

/*!
\brief Default constructor
\note Protected, so only the loadDrivers() function can create new instances
of this
*/
Driver::Driver(const QDomElement &e)
{
	flags = 0;
	if ( e.nodeName() != "driver" )
		throw EInvalidDriver();
	
	m_code = e.attribute("code", QString::null);
	if ( m_code.isNull() )
		throw EInvalidDriver();
	
	if ( existsDriver(m_code) )
		throw EInvalidDriver();
	
	QString className = e.attribute("class", QString::null);
	if ( className == "router" )
		m_deviceClass = dcRouter;
	else if ( className == "modem" )
		m_deviceClass = dcModem;
	else if ( className == "accesspoint" )
		m_deviceClass = dcAccessPoint;
	else
		throw EInvalidDriver();
		
	QDomNode n = e.firstChild();
	while ( ! n.isNull() )
	{
		QDomElement el = n.toElement();
		if ( el.isNull() ) continue;
			
		if ( el.nodeName() == "manufacturer" ) {
			m_manufacturer = el.text();
			m_manufacturerCode = el.attribute("code", QString::null);
			if ( m_manufacturer.isNull() || m_manufacturerCode.isNull() )
				throw EInvalidDriver();
		} else if ( el.nodeName() == "name" ) {
			m_name = el.text();
			if ( m_name.isNull() )
				throw EInvalidDriver();
		} else if ( el.nodeName() == "serial" ) {
			flags |= f_supportSerial;
			if ( el.attribute("rj12", QString::null) == "rj12" )
				flags |= f_serialRJ12;
		} else if ( el.nodeName() == "telnet" ) {
			flags |= f_supportTelnet;
		} else if ( el.nodeName() == "snmp" ) {
			flags |= f_supportSNMP;
			
			QString s_snmpVersion = el.attribute("version", "1");
			if ( s_snmpVersion == "1" )
				m_snmpVersion = snmpV1;
			else if ( s_snmpVersion == "2c" || s_snmpVersion == "2" )
				m_snmpVersion = snmpV2c;
			else if ( s_snmpVersion == "3" )
				m_snmpVersion = snmpV3;
			else
				throw EInvalidDriver();
			
			QString s_snmpMib = el.attribute("mib", "rfc1213");
			if ( s_snmpMib == "rfc1213" )
				m_snmpMIB = rfc1213;
			else
				throw EInvalidDriver();
		}
			
		n = n.nextSibling();
	}
	
	if ( flags == 0 )
		throw EInvalidDriver();
}

Driver::~Driver()
{
}

const Driver* Driver::getDriver(const QString& code)
{
	return drvs[code];
}

};
