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

#ifndef ADMIN_H
#define ADMIN_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

namespace Cutelyst {
class View;
}

class Admin : public Controller
{
    Q_OBJECT
    Q_CLASSINFO(C_NAMESPACE, ".admin")
public:
    Admin();

public slots:
    bool Auto(Context *ctx);

    void notFound(Context *ctx, Path);

    void End(Context *ctx);

private:
    View *m_view;
};

#endif // ADMIN_H
