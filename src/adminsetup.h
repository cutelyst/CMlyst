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

#include "cmengine.h"

using namespace Cutelyst;

class AdminSetup : public Controller, public CMEngine
{
    Q_OBJECT
    C_NAMESPACE("")
public:
    explicit AdminSetup(QObject *app = nullptr);

    C_ATTR(setup, :Local :AutoArgs)
    void setup(Context *c);

    C_ATTR(notFound, :Path)
    void notFound(Context *c);

    C_ATTR(End, :Private :ActionClass(RenderView) :View(admin))
    bool End(Context *c);
};

#endif // ADMINSETUP_H
