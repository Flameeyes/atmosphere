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
#ifndef KDEDATMOS_H
#define KDEDATMOS_H

#include <kdedmodule.h>

class KSystemTray;
class QTimer;

/**
@author Flameeyes
@brief KDED plugin of ATMOSphere

This class is the main class for ATMOSphere KDED module.
It's used by ATMOS main code, and kioslave, to get information about the
devices themselves, and to have information about them.

The module creates an icon in the systray where notifications about fatal
errors.
*/
class KDEDAtmos : public KDEDModule
{
Q_OBJECT
K_DCOP
k_dcop:
	void reloadSettings();
protected:
	/**
	@brief ATMOSphere KDED icon
	*/
	KSystemTray *m_sysTray;
	
	/**
	@brief Name used to load the module
	
	It's saved to allow the unload of the module via context menu.
	*/
	QCString m_moduleName;
	
	/**
	@brief Timer which checks the registered devices.
	*/
	QTimer *m_checkTimer;
	
	/**
	@brief Opens SNMP sessions for devices which supports SNMP.
	*/
	void openSNMP();
	
protected slots:
	void unload();
	void checkDevices();
public:
	KDEDAtmos(const QCString& name);
	~KDEDAtmos();
};

#endif
