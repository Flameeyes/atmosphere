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
#include "cliwidget.h"

#include <ktextedit.h>
#include <kcombobox.h>
#include <kglobalsettings.h>

/*!
\brief Default construtor
\param parent Parent widget, passed to the inherited class
\param name Name of the widget, passed to the inherited class
*/
CLIWidget::CLIWidget(QWidget *parent, const char *name)
 : uicliwidget(parent, name)
{
	output->setFont( KGlobalSettings::fixedFont() );
}

CLIWidget::~CLIWidget()
{
}

/*!
\brief Slot called when data is received from a CLInterfaface childclass
\param lst List of new lines sent
*/
void CLIWidget::dataReceived(const QStringList &lst)
{
	output->removeParagraph( output->paragraphs() -1);
	for( QStringList::const_iterator it = lst.begin(); it != lst.end(); it++)
	{
		QString line = *it;
		line.remove('\r');
		output->append(line);
	}
}

/*!
\brief Slot called when btnSend is clicked or when return is pressed in
	the history combo
*/
void CLIWidget::returnPressed()
{
	QString cmd = comboInput->currentText();
	comboInput->addToHistory(cmd);
	comboInput->setCurrentText(QString::null);
	emit commandSent(cmd);
}

#include "cliwidget.moc"
