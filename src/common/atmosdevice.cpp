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
#include "atmosdevice.h"
#include "utils.h"
#include "serialinterface.h"
#include "telnetinterface.h"
#include "atmosmodem.h"
#include "atmosrouter.h"
#include "accesspoint.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_TFTP
#include <tftp/server.h>
#include <tftp/serverwrite.h>
#include <tftp/serverread.h>
#endif

#include <libksnmp/mib1213.h>
#include <libksnmp/session.h>
#include <libksnmp/session1.h>
#include <libksnmp/session2c.h>

#include <qbuffer.h>
#include <qtextstream.h>
#include <qptrlist.h>
#include <qfile.h>

#include <kstandarddirs.h>
#include <kdebug.h>
#include <kwallet.h>

namespace ATMOS {

static QPtrList<Device> devices;

/**
@brief Gets the device's list

This function is used to avoid export iterators from the Device class.
The returned reference is constant, so const_iterators must be used.
Anyway, the device themselves aren't constants, so can be modified.
*/
const QPtrList<Device> &Device::getDevices()
{
	return devices;
}

/**
@brief Appends a device to the list
@param dev Device to append
@retval true The device is valid and it's been added to the devices list.
@retval false The device is not valid and was deleted.

This function is used to add devices to the @c devices list, without need
to make it visible to other units.

This function takes also care of delete the device is isn't valid, so
the @p dev parameter must be forget right after calling this function if
@c false is returned.
*/
bool Device::addDevice(Device *dev)
{
	if ( dev->validDevice() )
	{
		devices.append(dev);
		return true;
	} else {
		delete dev;
		return false;
	}
}

/**
@brief Removes a device to the list
@param dev Device tor emove

This function is used to remvoe devices from the @c devices list, without need
to make it visible to other units.

This function takes care of everything, so the @p dev parameter must be forget
right after calling this function.

@todo Password for devices should be removed instantly, or should remain until
	saving?
*/
void Device::removeDevice(Device *dev)
{
	delete dev;
	devices.remove(dev);
}

void Device::loadDevices()
{
	devices.clear();
	Driver::loadDrivers();

	QDomDocument domDevices("domdevices");
	QFile file( locateLocal("appdata", QString::fromLatin1("devices.xml")) );
	if ( !file.open( IO_ReadOnly ) )
		return;
	if ( !domDevices.setContent( &file ) )
	{
		file.close();
		return;
	}
	file.close();
	
	QDomElement docElem = domDevices.documentElement();
	
	QDomNode n = docElem.firstChild();
	while ( ! n.isNull() )
	{
		QDomElement e = n.toElement();
		if ( e.isNull() )
		{
			n = n.nextSibling();
			continue;
		}
			
		ATMOS::Device *dev = NULL;
		QString devtype = e.tagName();
		if ( devtype == "router" )
			dev = new Router();
		else if ( devtype == "modem" )
			dev = new Modem();
		else if ( devtype == "accesspoint" )
			dev = new AccessPoint();
		else {
			kdFatal() << "Tag " << devtype << " not known. Skipping." << endl;
			n = n.nextSibling();
			continue;
		}
		
		dev->m_type = e.attribute("driver");
		dev->m_IP = e.attribute("ip");
		
		QDomNode n2 = e.firstChild();
		while ( ! n2.isNull() )
		{
			QDomElement e2 = n.toElement();
			if ( e2.isNull() )
			{
				n2 = n2.nextSibling();
				continue;
			}
			
			QString opt = e2.tagName();
			if ( opt == "snmp" ) {
				getWallet()->readPassword("snmpCommunity_" + dev->m_IP, dev->m_snmpCommunity);
				
				dev->m_snmpPort = e2.attribute("port", "161").toUInt();
				
				dev->setSNMP(true);
				dev->setSNMPWrite(e2.attribute("write", "0").toUInt());
			} else if ( opt == "telnet" ) {
				if ( e2.attribute("password", "0").toUInt() )
				{
					getWallet()->readPassword("telnet_" + dev->m_IP, dev->m_telnetPassword);
					dev->setTelnetHasPwd(true);
				}
				
				dev->m_telnetPort = e2.attribute("port", "23").toUInt();
				
				dev->setTelnet(true);
			} else if ( opt == "serial" ) {
				dev->m_serialDevice = e2.attribute("device", "/dev/ttyS0");
				
				dev->setSerial(true);
			}else {
				kdFatal() << "Tag " << devtype << " not known. Skipping." << endl;
				n2 = n2.nextSibling();
				continue;
			}
			
			n2 = n2.nextSibling();
		}
		
		if ( dev->validDevice() )
			devices.append(dev);
		else
			delete dev;
		
		n = n.nextSibling();
	}
}

/**
@brief Save devices

This function is used to save all the devices into the XML datafile in user's home.
*/
void Device::saveDevices()
{
	QFile file( locateLocal("appdata", QString::fromLatin1("devices.xml")) );
	if ( !file.open( IO_WriteOnly ) )
		return;
	QTextStream ts(&file);
	
	QDomDocument domDevices("domdevices");
	QDomElement docEl = domDevices.createElement("devices");
	domDevices.appendChild(docEl);
	
	const QPtrList<Device>::const_iterator devend = devices.end();
	for(QPtrList<Device>::const_iterator it = devices.begin(); it != devend; ++it)
	{
		QDomElement devEl;
		QString devClass = (*it)->className();
		if ( devClass == "ATMOS::Router" )
			devEl = domDevices.createElement("router");
		else if ( devClass == "ATMOS::Modem" )
			devEl = domDevices.createElement("modem");
		else if ( devClass == "ATMOS::AccessPoint" )
			devEl = domDevices.createElement("accesspoint");
		else 
			continue; // Skip invalid devices
		
		devEl.setAttribute("driver", (*it)->m_type);
		devEl.setAttribute("ip", (*it)->m_IP);
		
		if ( (*it)->hasSNMP() )
		{
			QDomElement snmpEl = domDevices.createElement("snmp");
			devEl.appendChild(snmpEl);
			snmpEl.setAttribute("port", (*it)->m_snmpPort);
			snmpEl.setAttribute("write", (int)((*it)->SNMPWrite()));
			
			getWallet()->writePassword("snmpCommunity_" + (*it)->m_IP, (*it)->m_snmpCommunity);
		}
		
		if ( (*it)->hasTelnet() )
		{
			QDomElement telnetEl = domDevices.createElement("telnet");
			devEl.appendChild(telnetEl);
			telnetEl.setAttribute("port", (*it)->m_telnetPort);
			if ( (*it)->hasTelnetPwd() )
			{
				telnetEl.setAttribute("password", 1);
				getWallet()->writePassword("telnet_" + (*it)->m_IP, (*it)->m_telnetPassword);
			} else
				telnetEl.setAttribute("password", 0);
		}
		
		if ( (*it)->hasSerial() )
		{
			QDomElement serialEl = domDevices.createElement("serial");
			devEl.appendChild(serialEl);
			serialEl.setAttribute("device", (*it)->m_serialDevice);
		}
		
		docEl.appendChild(devEl);
	}
	
	ts << domDevices.toString();
	
	file.close();
}

/*!
\brief Default constructor

This function reset all the values of the variables, setting the string to
QString::null, pointers to NULL, flags and ports to 0s.
*/
Device::Device()
{
	m_flags = 0;
	m_IP = QString::null;
	m_type = QString::null;
	
	m_telnetPassword = QString::null;
	m_telnetPort = 23;
	m_telnetInterface = NULL;
	
	m_serialDevice = QString::null;
	m_serialInterface = NULL;
	
	m_snmpCommunity = "public";
	m_snmpPort = 161;
	m_snmp = NULL;
	m_snmpSession = NULL;
	
	m_logBuffer = new QBuffer();
	m_logStream = new QTextStream(m_logBuffer);

#ifdef HAVE_TFTP
	TFTP::Server::WriteSession::remapFilename("//isfs/eventlog", m_logBuffer);
#endif
}

Device::~Device()
{
	delete m_snmp;
	delete m_snmpSession;
	delete m_serialInterface;
	delete m_logStream;
	delete m_logBuffer;
#ifdef HAVE_TFTP
	TFTP::Server::WriteSession::unmapFilename("//isfs/eventlog");
#endif
}

/*!
\brief Checks if the capabilities set in the device meet the ones in the
driver.

This function removes flags not compatible with the driver selected for the
device.
*/
void Device::checkCapabilities()
{
	const Driver *drv = Driver::getDriver( m_type );
	if ( ! drv )
	{
		qWarning("Error at checkCapabilities(), maybe devices loaded before drivers?");
		return;
	}
	
	if ( !drv->supportTelnet() )
	{
		m_flags &= ~(f_hasTelnet|f_hasTelnetPwd);
		m_telnetPassword = QString::null;
		m_telnetPort = 0;
	}
	if ( !drv->supportSerial() )
	{
		m_flags &= ~f_hasSerial;
		m_serialDevice = QString::null;
	}
	if ( !drv->supportSNMP() )
	{
		m_flags &= ~(f_hasSNMP|f_SNMPWrite);
		m_snmpCommunity = QString::null;
		m_snmpPort = 0;
	}

	if ( m_flags & f_hasTelnet &&
		( m_telnetPort == 0 || ( m_flags & f_hasTelnetPwd && m_telnetPassword.isEmpty() ) ) )
		m_flags &= ~f_hasTelnet;

	if ( m_flags & f_hasSerial &&
		( m_serialDevice.isEmpty() ) )
		m_flags &= ~f_hasSerial;

	if ( m_flags & f_hasSNMP &&
		( m_snmpCommunity.isEmpty() || m_snmpPort == 0 ) )
		m_flags &= ~f_hasSNMP;
}

/*!
\brief Checks if the device is a fully-qualified Device or not
\return true if it's fully-qualified
*/
bool Device::validDevice()
{
	checkCapabilities();
	return Driver::existsDriver( m_type ) && !m_IP.isEmpty() && validIP(m_IP) && m_flags;
}

/*!
\brief Gets the preferred command line interface available
\return A pointer to the preferred opened interface, or NULL if only SNMP is available.

Maybe in a future can be decided, for the moment, the sequence is serial-telnet.
This is because the serial works also when the net is down.
*/
CLInterface *Device::getCLI()
{
	if ( hasSerial() )
		return serialInterface();
	if ( hasTelnet() )
		return telnetInterface();
	
	return NULL;
}

//! Opens SNMP sessions (read and write) to the device (if it supports it)
void Device::snmpOpenSession()
{
	const Driver *drv = Driver::getDriver( m_type );
	assert(drv);
	
	if ( ! hasSNMP() || m_snmp )
		return;
	
	bool done = false;
	switch( drv->snmpVersion() )
	{
	case Driver::snmpV1:
		{
			KSNMP::Session1 *tmp_sess = new KSNMP::Session1(m_IP, m_snmpCommunity);
			done = tmp_sess->open();
			m_snmpSession = tmp_sess;
		}
		break;
	case Driver::snmpV2c:
		{
			KSNMP::Session2c *tmp_sess = new KSNMP::Session2c(m_IP, m_snmpCommunity);
			done = tmp_sess->open();
			m_snmpSession = tmp_sess;
		}
		break;
	case Driver::snmpV3:
		/// @todo Need to display an error dialog
		return;
	}
	
	if ( ! done )
	{
		delete m_snmpSession;
		m_snmpSession = NULL;
		return;
	}

	switch(drv->snmpMIB())
	{
	case Driver::rfc1213:
		m_snmp = new KSNMP::Device1213(m_snmpSession);
		break;
	default:
		m_snmp = new KSNMP::Device(m_snmpSession);
	}
}

/// Closes the SNMP session
void Device::snmpCloseSession()
{
	delete m_snmp;
	m_snmpSession->close();
	delete m_snmpSession;
}

//! Opens Serial interface (connection) to the device (if it supports it)
void Device::serialOpenInterface()
{
	if ( ! hasSerial() || m_serialInterface != NULL )
		return;
	
	m_serialInterface = new SerialInterface(this);
	connect(m_serialInterface, SIGNAL(destroyed(QObject *obj)), this, SLOT(deletedInterface(QObject* )));
}

//! Opens Telnet interface (connection) to the device (if it supports it)
void Device::telnetOpenInterface()
{
	if ( ! hasTelnet() || m_telnetInterface != NULL )
		return;
	
	m_telnetInterface = new TelnetInterface(this);
	connect(m_telnetInterface, SIGNAL(destroyed(QObject *obj)), this, SLOT(deletedInterface(QObject* )));
}

//! Fetches the router's event log from the router
void Device::fetchLog()
{
	// We need an ATMOS router to fetch the eventlog
	if ( ! hasTelnet() && ! hasSerial() )
		return;
#ifdef HAVE_TFTP
	getCLI()->tftpConnect(QHostAddress());
	getCLI()->tftpGetFile("//isfs/eventlog");
#else
	qWarning("Not implemented without TFTP support");
#endif
}

/**
@brief Takes care of NULL-ize deleted interfaces
@param obj Deleted object

This slot is called whenever one interface is deleted. This removes the need to give
them the friend status, and avoid using <pre>delete this</pre> syntax which can lead
to bugs.
@sa http://developer.kde.org/documentation/other/mistakes.html
*/
void Device::deletedInterface(QObject *obj)
{
	if ( reinterpret_cast<TelnetInterface*>(obj) == m_telnetInterface )
		m_telnetInterface = NULL;
	if ( reinterpret_cast<SerialInterface*>(obj) == m_serialInterface )
		m_serialInterface = NULL;
}

};

#include "atmosdevice.moc"
