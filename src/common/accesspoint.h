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
#ifndef ATMOSACCESSPOINT_H
#define ATMOSACCESSPOINT_H

#include <atmosdevice.h>

namespace ATMOS {

	/**
	@brief Class representing the access to an Access Point device
	
	Actually, the Access Point I'm testing with it's not ATMOS-based,
	but it supports only SNMPv2 so I think it should be supported anyway.
	*/
	class AccessPoint : public Device
	{
	Q_OBJECT
	public:
		AccessPoint();
		~AccessPoint();
		
		QString getIcon() const;
	};

};

#endif
