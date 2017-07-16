#include "sqluserstore.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>

#include <QRegularExpression>

#include <QJsonDocument>
#include <QJsonObject>

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QLoggingCategory>

using namespace Cutelyst;

SqlUserStore::SqlUserStore(QObject *parent) : AuthenticationStore(parent)
{

}

Cutelyst::AuthenticationUser SqlUserStore::findUser(Cutelyst::Context *c, const Cutelyst::ParamsMultiMap &userinfo)
{
    Q_UNUSED(c)
    QSqlQuery query = CPreparedSqlQueryThreadForDB(
                QStringLiteral("SELECT * FROM users WHERE email = :email"),
                QStringLiteral("cmlyst"));
    query.bindValue(QStringLiteral(":email"), userinfo.value(QStringLiteral("email")));
    if (query.exec() && query.next()) {
        QVariant userId = query.value(QStringLiteral("id"));

        AuthenticationUser user(userId.toString());

        int columns = query.record().count();
        // send column headers
        QStringList cols;
        const QSqlRecord record = query.record();
        for (int j = 0; j < columns; ++j) {
            cols.append(record.fieldName(j));
        }

        for (int j = 0; j < columns; ++j) {
            user.insert(cols.at(j), query.value(j));
        }

        return user;
    }

    return AuthenticationUser();
}

QString SqlUserStore::addUser(const ParamsMultiMap &user, bool replace)
{
    QSqlQuery query;
    if (replace) {
        query = CPreparedSqlQueryThreadForDB(
                    QStringLiteral("INSERT OR REPLACE INTO users "
                                   "(slug, email, password, json) "
                                   "VALUES "
                                   "(:slug, :email, :password, :json)"),
                    QStringLiteral("cmlyst"));
    } else {
        query = CPreparedSqlQueryThreadForDB(
                    QStringLiteral("INSERT INTO users "
                                   "(slug, email, password, json) "
                                   "VALUES "
                                   "(:slug, :email, :password, :json)"),
                    QStringLiteral("cmlyst"));
    }

    const QString name = user.value(QStringLiteral("name"));
    QString slug = name;
    if (slug.isEmpty()) {
        slug  = name.section(QLatin1Char(' '), 0, 0);
    }
    slug.remove(QRegularExpression(QStringLiteral("[^\\w]")));
    slug = slug.left(50).toLower().toHtmlEscaped();
    query.bindValue(QStringLiteral(":slug"), slug);

    query.bindValue(QStringLiteral(":email"), user.value(QStringLiteral("email")));
    query.bindValue(QStringLiteral(":password"), user.value(QStringLiteral("password")));

    QJsonObject obj;
    obj.insert(QStringLiteral("name"),
               name.left(150).toHtmlEscaped());
    query.bindValue(QStringLiteral(":json"), QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));

    if (!query.exec()) {
        qDebug() << "Failed to add new user:" << query.lastError().databaseText() << user;
        return QString();
    }
    return slug;
}
