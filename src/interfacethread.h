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
#ifndef INTERFACETHREAD_H
#define INTERFACETHREAD_H

#include <qthread.h>
#include <stdint.h>
#include <libksnmp/mib1213.h>

class InterfaceStats;
namespace ATMOS { class Device; };

/*!
\brief Thread to get Interface statistics
\author Flameeyes

This class implements a thread which gets interface's statistics and put them
into their right dialog.
It also draws the graph which is shown in the dialog.

The update interval is user-configurable.
*/
class InterfaceThread : public QThread
{
public:
	InterfaceThread(InterfaceStats *I, ATMOS::Device *D, uint32_t ifi);
	~InterfaceThread();
	
protected:
	inline uint32_t heightFromOctets(uint32_t oct, KSNMP::Device1213::Interface &IF);
	InterfaceStats *is;	//!< Inteface's statistics dialog
	ATMOS::Device *dev;	//!< Device to took the data from
	uint32_t ifNumber;	//!< Interface's Index
	
	uint32_t prevTx;	//!< Previous cycle transmitted bytes
	uint32_t prevRx;	//!< Previous cycle received bytes

	uint8_t graphTx[60];	//!< Graph-heights for transmitted bytes
	uint8_t graphRx[60];	//!< Graph-heights for received bytes
	uint8_t currIdx;	//!< Current index for graphTx and graphRx
	
	void run();
};

#endif
