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

#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QHash>

namespace CMS {

class Page;
class Menu;
class EnginePrivate;
class Engine : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Engine)
public:
    explicit Engine(QObject *parent = 0);
    virtual ~Engine();

    bool init(const QHash<QString, QString> &settings);

    virtual Page *getPage(const QString &path);

    virtual Page *getPageToEdit(const QString &path) const;
    virtual bool savePage(Page *page);

    /**
     * Returns the available pages,
     * when depth is -1 all pages are listed
     */
    virtual QList<Page *> listPages(int depth = -1);

    virtual QHash<QString, Menu *> menus();

    virtual QHash<QString, Menu *> menuLocations();

    virtual bool saveMenu(Menu *menu);
    bool saveMenus(const QList<Menu *> &menus);

    virtual QDateTime lastModified();

protected:
    EnginePrivate *d_ptr;
};

}

#endif // ENGINE_H
