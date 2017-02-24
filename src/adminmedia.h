/***************************************************************************
 *   Copyright (C) 2014-2017 Daniel Nicoletti <dantti12@gmail.com>         *
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

#ifndef ADMINMEDIA_H
#define ADMINMEDIA_H

#include <QObject>
#include <Cutelyst/Controller>

using namespace Cutelyst;

class AdminMedia : public Controller
{
    Q_OBJECT
    C_NAMESPACE(".admin/media")
public:
    explicit AdminMedia(QObject *app = 0);
    ~AdminMedia();

    C_ATTR(index, :Path :AutoArgs)
    void index(Context *c);

    C_ATTR(upload, :Local :AutoArgs)
    void upload(Context *c);

    C_ATTR(remove, :Local :AutoArgs)
    void remove(Context *c, const QStringList &path);
};

#endif // ADMINMEDIA_H
