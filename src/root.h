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
#include <Cutelyst/Actions/actionrest.h>

#include <QSqlQuery>

using namespace Cutelyst;

namespace Cutelyst {
class View;
}

class Root : public Controller
{
    Q_OBJECT
    C_NAMESPACE("")
    Q_CLASSINFO("create_Path", "new")
public:
    Root();
    ~Root();

public slots:
    void notFound(Context *c, Path);

    static QVariant sqlQueryToStash(QSqlQuery *query, bool singleRow = false);
    static void bindToQuery(QSqlQuery *query, const QMultiHash<QString, QString> &params, bool htmlEscaped = true);


    void create(Cutelyst::Context *ctx, Args, ActionREST*);

    void create_POST(Cutelyst::Context *ctx);


private slots:
    void create_GET(Cutelyst::Context *ctx);

private slots:
    void End(Context *c);

private:
    View *m_view;
};

#endif // ROOT_H
