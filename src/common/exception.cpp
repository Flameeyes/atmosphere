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
#include "exception.h"

namespace ATMOS {

/*!
\brief Returns the i18n type of the exception
\param ec ExceptionCode to translate to message
\return the i18n string which represent the exception
*/
const QString Exception::message(ExceptionCode ec)
{
	switch(ec)
	{
		case ecUnknown:
			return tr("Unknown exception");
		case ecMissingSupport:
			return tr("Missing support exception");
		case ecInvalidDriver:
			return tr("Invalid driver created");
	}
	
	return QString::null;
}

};

#include "exception.moc"
