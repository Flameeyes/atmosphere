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

#ifndef _KCMATMOSPHERE_H_
#define _KCMATMOSPHERE_H_

#include <kcmodule.h>
#include <dcopobject.h>

class KcmMain;
class QLayout;
class KAboutData;

class kcmatmosphere: public KCModule, virtual public DCOPObject
{
Q_OBJECT
K_DCOP
k_dcop:
	void reloadConfig();

public:
	kcmatmosphere( QWidget *parent=0, const char *name=0, const QStringList& = QStringList() );
	~kcmatmosphere();
	
	virtual void load();
	virtual void save();
	virtual void defaults();
	virtual int buttons();
	virtual QString quickHelp() const;
	virtual const KAboutData *aboutData()const
	{ return myAboutData; };

public slots:
	void configChanged();

private:
	KcmMain *mw;
	QLayout *lay;
	KAboutData *myAboutData;
};

#endif
