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
#include "eventslogwidget.h"
#include "common/settings.h"

#include <qregexp.h>

#include <ktextedit.h>
#include <knotifyclient.h>

EventsLogWidget::EventsLogWidget(QWidget *parent, const char *name)
 : uiEventsLog(parent, name)
{
}

EventsLogWidget::~EventsLogWidget()
{
}

/*!
\brief Parses a message from a source
\param message Raw message to parse
\param si Source interface (not always used)
*/
void EventsLogWidget::parseMessage(const QString&message, SourceInterface si)
{
	if ( ATMOSsettings::logRawEvents() )
	{
		switch(si)
		{
		case siExtra:
			logMessage("RAW&gt; " + message);
			break;
		case siSerial:
			logMessage("RAW Serial &gt; " + message);
			break;
		case siTelnet:
			logMessage("RAW Telnet &gt; " + message);
			break;
		}
	}
	
	QString cleanMsg = message;
	QRegExp re;
	// Start TFTP message handling
	re.setPattern("\\(transfer timed out, aborted\\)\\r*\\n*");
	if ( re.search(message) != -1 ) {
		logMessage("TFTP&gt; " + tr( "Transfer timed out, aborted" ), wlLowWarning);
		return;
	}
	
	re.setPattern("putting '(.*)' to '(.*)'\\r*\\n*");
	if ( re.search(message) != -1 ) {
		logMessage(
			"TFTP&gt; " + tr("remote file %1 is being uploaded to %2")
				.arg(re.cap(1))
				.arg(re.cap(2))
			);
	}
	re.setPattern("getting '.*:(.*)' to '.*:(.*)'\\r*\\n*");
	if ( re.search(message) != -1 ) {
		logMessage(
			"TFTP&gt; " + tr("local file %1 is being downloaded to %2")
				.arg(re.cap(1))
				.arg(re.cap(2))
			);
		return;
	}
	re.setPattern("tftp error, code (\\d+): '(.*)'\\r*\\n*");
	if ( re.search(message) != -1 ) {
		logMessage(
			"TFTP&gt; " + tr("error packet received by the router: %1 [code %2]")
				.arg(re.cap(2))
				.arg(re.cap(1)),
			wlWarning
			);
		return;
	}
	re.setPattern("TFTP file put completed successfully\\r*\\n*");
	if ( re.search(message) != -1 ) {
		logMessage("TFTP&gt; " + tr("transfer completed"));
		return;
	}
	// End TFTP message handling
	
	// Start telnet message handling
	re.setPattern("logged on; type `@close' to close connection.\\r*\\n*");
	if ( re.search(message) != -1 ) {
		logMessage("Telnet&gt; " + tr("session correctly opened"));
		return;
	}
	re.setPattern("\\*\\*\\* telnet session will close in (\\d+)s\\r*\\n*");
	if ( re.search(message) != -1 ) {
		logMessage(
			"Telnet&gt; " + tr("session is being closed in %1 seconds")
				.arg(re.cap(1)),
			wlWarning
			);
		return;
	}
	// End telnet message handling
	
	// Start HTTPd message handling
	re.setPattern("sorry! No such name - (.*) !");
	if ( re.search(message) != -1 ) {
		logMessage(
			"HTTPd&gt; " + tr("requested non-existant file '%1'")
				.arg(re.cap(1)),
			wlLowWarning
			);
		return;
	}
	// End HTTPd message handling
	
	re.setPattern("Be Attacked:(.*),src:ip:(\\d+.\\d+.\\d+.\\d+),dst:port:(\\d+)\\r*\\n*");
	if ( re.search(message) != -1 ) {
		QString atkType;
		
		if ( re.cap(1) == "NET_BUS_SCAN-TCP" )
			atkType = tr("NetBus Scan [TCP]");
		else
			atkType = re.cap(1);
			
		logMessage(
			"Security&gt; " + tr("IP %1 scanned port %2, seems attack %3")
				.arg(re.cap(2))
				.arg(re.cap(3))
				.arg(atkType),
			wlSecurity
			);
	}
	
	re.setPattern("EVENT:Showtime\\r*\\n*Rate:.*Fast.*Interleave\\r*\\n*.*Down[ \\t]+(\\d+)[ \\t]+(\\d+)\\r*\\n*.*Up[ \\t]+(\\d+)[\\t ]+(\\d+)\\r*\\n*");
	if ( re.search(message) != -1 ) {
		logMessage(
			"Connection&gt; " + tr("Showtime connection enabled. Fast %1 / %2. Interleave %1 / %2.").
				arg(re.cap(1)).arg(re.cap(3)).
				arg(re.cap(2)).arg(re.cap(4)),
			wlInfo
			);
	}
}

/*!
\brief Output a message to the log widget
\param message Message to output
\param warn Warning level of the message (see WarningLevel)

\note This function also sends the events to KNotifyClient
*/
void EventsLogWidget::logMessage(const QString&message, WarningLevel warn)
{
	QString prefix, suffix;
	
	switch(warn)
	{
		case wlInfo:
			prefix = "<font>";	// We need to set them, else it won't
			suffix = "</font>";	// parse the right &gt; element
			KNotifyClient::event(winId(), "infomessage", message);
			break;
		case wlLowWarning:
			prefix = "<font color='orange'>";
			suffix = "</font>";
			KNotifyClient::event(winId(), "lowwarningmessage", message);
			break;
		case wlWarning:
			prefix = "<font color='orange'>";
			suffix = "</font>";
			KNotifyClient::event(winId(), "warningmessage", message);
			break;
		case wlAlert:
			prefix = "<font color='red'>";
			suffix = "</font>";
			KNotifyClient::event(winId(), "alertmessage", message);
			break;
		case wlSecurity:
			prefix = "<b><font color='red'>";
			suffix = "</font></b>";
			KNotifyClient::event(winId(), "securitymessage", message);
			break;
	}
	
	eventsOutput->append(prefix + message + suffix);
}

#include "eventslogwidget.moc"
