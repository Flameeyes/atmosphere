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

#include "utils.h"

#include <qregexp.h>
#include <qcolor.h>

#include <kdebug.h>
#include <kwallet.h>

/**
@brief ATMOSphere wallet

This wallet is used to access the password from within ATMOSpher. This
pointer will be initialized at the first call of getWallet(), and then used
for the next calls.
*/
static KWallet::Wallet *wallet = NULL;

/**
@brief Gets the ATMOSphere wallet
@return A pointer to the ATMOSphere wallet

This function simply checks wallet variable to check if it's not null.
In this case, it returns it, else it opens the wallet, set the variable to
the newly opened wallet, and then return it.
*/
KWallet::Wallet *getWallet()
{
	if ( wallet ) return wallet;
	
	wallet = KWallet::Wallet::openWallet( KWallet::Wallet::NetworkWallet() );
	if ( ! wallet )
	{
		kdFatal() << "Unable to open Network Password Wallet " << KWallet::Wallet::NetworkWallet() << endl;
		return wallet;
	}
	
	if ( ! wallet->hasFolder("atmosphere") && ! wallet->createFolder("atmosphere") )
	{
		kdFatal() << "Unable to create 'atmosphere' folder into '" << KWallet::Wallet::NetworkWallet() << "' wallet." << endl;
	}
	
	wallet->setFolder("atmosphere");
	
	return wallet;
}

/**
@brief Check if a string is a valid IP form
@param str String to check
@param flags a combination of ipHost, ipNetwork, ipBroadcast masks
@retval true @c str is a valid IP in dotted-decimal form
@retval false @c str is not a valid IP in dotted-decimal
@sa validIPFlags
*/
bool validIP(const QString &str, uint8_t flags)
{
	QRegExp exp("(\\d+)\\.(\\d+)\\.(\\d+)\\.(\\d)");
	
	if ( exp.search(str) == -1 )
		return false;
	
	uint32_t ip = (exp.cap(1).toUInt() << 24) + (exp.cap(2).toUInt()
		<< 16) + (exp.cap(3).toUInt() << 8) + exp.cap(4).toUInt();
	
	if ( ip == 0 )
		return false;
		
	uint32_t mask;
	if ( ! ( ip & 0x80000000 ) )
		mask = 0xFF000000;
	else if ( ! ( ip & 0x40000000 ) )
		mask = 0xFFFF0000;
	else if ( ! ( ip & 0x20000000 ) )
		mask = 0xFFFFFF00;
	else if ( ip == 0xFFFFFFFF && flags & ipBroadcast )
		return true;
	else
		return false;
	
	uint32_t suffix = ip & ~mask;
	
	if ( suffix == 0 && flags & ipNetwork )
		return true;
	else if ( suffix | mask == 0xFFFFFFFF && flags & ipBroadcast )
		return true;
	else if ( flags & ipHost )
		return true;
	else
		return false;
}

/*!
\brief Humanize a speed value
\param speed Speed in bps (bit per second) to humanize
\return a string with the bigger multiple of bps that can be used
*/
QString humanizeSpeed(uint32_t speed)
{
	if ( speed % 1000 != 0 )
		return QString::number(speed) + " bps";
	
	speed /= 1000;
	if ( speed % 1000 != 0 )
		return QString::number(speed) + " Kbps";
	
	speed /= 1000;
	if ( speed % 1000 != 0 )
		return QString::number(speed) + " Mbps";
	
	speed /= 1000;
	return QString::number(speed) + " Gbps";
}

/*!
\brief Humanize an octets value
\param octets Number of octets to humanize
\return a string with the bigger multiple of B (bytes) that can be used
*/
QString humanizeOctets(uint32_t octets)
{
	if ( octets < 1024 )
		return QString::number(octets) + " B";
	
	float fOctets = octets/1024.0;
	QString ris;
	
	if ( fOctets < 1024 )
	{
		ris.sprintf("%.2f KB", fOctets);
		return ris;
	}
	
	fOctets /= 1024.0;
	
	if ( fOctets < 1024 )
	{
		ris.sprintf("%.2f MB", fOctets);
		return ris;
	}
	
	fOctets /= 1024.0;
	
	ris.sprintf("%.2f GB", fOctets);
	return ris;
}

/*!
\brief Humanize a TimeTicks value
\param value TimeTicks value to humanize
\return a string in the form "DD days, HH:mm:ss.hh"
*/
QString humanizeTimeTicks(uint32_t value)
{
	uint8_t hundredthsSeconds = value % 100;	value /= 100;
	uint8_t seconds = value % 60;			value /= 60;
	uint8_t minutes = value % 60;			value /= 60;
	uint8_t hours = value % 24;			value /= 24;
	
	QString str;
	str.sprintf("%d days, %d:%02d:%02d.%02d", value, hours, minutes, seconds, hundredthsSeconds);
	
	return str;
}

/**
@brief Trivial function to allow to set easily the color of the status leds
@param status Integer representing the status of the interface
@retval Qt::green Interface Up
@retval Qt::red Interface Down
@retval Qt::yellow Interface in test
@retval Qt::black Switch fall out (should never happen!)
*/
QColor statusColor(uint32_t status)
{
	switch(status)
	{
		case 1: // Up
			return Qt::green;
		case 2: // Down
			return Qt::red;
		case 3: // Test
			return Qt::yellow;
		default:
			return Qt::black;
	}
}
