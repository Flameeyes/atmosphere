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
#ifndef ATMOSEINVALIDDRIVER_H
#define ATMOSEINVALIDDRIVER_H

#include <exception.h>

namespace ATMOS {

	/*!
	\brief Invalid driver exception
	
	This exception is thrown when ATMOSphere tries to load a driver
	which has some parameters messed up, or isn't a driver, or something
	like that.
	This replace the old behaviour to mark the driver invalid and check
	this before add the driver to the list.
	*/
	class EInvalidDriver : public Exception
	{
	public:
		EInvalidDriver() : Exception(ecInvalidDriver)
		{ }
		
		~EInvalidDriver()
		{ }
	};

};

#endif
