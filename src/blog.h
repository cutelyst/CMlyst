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

#ifndef BLOG_H
#define BLOG_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Blog : public Controller
{
    Q_OBJECT

    C_NAMESPACE("")
public:
    Blog();

    Q_INVOKABLE void index(Context *ctx, Path, Args);

    Q_INVOKABLE void page(Context *ctx, const QString &pageName, Path, Args);

    Q_INVOKABLE void blog(Context *ctx,
                          const QString &year,
                          const QString &month,
                          const QString &day,
                          const QString &postName,
                          Path,
                          Args);
};

#endif // BLOG_H
