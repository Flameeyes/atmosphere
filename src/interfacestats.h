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

#ifndef INTERFACESTATS_H
#define INTERFACESTATS_H

#include "uiinterfacestats.h"

#include <stdint.h>

#include <qpainter.h>
#include <qpicture.h>

namespace ATMOS {
	class Device;
};
class InterfaceThread;

/*!
\brief Interface's Statistics Dialog

This dialog shows interface statistics and status information to the user.
*/
class InterfaceStats : public uiinterfacestats
{
	friend class InterfaceThread;
Q_OBJECT

public:
	InterfaceStats(ATMOS::Device *dev, uint32_t interface, QWidget* parent = 0, const char* name = 0);
	~InterfaceStats();

protected:
	uint32_t ifIndex;	//!< Index of the interface to get the statistics of
	InterfaceThread *it;	//!< InterfaceThread instance
	QPicture graph_p;	//!< Picture which represent the graph
	QPainter p;		//!< Painter on the graph
};

#endif

