/***************************************************************************
 *   Copyright (C) 2014 Daniel Nicoletti <dantti12@gmail.com>              *
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
 *   along with this program; see the file COPYING. If not, write to       *
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,  *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/

#ifndef ADMINSETUP_H
#define ADMINSETUP_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class AdminSetup : public Controller
{
    Q_OBJECT
    C_NAMESPACE("")
public:
    explicit AdminSetup(QObject *parent = 0);

    C_ATTR(setup, :Local :AutoArgs)
    void setup(Context *c);

    C_ATTR(edit, :Local :AutoArgs)
    void edit(Context *c, const QString &id);

    C_ATTR(remove_user, :Local :AutoArgs)
    void remove_user(Context *c, const QString &id);

    C_ATTR(status, :Path :AutoArgs)
    void status(Context *c);

    C_ATTR(End, :Private :ActionClass(RenderView) :View(admin))
    void End(Context *c);
};

#endif // ADMINSETUP_H
