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
#include "adddevice.h"
#include <atmosdriver.h>
#include <atmosdevice.h>

#include <klineedit.h>
#include <kmessagebox.h>
#include <kcombobox.h>
#include <klocale.h>

#include <qgroupbox.h>

AddDevice::AddDevice(QWidget *parent, const char *name)
 : uiadddevice(parent, name)
{
	for ( ATMOS::Driver::Drivers::iterator it = ATMOS::Driver::firstDriver(); it != ATMOS::Driver::lastDriver(); it++ )
	{
		if ( ! *it ) continue;
		
		comboDeviceType->insertItem( (*it)->manufacturer() + " " + (*it)->name() );
		devices.push_back( (*it)->code() );
	}
}

AddDevice::~AddDevice()
{
}

/*!
\brief Accepted Add Device

This function is called whenever the Add Device dialog is accepted, and checks
if the dialog is complete or there're errors.
*/
void AddDevice::tryAccept()
{
	if ( lineDeviceIP->text().isEmpty() )
		if ( KMessageBox::warningYesNo(
			this,
			i18n("You haven't setted up an IP for the device, do you want to continue editing this device?")
			) == KMessageBox::Yes
		) {
			return;
		} else {
			reject(); return;
		}
	
	if ( boxSerial->isChecked() )
	{
		const ATMOS::Driver *drv = ATMOS::Driver::getDriver(typeSelected());
		if ( ! drv )
		{
			qWarning("This shouldn't happen... selected a driver which doesn't exists...");
			return;
		}
		if ( drv->serialRJ12() )
				if ( KMessageBox::questionYesNo(
					this,
					i18n("Using the serial connection with this device needs a special RJ12 cable.\n"
					"This shouldn't be used outside the device's manufacturer.\n"
					"Are you sure you want to enable the serial connection?"),
					QString::null,
					KStdGuiItem::yes(),
					KStdGuiItem::no(),
					"rj12warning"
					) == KMessageBox::No )
				{
					return;
				}
	}
	
	const QPtrList<ATMOS::Device> &devices = ATMOS::Device::getDevices();
	const QPtrList<ATMOS::Device>::const_iterator devend = devices.end();
	for( QPtrList<ATMOS::Device>::const_iterator it = devices.begin(); it != devend; ++it )
	{
		if ( lineDeviceIP->text() == (*it)->IP() )
			if ( KMessageBox::questionYesNo(
				this,
				i18n("There's already a device using this IP. If you add another, you probably make ATMOSphere go crazy\n"
				"Are you sure you want to add this device?"),
				QString::null,
				KStdGuiItem::yes(),
				KStdGuiItem::no()
				) == KMessageBox::No )
			{
				return;
			}
	}

	accept();

}

/*!
\brief Return the selected device type
*/
const QString AddDevice::typeSelected() const
{
	return devices[ comboDeviceType->currentItem() ];
}

#include "adddevice.moc"
