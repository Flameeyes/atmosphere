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
#ifndef ATMOSEXCEPTION_H
#define ATMOSEXCEPTION_H

#include <stdint.h>

#include <qobject.h>

namespace ATMOS {

	/*!
	\brief Base class for exceptions
	\author Flameeyes
	
	This is the base class for ATMOS namespace. This can't be
	instantiated because is a pure-virtual class.
	*/
	class Exception : public QObject{
	Q_OBJECT
	public:
		//! List of exception codes
		enum ExceptionCode {
			ecUnknown,		//!< Unknown exception
			ecMissingSupport,	//!< Missing support for
						//!< something requested
			ecInvalidDriver		//!< Tried to load an
						//!< invalid driver
		};
		
		static const QString message(ExceptionCode ec);
	
		//! Returns the message of the exception (uses the
		//! static function message(ExceptionCode)
		const QString message() const
		{ return message(m_exceptionCode); }
		
		//! Returns the exception code
		const ExceptionCode exceptionCode() const
		{ return m_exceptionCode; }
		
		//! \brief Copy constructor
		Exception(const Exception&e) : QObject()
		{ m_exceptionCode = e.exceptionCode(); }
		
		virtual ~Exception() { }
	protected:
		Exception(ExceptionCode ec)
		{ m_exceptionCode = ec; }
		
		ExceptionCode m_exceptionCode;
	};

};

#endif
