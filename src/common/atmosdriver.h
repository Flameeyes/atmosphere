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
#ifndef ATMOSDRIVER_H
#define ATMOSDRIVER_H

#include <stdint.h>

#include <qmap.h>
#include <qstring.h>
#include <qdom.h>

namespace ATMOS {
	/*!
	\author Flameeyes
	\brief Class representing capabilities and informations about ATMOS driven
	devices.
	
	This class contains the capabilities and the data about the ATMOS driven
	hardware
	*/
	class Driver{
	public:
		typedef QMap<QString,Driver*> Drivers;
		
		//! Version of SNMP supported
		enum SNMPversion {
			snmpV1,		//!< Version 1
			snmpV2c,	//!< Version 2 (community)
			snmpV3		//!< Version 3 (currently unsupported)
		};
		
		enum SNMPmib {
			rfc1213		//!< RFC 1213 MIB (default)
		};
		
		//! Type of device
		enum DeviceClass {
			dcUnknown,	//!< Unknown device (shouldn't be used)
			dcRouter,	//!< Device is a router
			dcModem,	//!< Device is a modem
			dcAccessPoint	//!< Device is an access point
		};
		
	protected:
		//! List of drivers
		static Drivers drvs;
	public:
		static void loadDrivers();
		static void unloadDrivers();
		
		static const Driver* getDriver(const QString& code);
		
		//! Check if a driver exists
		static inline const bool existsDriver(const QString& code)
		{ return drvs.contains(code); }
		
		//! Return the first driver in the list
		static Drivers::iterator firstDriver()
		{ return drvs.begin(); }
		
		//! Return the last driver in the list
		static Drivers::iterator lastDriver()
		{ return drvs.end(); }
		
		static const uint8_t f_supportTelnet	= 0x01;	//!< Support for telnet access
		static const uint8_t f_supportSerial	= 0x02;	//!< Support for serial access
		static const uint8_t f_supportSNMP	= 0x04;	//!< Support for SNMP access
		static const uint8_t f_serialRJ12	= 0x08;	//!< Need RJ12 cable for serial access
		static const uint8_t f_invalid		= 0x10; //!< Invalid driver (need remove)
	
		~Driver();
	
	protected:
		Driver(const QDomElement &e);
		
		uint8_t flags;	//!< Flags for capabilities
		QString m_manufacturer;		//!< Device's Manufacturer
		QString m_manufacturerCode;	//!< Device's Manufacturer's code
		QString m_code;			//!< Device's code
		QString m_name;			//!< Device's name
		
		SNMPversion m_snmpVersion;	//!< SNMP version supported by the device
		SNMPmib m_snmpMIB;		//!< SNMP MIB supported by the device
		DeviceClass m_deviceClass;	//!< Device's class
	public:
		inline const bool supportTelnet() const
		{ return flags & f_supportTelnet; }
		
		inline const bool supportSerial() const
		{ return flags & f_supportSerial; }
		
		inline const bool supportSNMP() const
		{ return flags & f_supportSNMP; }
		
		inline const bool serialRJ12() const
		{ return flags & f_serialRJ12; }
		
		inline const bool invalid() const
		{ return flags & f_invalid; }
		
		//! Gets the SNMP version supported by the device
		inline const SNMPversion snmpVersion() const
		{ return m_snmpVersion; }
		
		//! Gets the SNMP MIB supported by the device
		inline const SNMPmib snmpMIB() const
		{ return m_snmpMIB; }
		
		//! Gets the device's class
		inline const DeviceClass deviceClass() const
		{ return m_deviceClass; }
		
		//! Gets the device's manufacturer
		inline const QString manufacturer() const
		{ return m_manufacturer; }
		
		//! Gets the device's manufacturer code
		inline const QString manufacturerCode() const
		{ return m_manufacturerCode; }
		
		//! Gets the device's code
		inline const QString code() const
		{ return m_code; }
		
		//! Gets the device's name
		inline const QString name() const
		{ return m_name; }
	};
};

#endif
