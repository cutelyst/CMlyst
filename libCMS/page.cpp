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

#include "page_p.h"

Page::Page() :
    d_ptr(new PagePrivate)
{
}

Page::~Page()
{
    delete d_ptr;
}

QString Page::name() const
{
    Q_D(const Page);
    return d->name;
}

void Page::setName(const QString &name)
{
    Q_D(Page);
    d->name = name;
}

QString Page::navigationLabel() const
{
    Q_D(const Page);
    return d->navigationLabel;
}

void Page::setNavigationLabel(const QString &label)
{
    Q_D(Page);
    d->navigationLabel = label;
}

QString Page::path() const
{
    Q_D(const Page);
    return d->path;
}

void Page::setPath(const QString &path)
{
    Q_D(Page);
    d->path = path;
}

QByteArray Page::content() const
{
    Q_D(const Page);
    return d->content;
}

void Page::setContent(const QByteArray &body)
{
    Q_D(Page);
    d->content = body;
}

QDateTime Page::modified() const
{
    Q_D(const Page);
    return d->modified;
}

void Page::setModified(const QDateTime &dateTime)
{
    Q_D(Page);
    d->modified = dateTime;
}

QStringList Page::tags() const
{
    Q_D(const Page);
    return d->tags;
}

void Page::setTags(const QStringList &tags)
{
    Q_D(Page);
    d->tags = tags;
}
