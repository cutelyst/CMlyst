/***************************************************************************
 *   Copyright (C) 2014-2015 Daniel Nicoletti <dantti12@gmail.com>         *
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

#include "engine.h"
#include "menu.h"

#include <QStringList>
#include <QDateTime>

using namespace CMS;

Engine::Engine(QObject *parent) : QObject(parent)
{

}

Engine::~Engine()
{

}

bool Engine::init(const QHash<QString, QString> &settings)
{
    return true;
}

Page *Engine::getPage(const QString &path)
{
    return 0;
}

Page *Engine::getPageToEdit(const QString &path) const
{
    return 0;
}

bool Engine::savePage(Page *page)
{
    return false;
}

QList<Page *> Engine::listPages(int depth)
{
    Q_UNUSED(depth)
    return QList<Page *>();
}

QHash<QString, Menu *> Engine::menus()
{
    return QHash<QString, Menu *>();
}

QHash<QString, Menu *> Engine::menuLocations()
{
    QHash<QString, Menu *> ret;

    const QHash<QString, Menu *> &allMenus = menus();
    QHash<QString, Menu *>::ConstIterator it = allMenus.constBegin();
    while (it != allMenus.constEnd()) {
        Menu *menu = it.value();
        Q_FOREACH (const QString &location, menu->locations()) {
            ret.insert(location, menu);
        }
        ++it;
    }

    return ret;
}

bool Engine::saveMenu(Menu *menu)
{
    Q_UNUSED(menu)
    return false;
}

bool Engine::saveMenus(const QList<Menu *> &menus)
{
    Q_FOREACH (Menu *menu, menus) {
        if (!saveMenu(menu)) {
            return false;
        }
    }

    return true;
}

QDateTime Engine::lastModified()
{
    return QDateTime();
}

bool Engine::settingsIsWritable()
{
    return false;
}

QHash<QString, QString> Engine::settings()
{
    return QHash<QString, QString>();
}

QString Engine::settingsValue(const QString &key, const QString &defaultValue)
{
    Q_UNUSED(key)
    return defaultValue;
}

bool Engine::setSettingsValue(const QString &key, const QString &value)
{
    Q_UNUSED(key)
    Q_UNUSED(value)
    return false;
}
