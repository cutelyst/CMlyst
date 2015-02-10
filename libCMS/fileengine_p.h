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

#ifndef FILEENGINE_P_H
#define FILEENGINE_P_H

#include <QDir>
#include <QHash>
#include <QSettings>
#include <QFileInfo>
#include <QDateTime>

#include "fileengine.h"

namespace CMS {

class FileEnginePrivate
{
public:
    Menu *createMenu(const QString &name, QObject *parent);

    QDir rootPath;
    QDir pagesPath;
    QSettings *settings;
    QFileInfo settingsInfo;
    QDateTime mainSettingsDT;
    QHash<QString, QString> mainSettings;
    QHash<QString, Page*> pathPages;
    QList<Page*> pages;
    QList<Page*> posts;
    QDateTime menusDT;
    QList<CMS::Menu *> menus;
    QDateTime menuLocationsDT;
    QHash<QString, CMS::Menu *> menuLocations;
};

}

#endif // FILEENGINE_P_H

