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
#ifndef ATMOSDEVICE_H
#define ATMOSDEVICE_H

#include "atmosdriver.h"

#include <libksnmp/device.h>
#include <libksnmp/session.h>

#include <qobject.h>
#include <qstring.h>
#include <stdint.h>

class QTextStream;
class QBuffer;
template<class T> class QPtrList;

namespace ATMOS {

	class CLInterface;
	class TelnetInterface;
	class SerialInterface;
	
	/**
	@author Flameeyes
	@brief Class representing an ATMOSphere supported device
	
	This is the base class for ATMOSphere supported devices, such as ATMOS-driven
	routers or modems, and access points.
	
	ATMOSphere started working only with ATMOS operating system, so the code sometimes
	referes at them as ATMOS-driven devices, but they can actually be driven by any other
	operating system.
	For instance, I use ATMOSphere also to monitor my NetBSD-driven Wireless Access Point,
	but in this case I haven't a CLI access.
	*/
	class Device : public QObject
	{
	Q_OBJECT
	public:
		static void loadDevices();
		static void saveDevices();
		static const QPtrList<Device> &getDevices();
		static bool addDevice(Device *dev);
		static void removeDevice(Device *dev);
		
		Device();
		~Device();
		bool validDevice();
		
		/**
		@brief Gets the icon name for the device.
		@return The name of an icon representing the class of the device.
		
		This pure virtual method must be reimplemented by child classes to allow
		ATMOSphere base program, eventual ioslave and other things to not require
		checks to find the right name of the icon to give to the device.
		
		Obviously, the load of the icon is still of the calling function, which
		can select the right dimension needed for the icon.
		*/
		virtual QString getIcon() const = 0;
		
	protected:
		void checkCapabilities();
	
		QString m_IP;	//!< Device's IP
		QString m_type;	//!< Device's type-string
		uint8_t m_flags;//!< Device's flags
		
		static const uint8_t f_hasTelnet	= 0x01;	//!< Flag set if device has telnet support
		static const uint8_t f_hasSerial	= 0x02;	//!< Flag set if device has serial support
		static const uint8_t f_hasSNMP		= 0x04;	//!< Flag set if device has SNMP support
		static const uint8_t f_hasTelnetPwd	= 0x08;	//!< Flag set if device has an ATMOS-style telnet
		static const uint8_t f_SNMPWrite	= 0x10;	//!< Flag set if device configured with SNMP write access
		
		inline void setFlag(uint8_t flag, bool on)
		{
			if ( on )
				m_flags |= flag;
			else
				m_flags &= ~flag;
		}
	
	protected slots:
		void deletedInterface(QObject *obj);
		
	public:
		//! Gets the current device's IP
		inline const QString IP() const
		{ return m_IP; }
		
		//! Sets the current device's IP
		inline void setIP(const QString &str)
		{ m_IP = str; }
		
		//! Gets the current device's type
		inline const QString type() const
		{ return m_type; }
		
		//! Gets the current device's driver
		const Driver *driver() const
		{ return Driver::getDriver( m_type ); }
		
		//! Sets the current device's type
		inline void setType(const QString &str)
		{ m_type = str; checkCapabilities(); }
		
		//! Gets if the device has telnet support
		inline const bool hasTelnet() const
		{ return m_flags & f_hasTelnet; }
		
		//! Sets the telnet support
		inline void setTelnet(bool on)
		{ setFlag(f_hasTelnet, on); checkCapabilities(); }
		
		//! Gets if the device has telnet password support
		inline const bool hasTelnetPwd() const
		{ return m_flags & f_hasTelnetPwd; }
		
		//! Sets the telnet password support
		inline void setTelnetHasPwd(bool on)
		{ setFlag(f_hasTelnetPwd, on); checkCapabilities(); }
		
		//! Gets if the device has serial support
		inline const bool hasSerial() const
		{ return m_flags & f_hasSerial; }
		
		//! Sets the serial support
		inline void setSerial(bool on)
		{ setFlag(f_hasSerial, on); checkCapabilities(); }
		
		//! Gets if the device has SNMP support
		inline const bool hasSNMP() const
		{ return m_flags & f_hasSNMP; }
		
		//! Sets the SNMP support
		inline void setSNMP(bool on)
		{ setFlag(f_hasSNMP, on); checkCapabilities(); }
		
		//! Gets if the device has SNMP write access
		inline const bool SNMPWrite() const
		{ return m_flags & f_SNMPWrite; }
		
		//! Sets the SNMP write access
		inline void setSNMPWrite(bool on)
		{ setFlag(f_SNMPWrite, on); checkCapabilities(); }
		
		CLInterface *getCLI();
		
	//@{
	/**
	@name telnet Telnet specific options
	*/
	protected:
		QString m_telnetPassword;	//!< Device's telnet password
		uint16_t m_telnetPort;		//!< Device's telnet port
		TelnetInterface *m_telnetInterface;	//!< Device's Telnet Interface (openend)
	public:
		//! Gets the current telnet password
		inline const QString telnetPassword() const
		{ return m_telnetPassword; }
		
		//! Sets the current telnet password
		inline void setTelnetPassword(const QString &str)
		{ m_telnetPassword = str; checkCapabilities(); }
		
		//! Gets the current telnet port
		inline const uint16_t telnetPort() const
		{ return m_telnetPort; }
		
		//! Sets the current telnet port
		inline void setTelnetPort(uint16_t port)
		{ m_telnetPort = port; checkCapabilities(); }
		
		void telnetOpenInterface();
		
		//! Gets the opened Telnet interface
		inline TelnetInterface *telnetInterface() const
		{ return m_telnetInterface; }
	//@}
	
	//@{
	/**
	@name serial Serial specific options
	*/
	protected:
		QString m_serialDevice;			//!< Device's serial device
		SerialInterface *m_serialInterface;	//!< Device's Serial Interface (opened)
	public:
		//! Gets the current serial device
		inline const QString serialDevice() const
		{ return m_serialDevice; }
		
		//! Sets the current serial device
		inline void setSerialDevice(const QString &str)
		{ m_serialDevice = str; checkCapabilities(); }
		
		void serialOpenInterface();
		
		//! Gets the opened Serial Interface
		inline SerialInterface *serialInterface() const
		{ return m_serialInterface; }
	//@}
	
	//@{
	/**
	@name snmp SNMP specific options
	
	Here we have specific options for SNMP session. They are passed to
	classes from libksnmp to open the sessions and maintain them.
	
	@note Session and Device objects must be protected by concurrent access by a
		QMutex until libksnmp isn't thread-safe.
	@note The SNMP community is stored into the KDE Wallet for security reasons.
	*/
	protected:
		QString m_snmpCommunity;	//!< Device's SNMP write community
		uint16_t m_snmpPort;		//!< Device's SNMP port
		KSNMP::Session *m_snmpSession;	//!< Device's SNMP session
		KSNMP::Device *m_snmp;		//!< Device's SNMP session abstraction
	public:
		//! Gets the current SNMP community for read
		inline const QString snmpCommunity() const
		{ return m_snmpCommunity; }
		
		//! Sets the current SNMP communiy for read
		inline void setSnmpCommunity(const QString &str)
		{ m_snmpCommunity = str; checkCapabilities(); }
		
		//! Gets the current SNMP port
		inline const uint16_t snmpPort() const
		{ return m_snmpPort; }
		
		//! Sets the current SNMP port
		inline void setSnmpPort(uint16_t port)
		{ m_snmpPort = port; checkCapabilities(); }
		
		void snmpOpenSession();
		void snmpCloseSession();
		
		//! Gets the SNMP session abstraction (if any)
		inline KSNMP::Device *snmp() const
		{ return m_snmp; }
		
		//! Gets the SNMP session (low-level)
		inline KSNMP::Session *snmpSession() const
		{ return m_snmpSession; }
	//@}
	
	//@{
	/**
	@name eventlog Event Log stuff
	
	@note This is mainly an ATMOS-specific feature, but maybe it's also
		implemented in other devices, so this remain a doubt for now.
	*/
	protected:
		QBuffer *m_logBuffer;
		QTextStream *m_logStream;
	public:
		void fetchLog();
	//@}
	
	};
};

#endif
