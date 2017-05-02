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
#include "engine.h"

#include <QDebug>
#include <QUuid>

using namespace CMS;

Page::Page(QObject *parent) : QObject(parent)
    , d_ptr(new PagePrivate)
{
//    qDebug() << Q_FUNC_INFO;
}

Page::~Page()
{
    delete d_ptr;
    //    qDebug() << Q_FUNC_INFO;
}

int Page::id() const
{
    Q_D(const Page);
    return d->id;
}

void Page::setId(int id)
{
    Q_D(Page);
    d->id = id;
}

QString Page::uuid() const
{
    Q_D(const Page);
    return d->uuid;
}

void Page::setUuid(const QString &uuid)
{
    Q_D(Page);
    if (!uuid.isEmpty()) {
        d->uuid = uuid;
    } else {
        d->uuid = QUuid::createUuid().toString().remove(QLatin1Char('{')).remove(QLatin1Char('}'));
    }
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

Author Page::author() const
{
    Q_D(const Page);
    return d->author;
}

void Page::setAuthor(const Author &author)
{
    Q_D(Page);
    d->author = author;
}

Grantlee::SafeString Page::content() const
{
    Q_D(const Page);
    return d->content;
}

void Page::setContent(const QString &body, bool safe)
{
    Q_D(Page);
    d->content = Grantlee::SafeString(body, safe);
}

void Page::updateContent(const Grantlee::SafeString &body)
{
    Q_D(Page);
    d->content = body;
}

QDateTime Page::published() const
{
    Q_D(const Page);
    return d->published;
}

void Page::setPublished(const QDateTime &dateTime)
{
    Q_D(Page);
    d->published = dateTime;
}

QDateTime Page::updated() const
{
    Q_D(const Page);
    return d->updated;
}

void Page::setUpdated(const QDateTime &dateTime)
{
    Q_D(Page);
    d->updated = dateTime;
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

bool Page::page() const
{
    Q_D(const Page);
    return d->page;
}

void Page::setPage(bool enable)
{
    Q_D(Page);
    d->page = enable;
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
