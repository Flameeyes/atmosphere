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

#include "atmosphere.h"
#include "devicewidget.h"
#include <atmosdevice.h>
#include <emissingsupport.h>
#include <settings.h>

#ifdef HAVE_TFTP
#include <tftp/server.h>
#endif

#include <qlabel.h>
#include <qhostaddress.h>
#include <qptrlist.h>

#include <kmainwindow.h>
#include <klocale.h>
#include <kaction.h>
#include <kkeydialog.h>
#include <knotifydialog.h>
#include <kiconloader.h>
#include <kcmultidialog.h>
#include <kedittoolbar.h>
#include <kiconview.h>
#include <kmessagebox.h>
#include <knotifyclient.h>

/*!
\brief Default constructor

This is the default constructor for the main class of the application.
It adds the standard actions and create the ATMOSphere's KCM.
*/
ATMOSphere::ATMOSphere()
    : KMainWindow( 0, "ATMOSphere" )
{
	// set the shell's ui resource file
	setXMLFile("atmosphereui.rc");
	
	KStdAction::keyBindings(this, SLOT( slotConfigureKeys() ), actionCollection() );
	KStdAction::configureNotifications(this, SLOT(slotConfigureNotifications()), actionCollection());
	KStdAction::configureToolbars(this, SLOT(slotConfigureToolBars()), actionCollection());
	KStdAction::quit(this, SLOT(close()), actionCollection());
	KStdAction::preferences(this, SLOT(setup()), actionCollection());

	createGUI();
	
	kcmDialog = new KCMultiDialog( KCMultiDialog::Plain, "ATMOSphere settings", this, "kcmDialog", true);
	kcmDialog->addModule("kcmatmosphere");
	
	iv = new KIconView(this);
	setCentralWidget(iv);
	loadDevices();
	
	connect(iv, SIGNAL( executed(QIconViewItem*) ), this, SLOT(deviceOpened(QIconViewItem*)));
	
#ifdef HAVE_TFTP
	tftpServer = new TFTP::Server( QHostAddress() );
	connect(tftpServer, SIGNAL( receivedFile(QIODevice*,const QString) ), this, SLOT(tftpFileReceived(QIODevice *, const QString)));
	connect(tftpServer, SIGNAL( sentFile(QIODevice*, QString) ), this, SLOT(tftpFileSent(QIODevice *, QString)));
#endif
}

/**
@brief Destructor of the main class

This destructor takes care of destroying the icon view, the kcm and the tftp server if applicable.
*/
ATMOSphere::~ATMOSphere()
{
	delete iv;
	delete kcmDialog;
#ifdef HAVE_TFTP
	delete tftpServer;
#endif
}

//! Called when asked to configure keys - Standard behaviour
void ATMOSphere::slotConfigureKeys()
{
	KKeyDialog::configure( actionCollection(), this );
}

//! Called when asked to configure notifications - Standard behaviour
void ATMOSphere::slotConfigureNotifications()
{
	KNotifyDialog::configure(this);
}

//! Called when asked to configure toolbars - Standard behaviour
void ATMOSphere::slotConfigureToolBars()
{
	KEditToolbar dlg(actionCollection());
	if (dlg.exec())
		createGUI();
}

//! Called when asked to modify preferences, calls the KCM instance
void ATMOSphere::setup()
{
	kcmDialog->exec();
}

//! Loads the devices configured and create icons to be selected
void ATMOSphere::loadDevices()
{
	const QPtrList<ATMOS::Device> &devices = ATMOS::Device::getDevices();
	const QPtrList<ATMOS::Device>::const_iterator devend = devices.end();
	for ( QPtrList<ATMOS::Device>::const_iterator it = devices.begin(); it != devend; ++it)
	{
		// The icon depends on the class, so we should load different icons for different devices
		KIconViewItem *ivi = new KIconViewItem(
			iv,
			(*it)->IP(),
			KGlobal::iconLoader()->loadIcon( (*it)->getIcon(), KIcon::Desktop )
			);
		
		devicemap[ivi] = *it;
	}
}

/*!
\brief Opens the device dialog for a device when an icon is executed
\param dev the icon executed (need to check the devicemap to find the ATMOS::Device pointer).
*/
void ATMOSphere::deviceOpened(QIconViewItem* dev)
{
	try {
		DeviceWidget *dw = new DeviceWidget( devicemap[ reinterpret_cast<KIconViewItem*>(dev) ], this );
		dw->exec();
		delete dw;
	} catch ( ATMOS::EMissingSupport &e ) {
		KNotifyClient::event(
			winId(),
			"invaliddevice",
			tr("Tried to access a missing support. Device will be removed from the pool!")
			);
		
		//! \todo remove the device from the pool
	}
}

/*!
\brief Slot called when a file is sent by TFTP server
\param dev File or buffer where the data was read from
\param filename Name of the file on the router side
*/
void ATMOSphere::tftpFileSent(QIODevice *dev, QString filename)
{
#ifdef HAVE_TFTP
/*	if ( dev->className() == "QFile" )
	{
		dev->close();
		delete dev;
	}*/
#endif
}

/*!
\brief Slot called when a file is received by TFTP server
\param dev File or buffer where the data was wrote to
\param filename Name of the file on the router side
*/
void ATMOSphere::tftpFileReceived(QIODevice *dev, const QString filename)
{
#ifdef HAVE_TFTP
/*	if ( dev->className() == "QFile" )
	{
		dev->close();
		delete dev;
	}*/
#endif
}

#include "atmosphere.moc"
