/*
 * Copyright (C) 2021 Daniel Nicoletti <dantti12@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <QCoreApplication>

#include <Cutelyst/WSGI/wsgi.h>

#include "cmlyst.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(QStringLiteral("Cutelyst"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("cutelyst.org"));
    QCoreApplication::setApplicationName(QStringLiteral("cutelyst-wsgi"));
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0.0"));

    CWSGI::WSGI server;
    server.setStaticMap({
                            QStringLiteral("/static=root/static")
                        });
    server.setChdir2(QStringLiteral(CMLYST_ROOT));

    QCoreApplication app(argc, argv);

    server.parseCommandLine(app.arguments());

    return server.exec(new CMlyst{});
}
