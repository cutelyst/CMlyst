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

#ifndef FILEENGINE_H
#define FILEENGINE_H

#include <QObject>
#include <QUrl>

#include "engine.h"

namespace CMS {

class FileEnginePrivate;
class FileEngine : public Engine
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileEngine)
public:
    explicit FileEngine(QObject *parent = 0);
    ~FileEngine();

    virtual bool init(const QHash<QString, QString> &settings) override;

    virtual Page *getPage(const QString &path) override;

    virtual Page *getPageToEdit(const QString &path) override;

    virtual Page *loadPage(const QString &filename);

    virtual bool savePageBackend(Page *page) override;

    virtual QList<Page *> listPages(Filters filters = NoFilter,
                                    SortFlags sort = SortFlags(Date | Reversed),
                                    int depth = -1,
                                    int limit = -1) override;

    virtual QList<Menu *> menus() override;
    virtual QHash<QString, Menu *> menuLocations() override;

    virtual bool saveMenu(Menu *menu, bool replace) override;
    virtual bool removeMenu(const QString &name) override;

    virtual QDateTime lastModified();

    virtual bool settingsIsWritable();
    virtual QHash<QString, QString> settings();
    virtual QString settingsValue(const QString &key, const QString &defaultValue = QString()) const override;
    virtual bool setSettingsValue(const QString &key, const QString &value);

protected:
    FileEnginePrivate *d_ptr;

    void loadPages();
    void settingsDirChanged(const QString &path);
    void loadSettings();
};

}

#endif // FILEENGINE_H
