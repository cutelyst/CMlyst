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

    if (QSqlDatabase::contains(QStringLiteral("cmlyst"))) {
        return true;
    }

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
        return false;
    }
    return true;
}

Page *createPageObj(const QSqlQuery &query)
{
    auto page = new Page;
    page->setAllowComments(query.value(QStringLiteral("allow_comments")).toBool());
    page->setAuthor(query.value(QStringLiteral("author")).toString());
    page->setBlog(query.value(QStringLiteral("blog")).toBool());
    page->setContent(query.value(QStringLiteral("content")).toString());
    page->setCreated(query.value(QStringLiteral("created")).toDateTime());
    page->setModified(query.value(QStringLiteral("modified")).toDateTime());
    page->setName(query.value(QStringLiteral("name")).toString());
    page->setNavigationLabel(query.value(QStringLiteral("navigation_label")).toString());
    page->setPath(query.value(QStringLiteral("path")).toString());
    return page;
}

Page *SqlEngine::getPage(const QString &path)
{
    qDebug() << Q_FUNC_INFO << path;
    QSqlQuery query = CPreparedSqlQueryForDatabase(QStringLiteral("SELECT path, name, navigation_label, author, content,"
                                                                  " modified, created, tags, blog, allow_comments "
                                                                  "FROM pages "
                                                                  "WHERE path = :path"),
                                                   QSqlDatabase::database(QStringLiteral("cmlyst")));
    query.bindValue(QStringLiteral(":path"), path);
    if (query.exec() && query.next()) {
        return createPageObj(query);
    }
    return 0;
}

Page *SqlEngine::getPageToEdit(const QString &path)
{
    qDebug() << Q_FUNC_INFO;
    return getPage(path);
}

QString sortString(Engine::SortFlags sort)
{
    if (sort & Engine::Reversed) {
        if (sort & Engine::Name) {
            return QStringLiteral(" ORDER BY name ASC ");
        } else if (sort & Engine::Date) {
            return QStringLiteral(" ORDER BY date ASC ");
        }
    } else {
        if (sort & Engine::Name) {
            return QStringLiteral(" ORDER BY name DESC ");
        } else if (sort & Engine::Date) {
            return QStringLiteral(" ORDER BY date DESC ");
        }
    }
}

QList<Page *> SqlEngine::listPages(Engine::Filters filters, Engine::SortFlags sort, int depth, int limit)
{
    qDebug() << Q_FUNC_INFO;
    QList<Page *> ret;
    QSqlQuery query;
    if (filters == Engine::Pages) {
        query = CPreparedSqlQueryForDatabase(QStringLiteral("SELECT path, name, navigation_label, author, content,"
                                                            " modified, created, tags, blog, allow_comments "
                                                            "FROM pages "
                                                            "WHERE blog = 0 "
                                                            "LIMIT :limit "
                                                            ),
                                             QSqlDatabase::database(QStringLiteral("cmlyst")));
    } else if (filters == Engine::Posts) {
        query = CPreparedSqlQueryForDatabase(QStringLiteral("SELECT path, name, navigation_label, author, content,"
                                                            " modified, created, tags, blog, allow_comments "
                                                            "FROM pages "
                                                            "WHERE blog = 1 "
                                                            "LIMIT :limit "
                                                            ),
                                             QSqlDatabase::database(QStringLiteral("cmlyst")));
    } else {
        query = CPreparedSqlQueryForDatabase(QStringLiteral("SELECT path, name, navigation_label, author, content,"
                                                            " modified, created, tags, blog, allow_comments "
                                                            "FROM pages "
                                                            "LIMIT :limit "
                                                            ),
                                             QSqlDatabase::database(QStringLiteral("cmlyst")));
    }

    query.bindValue(QStringLiteral(":limit"), limit);
    if (query.exec()) {
        while (query.next()) {
            ret.append(createPageObj(query));
        }
    }
    return ret;
}

bool SqlEngine::savePageBackend(Page *page)
{
    qDebug() << Q_FUNC_INFO << page->path();
    QSqlQuery query = CPreparedSqlQueryForDatabase(QStringLiteral("INSERT OR REPLACE INTO pages "
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
        qWarning() << "Failed to save page" << query.lastError().driverText();
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
