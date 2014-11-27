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

#include "root.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/authentication.h>
#include <Cutelyst/view.h>

#include <QTimer>
#include <QSqlRecord>
#include <QStringBuilder>
#include <QDebug>

Root::Root()
{
    qDebug() << Q_FUNC_INFO;
    m_view = new View("Grantlee", this);
    m_view->setIncludePath("/home/daniel/code/untitled/root/src");
    m_view->setTemplateExtension(".html");
    m_view->setWrapper("wrapper.html");
}

Root::~Root()
{
}

void Root::notFound(Context *c)
{
    c->stash()[QLatin1String("template")] = "404.html";
    c->res()->setStatus(404);
}

void Root::End(Context *c)
{
    qDebug() << "*** Root::End()";
//    c->stash()["template"] = "/home/daniel/code/untitled/root/src/login.html";

    m_view->render(c);
}

QVariant Root::sqlQueryToStash(QSqlQuery *query, bool singleRow)
{
    int colunas = query->record().count();
    // send column headers
    QStringList cols;
    for (int j = 0; j < colunas; ++j) {
        cols << query->record().fieldName(j);
    }

    QList<QVariantHash> lines;
    while (query->next()) {
        QVariantHash line;
        for (int j = 0; j < colunas; ++j) {
            line.insert(cols.at(j),
                        query->value(j));
        }

        if (singleRow) {
            return line;
        }
        lines << line;
    }

    if (singleRow) {
        return QVariant();
    }

    return qVariantFromValue(lines);
}

void Root::bindToQuery(QSqlQuery *query, const QMultiHash<QString, QString> &params, bool htmlEscaped)
{
    QMultiHash<QString, QString>::ConstIterator it = params.constBegin();
    if (htmlEscaped) {
        while (it != params.constEnd()) {
            query->bindValue(QLatin1Char(':') % it.key(), it.value().toHtmlEscaped());
            ++it;
        }
    } else {
        while (it != params.constEnd()) {
            query->bindValue(QLatin1Char(':') % it.key(), it.value());
            ++it;
        }
    }
}

void Root::create(Context *ctx)
{
    qDebug() << Q_FUNC_INFO;
}
