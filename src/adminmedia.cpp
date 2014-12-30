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

#include "adminmedia.h"

#include <Cutelyst/Upload>

#include <QDir>
#include <QDirIterator>
#include <QStringBuilder>
#include <QDebug>

AdminMedia::AdminMedia(QObject *parent) : Controller(parent)
{

}

AdminMedia::~AdminMedia()
{

}

void AdminMedia::index(Context *ctx)
{
    QDir mediaDir(ctx->config("DataLocation").toString() % QLatin1String("/media"));
    qDebug() << mediaDir.absolutePath();

    QStringList files;
    QDirIterator it(mediaDir.absolutePath(),
                    QDir::Files | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        files.append(it.next());
    }
    files.sort(Qt::CaseInsensitive);
    qDebug() << files;

    int removeSize = mediaDir.absolutePath().size();
    QList<QHash<QString, QString> > filesHash;
    foreach (const QString &file, files) {
        QFileInfo fileInfo(file);
        QString urlPath = file;
        urlPath.remove(0 ,removeSize);
        qDebug() << urlPath;
        urlPath.prepend("/.media");

        QHash<QString, QString> hash;
        hash.insert("name", fileInfo.fileName());
        hash.insert("modified", fileInfo.lastModified().toString());
        hash.insert("author", fileInfo.owner());
        hash.insert("url", ctx->uriFor(urlPath.toLatin1()).toString());
        filesHash.append(hash);
    }
    qDebug() << filesHash;

    ctx->stash({
                   {"template", "media/index.html"},
                   {"files", QVariant::fromValue(filesHash)}
               });
}

void AdminMedia::upload(Context *ctx)
{
    QDir mediaDir(ctx->config("DataLocation").toString() % QLatin1String("/media"));
    if (!mediaDir.exists() && !mediaDir.mkpath(mediaDir.absolutePath())) {
        qWarning() << "Could not create media directory" << mediaDir.absolutePath();
        ctx->response()->redirect(ctx->uriForNoArgs(actionFor("index"), {
                                                        {"error_msg", "Failed to save file"}
                                                    }));
        return;
    }

    // TODO this is NOT working...
    QFile link(ctx->config("DataLocation").toString() % QLatin1String("/.media"));
    if (!link.exists() && !QFile::link(mediaDir.absolutePath(), mediaDir.absolutePath())) {
        qWarning() << "Could not create link media directory" << mediaDir.absolutePath() << link.fileName();
        ctx->response()->redirect(ctx->uriForNoArgs(actionFor("index"), {
                                                        {"error_msg", "Failed to save file"}
                                                    }));
        return;
    }

    QDir fileDir(mediaDir.absolutePath() % QDateTime::currentDateTimeUtc().toString("/yyyy/MM"));
    if (!fileDir.exists() && !fileDir.mkpath(fileDir.absolutePath())) {
        qWarning() << "Could not create media directory" << fileDir.absolutePath();
        ctx->response()->redirect(ctx->uriForNoArgs(actionFor("index"), {
                                                        {"error_msg", "Failed to save file"}
                                                    }));
        return;
    }

    Request *request = ctx->request();
    Upload *upload = request->upload("file");
    if (!upload) {
        qWarning() << "Could not find upload";
        ctx->response()->redirect(ctx->uriForNoArgs(actionFor("index"), {
                                                        {"error_msg", "Failed to save file"}
                                                    }));
        return;
    }

    QString filepath = fileDir.absoluteFilePath(upload->filename());
    if (!upload->save(filepath)) {
        qWarning() << "Could not save upload" << filepath;
        ctx->response()->redirect(ctx->uriForNoArgs(actionFor("index"), {
                                                        {"error_msg", "Failed to save file"}
                                                    }));
        return;
    }

    ctx->response()->redirect(ctx->uriFor(actionFor("index")));
}

