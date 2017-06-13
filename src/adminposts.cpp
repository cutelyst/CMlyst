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

#include "adminposts.h"

#include "root.h"

#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Application>

#include "libCMS/page.h"

#include <QDebug>

AdminPosts::AdminPosts(Application *app) : AdminPages(app)
{
}

void AdminPosts::index(Context *c)
{
    AdminPages::index(c, QStringLiteral("post"), CMS::Engine::Posts);
}

void AdminPosts::create(Context *c)
{
    AdminPages::create(c,  QStringLiteral("post"), false);
}

void AdminPosts::edit(Context *c, const QString &id)
{
    AdminPages::edit(c, id, QStringLiteral("post"), false);
}
