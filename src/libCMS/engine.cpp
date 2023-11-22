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
#include "page.h"

#include <QRegularExpression>
#include <QStringList>
#include <QDateTime>
#include <QDebug>

using namespace CMS;

Engine::Engine(QObject *parent) : QObject(parent)
{

}

Engine::~Engine()
{

}

int Engine::savePage(Cutelyst::Context *c, Page *page)
{
    int ret = savePageBackend(page);
    if (ret) {
        QList<Menu *> autoMenus = menus();
        Q_FOREACH (Menu *menu, autoMenus) {
            if (menu->autoAddPages()) {
                menu->appendEntry(page->title(), page->path());
                saveMenu(c, menu, true);
            }
        }
    }
    return ret;
}

Menu *Engine::menu(const QString &id)
{
    Q_FOREACH (Menu *menu, menus()) {
        if (menu->id() == id) {
            return menu;
        }
    }
    return 0;
}

QVariant Engine::menusProperty()
{
    return QVariant::fromValue(menuLocations());
}

bool Engine::saveMenu(Cutelyst::Context *c, Menu *menu, bool replace)
{
    Q_UNUSED(c)
    Q_UNUSED(menu)
    Q_UNUSED(replace)
    return false;
}

bool Engine::removeMenu(Cutelyst::Context *c, const QString &name)
{
    Q_UNUSED(c)
    Q_UNUSED(name)
    return false;
}

bool Engine::saveMenus(Cutelyst::Context *c, const QList<Menu *> &menus)
{
    Q_FOREACH (Menu *menu, menus) {
        if (!saveMenu(c, menu, true)) {
            return false;
        }
    }

    return true;
}

QDateTime Engine::lastModified()
{
    return QDateTime();
}

QVariant Engine::settingsProperty()
{
    return QVariant::fromValue(settings());
}

QString Engine::normalizePath(const QString &path)
{
    QString ret;
    // "/foo/bar/Iam a big...path" turns into
    // "/foo/bar/iam-a-big-path"
    QStringList parts = path.split(u'/', Qt::SkipEmptyParts);
    for (int i = 0; i < parts.size(); ++i) {
        parts.replace(i, normalizeTitle(parts.at(i)));
    }
    ret = parts.join(u'/');
    if (ret.isNull()) {
        ret = QLatin1String("");
    }
    return ret;
}

QString Engine::normalizeTitle(const QString &title)
{
    // "Iam a big/small...path" turns into
    // "/foo/bar/iam-a-bigsmall-path"
    QString ret = title.toLower();

    // turn separators into space
    ret.replace(u'.', QChar::Space);
    ret.replace(u'-', QChar::Space);

    // remove everything that is not a word or space
    static QRegularExpression re(QStringLiteral("[^\\w\\s]"));
    ret.remove(re);

    // remove abused space
    ret = ret.simplified();

    // turn space into dashes
    ret.replace(QChar::Space, QLatin1Char('-'));

    return ret;
}
