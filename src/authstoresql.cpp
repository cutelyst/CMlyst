#include "authstoresql.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>
#include <QSqlError>
#include <QStringList>
#include <QDebug>

AuthStoreSql::AuthStoreSql()
{
    m_idField = "username";
}

Authentication::User AuthStoreSql::findUser(Context *ctx, const CStringHash &userinfo)
{
    qDebug() << "FIND USER -> " << userinfo;
    QString id = userinfo[m_idField];

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :id");
    query.bindValue(":id", id);
    query.exec();
    qDebug() << query.executedQuery() <<  query.size();
    if (query.exec() && query.next()) {
        qDebug() << "FOUND USER -> " << query.value("id");
        Authentication::User user(query.value("id").toString());

        int colunas = query.record().count();
        // send column headers
        QStringList cols;
        for (int j = 0; j < colunas; ++j) {
            cols << query.record().fieldName(j);
        }

        for (int j = 0; j < colunas; ++j) {
            user.insert(cols.at(j),
                        query.value(j).toString());
        }

        return user;
    }
    qDebug() << query.lastError().text();

    return Authentication::User();
}
