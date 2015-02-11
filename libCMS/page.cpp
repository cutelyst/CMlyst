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

using namespace CMS;

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
    d->path = readablePath(path);
}

QString Page::author() const
{
    Q_D(const Page);
    return d->author;
}

void Page::setAuthor(const QString &author)
{
    Q_D(Page);
    d->author = author;
}

QString Page::content() const
{
    Q_D(const Page);
    return d->content;
}

void Page::setContent(const QString &body)
{
    Q_D(Page);
    d->content = body;
}

QDateTime Page::modified() const
{
    Q_D(const Page);
    // TODO fix Grantlee
    return d->modified.toLocalTime();
}

void Page::setModified(const QDateTime &dateTime)
{
    Q_D(Page);
    d->modified = dateTime;
}

QDateTime Page::created() const
{
    Q_D(const Page);
    // TODO fix Grantlee
    return d->created.toLocalTime();
}

void Page::setCreated(const QDateTime &dateTime)
{
    Q_D(Page);
    d->created = dateTime;
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

bool Page::blog() const
{
    Q_D(const Page);
    return d->blog;
}

void Page::setBlog(bool enable)
{
    Q_D(Page);
    d->blog = enable;
}

bool Page::allowComments() const
{
    Q_D(const Page);
    return d->allowComments;
}

void Page::setAllowComments(bool allow)
{
    Q_D(Page);
    d->allowComments = allow;
}

QString Page::readablePath(const QString &path)
{
    return path.simplified().replace(QLatin1Char(' '), QLatin1String("-"));
}
