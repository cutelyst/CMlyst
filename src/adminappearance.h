/***************************************************************************
 *   Copyright (C) 2015 Daniel Nicoletti <dantti12@gmail.com>              *
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

#ifndef ADMINAPPEARANCE_H
#define ADMINAPPEARANCE_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class AdminAppearance : public Controller
{
    Q_OBJECT
    C_NAMESPACE(".admin/appearance")
public:
    explicit AdminAppearance(QObject *parent = 0);
    ~AdminAppearance();

    C_ATTR(index, :Path :Args)
    void index(Context *ctx);

    C_ATTR(menus, :Local :Args)
    void menus(Context *ctx);

    C_ATTR(menus_remove, :Path(menus/remove))
    void menus_remove(Context *ctx, const QString &id);

    C_ATTR(menus_new, :Path(menus/new))
    void menus_new(Context *ctx);

    C_ATTR(menus_edit, :Path(menus/edit))
    void menus_edit(Context *ctx, const QString &id);
};

#endif // ADMINAPPEARANCE_H
