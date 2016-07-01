#include "sqlengine.h"
#include "page.h"

#include <Cutelyst/Plugins/Utils/Sql>

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

using namespace CMS;
using namespace Cutelyst;

SqlEngine::SqlEngine(QObject *parent) : Engine(parent)
{

}

bool SqlEngine::init(const QHash<QString, QString> &settings)
{
    QString root = settings.value(QStringLiteral("root"));
    if (root.isEmpty()) {
        root = QDir::currentPath();
    }

    const QString dbPath = root + QLatin1String("/cmlyst.sqlite");
    bool create = !QFile::exists(dbPath);

    auto db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("cmlyst"));
    db.setDatabaseName(dbPath);
    if (db.open()) {
        qDebug() << "Database is open:" << dbPath;
        if (create) {
            createDb();
            qDebug() << "Database tables created";
        }
    } else {
        qCritical() << "Error opening database" << dbPath << db.lastError().driverText();
        exit(1);
    }
}

Page *SqlEngine::getPage(const QString &path)
{

}

Page *SqlEngine::getPageToEdit(const QString &path)
{

}

QList<Page *> SqlEngine::listPages(Engine::Filters filters, Engine::SortFlags sort, int depth, int limit)
{

}

bool SqlEngine::savePageBackend(Page *page)
{

    static QSqlQuery query = CPreparedSqlQueryForDatabase(QStringLiteral("INSERT INTO pages "
                                                                         "(path, name, navigation_label, author, content,"
                                                                         " modified, created, tags, blog, allow_comments) "
                                                                         "VALUES "
                                                                         "(:path, :name, :navigation_label, :author, :content,"
                                                                         " :modified, :created, :tags, :blog, :allow_comments)"),
                                                          QSqlDatabase::database(QStringLiteral("cmlyst")));
    query.bindValue(QStringLiteral(":path"), page->path());
    query.bindValue(QStringLiteral(":name"), page->name());
    query.bindValue(QStringLiteral(":navigation_label"), page->navigationLabel());
    query.bindValue(QStringLiteral(":author"), page->author());
    query.bindValue(QStringLiteral(":content"), page->content());
    query.bindValue(QStringLiteral(":modified"), page->modified());
    query.bindValue(QStringLiteral(":created"), page->created());
    query.bindValue(QStringLiteral(":tags"), page->tags());
    query.bindValue(QStringLiteral(":blog"), page->blog());
    query.bindValue(QStringLiteral(":allow_comments"), page->allowComments());
    if (!query.exec()) {
        qWarning() << "Failed to save page" << query.lastError().databaseText();
        return false;
    }
    return true;
}

void SqlEngine::createDb()
{
    QSqlQuery query(QSqlDatabase::database(QStringLiteral("cmlyst")));

    bool ret = query.exec("PRAGMA journal_mode = WAL");
    qDebug() << "PRAGMA journal_mode = WAL" << ret << query.lastError().driverText();

    if (!query.exec(QStringLiteral("CREATE TABLE pages ( "
                                   "path TEXT NOT NULL UNIQUE PRIMARY KEY "
                                   ", name TEXT "
                                   ", navigation_label TEXT "
                                   ", author TEXT "
                                   ", content TEXT "
                                   ", modified TEXT "
                                   ", created TEXT "
                                   ", tags TEXT "
                                   ", blog BOOL NOT NULL "
                                   ", allow_comments BOOL NOT NULL "
                                   ")"))) {
        qCritical() << "Error creating database" << query.lastError().text();
        exit(1);
    }

    if (!query.exec(QStringLiteral("CREATE TABLE settings "
                                   "( name TEXT NOT NULL PRIMARY KEY "
                                   ", value TEXT"
                                   ")"))) {
        qCritical() << "Error creating database" << query.lastError().text();
        exit(1);
    }
}
