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
#ifndef EVENTSLOGWIDGET_H
#define EVENTSLOGWIDGET_H

#include "uieventslog.h"

/*!
\brief Class which manage the events log

This widget is derived from uiEventsLog which is the events log widget
itself.
This class add functions to send messages with a determined warning level
*/
class EventsLogWidget : public uiEventsLog
{
Q_OBJECT
public:
	//! Message warning level
	enum WarningLevel {
		wlInfo,		//!< The message is informative
		wlLowWarning,	//!< The message is a low warning
		wlWarning,	//!< The message is a (medium) warning
		wlAlert,	//!< The message is an alert
		wlSecurity	//!< The message is a security alert
	};
	
	//! Source interface
	enum SourceInterface {
		siExtra,	//!< Extern message
		siSerial,	//!< Serial interface message
		siTelnet	//!< Telnet interface message
	};
	
	EventsLogWidget(QWidget *parent = 0, const char *name = 0);
	~EventsLogWidget();

	void parseMessage(const QString&message, SourceInterface si = siExtra);
	void logMessage(const QString&message, WarningLevel warn = wlInfo);

};

#endif
