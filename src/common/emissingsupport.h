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
#ifndef ATMOSEMISSINGSUPPORT_H
#define ATMOSEMISSINGSUPPORT_H

#include "exception.h"

namespace ATMOS {

	/*!
	\brief Missing support exception
	
	This exception is thrown when we request from the device
	something it can't provide (like SNMP in a non-snmp capable
	device, or serial in a device not connected to a serial cable).
	*/
	class EMissingSupport : public Exception
	{
	public:
		enum MissingSupport {
			msUnknown,	//!< Unknown missing support
					//!< (shouldn't be used!)
			msSNMP,		//!< Device misses SNMP support
			msTelnet,	//!< Device misses Telnet support
			msSerial	//!< Device misses Serial support
		};
	
		/*!
		\brief Constructor
		\param wm The missing support
		\see EMissingSupport::MissingSupport
		*/
		EMissingSupport(MissingSupport wm) : Exception( Exception::ecMissingSupport )
		{ m_whatsMissing = wm; qWarning("missingsupport thrown"); }

		//! \brief Copy constructor
		EMissingSupport(const EMissingSupport &e) : Exception(e)
		{ m_whatsMissing = e.whatsMissing(); }
		
		~EMissingSupport() { }
		
		//! Returns which support is missing
		const MissingSupport whatsMissing() const
		{ return m_whatsMissing; }
	protected:
		MissingSupport m_whatsMissing;	//!< Which support is missing?
	};

};

#endif
