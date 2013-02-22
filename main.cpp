/* Copyright © 2007 Michael Pyne <michael.pyne@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "MainWindowImpl.h"

#include <QtGui>
#include <QtCore>

/**
 * @todo All issues in list should be closed ASAP
 *  1. Drag&drop doesn't work
 *  2. App shows children only for first occurance of library
 *       -> if app depends on QtGui and QtCore, QtGui depends on QtCore,
 *          than 2 rows QtCore presented, but only one of them has children
 */

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MainWindowImpl mw(0);
    mw.show();

    QStringList arguments = app.arguments();
    if(arguments.count() > 1) {
        mw.openFile(arguments.last());
    }

    app.connect(&mw, SIGNAL(quit()), SLOT(quit()));

    return app.exec();
}

// vim: set ts=8 sw=4 et encoding=utf8:
