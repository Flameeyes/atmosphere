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
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <libksnmp/session.h>

#include "common/exception.h"

static const char description[] =
    I18N_NOOP("ATMOSphere, router management software");

static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open." ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
	// Initialize net-snmp library
	KSNMP::init("atmosphere");
		
	KAboutData about("atmosphere", I18N_NOOP("ATMOSphere"), version, description,
			KAboutData::License_GPL, "(C) 2004 Flameeyes", 0, 0, "dgp85@users.sourceforge.net");
	about.addAuthor( "Flameeyes", 0, "dgp85@users.sourceforge.net" );
	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions( options );
	KApplication app;
	ATMOSphere *mainWin = 0;
	
	try {
		if (app.isRestored())
		{
			RESTORE(ATMOSphere);
		}
		else
		{
			// no session.. just start up normally
			KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
		
			/// @todo do something with the command line args here
		
			mainWin = new ATMOSphere();
			app.setMainWidget( mainWin );
			mainWin->show();
		
			args->clear();
		}
	} catch( ATMOS::Exception &e ) {
		KMessageBox::error(0, e.message(), "Unhandled exception!", KMessageBox::Dangerous);
	}
	
	// mainWin has WDestructiveClose flag by default, so it will delete itself.
	int ret = app.exec();
	
	// Deinitialize net-snmp library
	KSNMP::shutdown();
	
	return ret;
}

