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
#include "interfacethread.h"
#include "interfacestats.h"

#include "common/settings.h"
#include "common/atmosdevice.h"
#include "common/utils.h"

#include <libksnmp/mib1213.h>

#include <klistview.h>
#include <klineedit.h>
#include <kled.h>
#include <klocale.h>

#include <qlabel.h>

/*!
\brief Translate the octets value in a graph value
\param oct Octets value to translate
\param IF references for speed value
\return the height of the graph point for the octets
\note This function do math with IF's speed to reach the right result
*/
inline uint32_t InterfaceThread::heightFromOctets(uint32_t oct, KSNMP::Device1213::Interface &IF)
{
	uint32_t bits = oct * 8;
//	uint32_t H = is->netloadGraph->height() -4;
	static const uint32_t H = 96;
	
	if ( bits > IF.ifSpeed )
		bits = IF.ifSpeed;
	
	uint32_t newh = 100 - ((H * bits) / IF.ifSpeed);
	if ( newh > H )
		return 96 ;
	
	return newh;
}

/*!
\brief Default constructor
\param I interface statistics' dialog
\param D device to read the statistics from
\param ifi Interface's index
*/
InterfaceThread::InterfaceThread(InterfaceStats *I, ATMOS::Device *D, uint32_t ifi)
 : is(I), dev(D), ifNumber(ifi)
{
	prevTx = 0;
	prevRx = 0;
	
	memset(graphTx, 100, sizeof(graphTx));
	memset(graphRx, 100, sizeof(graphRx));
	currIdx = 0;
}

InterfaceThread::~InterfaceThread()
{
}

//! Actual thread method
void InterfaceThread::run()
{
	while(1)
	{
		// If not 1213 compliant simply doesn't display anything
		KSNMP::Device1213 *snmpDev = reinterpret_cast<KSNMP::Device1213 *>(dev->snmp());
		if ( ! snmpDev ) return;
		
		KSNMP::Device1213::Interface IF = snmpDev->getInterface(ifNumber);
		
		is->lineTransmitted->setText( humanizeOctets(IF.ifInOctets) );
		is->lineReceived->setText( humanizeOctets(IF.ifOutOctets) );
		is->lineUnknownProto->setText( QString::number(IF.ifInUnknownProtos) );
		is->lineQueueLen->setText( QString::number(IF.ifOutQLen) );
		is->lineLastChange->setText( humanizeTimeTicks(IF.ifLastChange) );
		
		is->viewStats->clear();
		new KListViewItem(
			is->viewStats,
			is->viewStats->lastItem(),
			i18n("Unicast"),
			QString::number(IF.ifInUcastPkts),
			QString::number(IF.ifOutUcastPkts)
			);
		new KListViewItem(
			is->viewStats,
			is->viewStats->lastItem(),
			i18n("Non-Unicast"),
			QString::number(IF.ifInNUcastPkts),
			QString::number(IF.ifOutNUcastPkts)
			);
		new KListViewItem(
			is->viewStats,
			is->viewStats->lastItem(),
			i18n("Discarded"),
			QString::number(IF.ifInDiscards),
			QString::number(IF.ifOutDiscards)
			);
		new KListViewItem(
			is->viewStats,
			is->viewStats->lastItem(),
			i18n("Errors"),
			QString::number(IF.ifInErrors),
			QString::number(IF.ifOutErrors)
			);
		
		if ( prevTx > IF.ifOutOctets )
			prevTx = 0;
		if ( prevRx > IF.ifInOctets )
			prevRx = 0;
		
		graphTx[currIdx] = heightFromOctets(IF.ifOutOctets - prevTx, IF);
		graphRx[currIdx] = heightFromOctets(IF.ifInOctets - prevRx, IF);
		
		uint32_t firstIdx = wrapAround(currIdx+1, 60);
		
		is->p.begin(&(is->graph_p));
		is->p.setPen(QPen(Qt::white));
		is->p.setBrush(Qt::white);
		is->p.drawRect(0, 0, 600, 100);
		
		for(int i = 0; i < 60; i++)
		{
			uint32_t nextIdx = wrapAround(firstIdx+1, 60);
			is->p.setPen(QPen(Qt::red, 2));
			is->p.drawLine(i*10, graphTx[firstIdx], (i+1)*10, graphTx[nextIdx]);
			is->p.setPen(QPen(Qt::green, 2));
			is->p.drawLine(i*10, graphRx[firstIdx], (i+1)*10, graphRx[nextIdx]);
			firstIdx = nextIdx;
		}

		is->p.end();
		is->netloadGraph->setPicture(is->graph_p);
		
		prevTx = IF.ifOutOctets;
		prevRx = IF.ifInOctets;
		currIdx = wrapAround(currIdx+1, 60);

		msleep(ATMOSsettings::reReadInterface());
	}
}
