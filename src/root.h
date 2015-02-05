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
#include <QDir>

using namespace Cutelyst;

namespace CMS {
class Engine;
}

class Root : public Controller
{
    Q_OBJECT
    C_NAMESPACE("")
public:
    Root();
    ~Root();

public:
//    C_ATTR(notFound, :Path)
//    void notFound(Context *c);

    C_ATTR(page, :Path)
    void page(Cutelyst::Context *ctx);

private slots:
    bool Auto(Context *ctx);

protected:
    void init(Application *app);

private:
    C_ATTR(End, :ActionClass(RenderView))
    void End(Context *ctx);

    CMS::Engine *m_engine;
    QDir m_rootDir;
};

#endif // ROOT_H
