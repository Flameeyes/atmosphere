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

#ifndef _ATMOSPHERE_H_
#define _ATMOSPHERE_H_

#include <kmainwindow.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

class KCMultiDialog;
class KIconView;
class KIconViewItem;
class DeviceWidget;
class QIconViewItem;
class QIODevice;
template<class T, class Q> class QMap;
namespace ATMOS { class Device; };
#ifdef HAVE_TFTP
namespace TFTP { class Server; }
#endif

/*!
\brief ATMOSphere Main class
*/
class ATMOSphere : public KMainWindow
{
Q_OBJECT
public:
	ATMOSphere();
	~ATMOSphere();
	void loadDevices();
public slots:
	void slotConfigureKeys();
	void slotConfigureNotifications();
	void slotConfigureToolBars();
	void setup();
	void deviceOpened(QIconViewItem* dev);
protected:
	KCMultiDialog *kcmDialog;	//!< KCM instance
	KIconView *iv;			//!< View of the devices as icons
#ifdef HAVE_TFTP
	TFTP::Server *tftpServer;	//!< Trivial FTP server
#endif
	
	typedef QMap<KIconViewItem*,ATMOS::Device*> DeviceMap;
		//!< Typedef of ATMOSphere::devicemap object
	DeviceMap devicemap;		//!< Map of icon-device pairs

protected slots:
	void tftpFileSent(QIODevice *dev, QString filename);
	void tftpFileReceived(QIODevice *dev, const QString filename);
};

#endif // _ATMOSPHERE_H_
