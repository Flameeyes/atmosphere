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
#ifndef KCMMAIN_H
#define KCMMAIN_H

#include "uikcmmain.h"

#include <qmap.h>

class kcmatmosphere;
class KListViewItem;

namespace ATMOS {
	class Device;
};

/*!
\author Flameeyes
\brief Main widget for KControl module
*/
class KcmMain : public uikcmmain
{
Q_OBJECT
public:
	KcmMain(QWidget *parent = 0, const char *name = 0);
	~KcmMain();
	void loadDevices();
public slots:
	void addDevice();
	void removeDevice();
	void addXML();
	void removeXML();
	void urlXMLChanged(const QString &url);
	void snmpReauditChanged();
	void ifStatsReauditChanged();
	void toggledShowRawEvents();
	void toggledTelnetKeepAlive();
protected:
	kcmatmosphere *kcm;	//!< KCM Parent module
	
	typedef QMap<KListViewItem*,ATMOS::Device*> DeviceMap;
	DeviceMap devicemap;
};

#endif
