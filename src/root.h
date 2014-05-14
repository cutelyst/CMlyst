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

#ifndef ROOT_H
#define ROOT_H

#include <Cutelyst/Controller>

#include <QSqlQuery>

using namespace Cutelyst;

namespace Cutelyst {
class View;
}

class Root : public Controller
{
    Q_OBJECT
    Q_CLASSINFO("Namespace", "")
public:
    Root();

public:
    Q_INVOKABLE void notFound(Context *c, Path);

    Q_INVOKABLE void End(Context *c);

    static QVariant sqlQueryToStash(QSqlQuery *query, bool singleRow = false);
    static void bindToQuery(QSqlQuery *query, const QMultiHash<QString, QString> &params, bool htmlEscaped = true);

private:
    View *m_view;
};

#endif // ROOT_H
