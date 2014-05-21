#include "adminposts.h"

#include "root.h"

#include <Cutelyst/Plugin/authentication.h>

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

using namespace Plugin;

AdminPosts::AdminPosts(QObject *parent) :
    Controller(parent)
{
}

void AdminPosts::index(Context *ctx, Controller::Path, Controller::Args)
{
    QSqlQuery query;
    query.prepare("SELECT p.id, p.title, u.username AS author, p.modified "
                  "FROM u_posts p, u_users u "
                  "WHERE p.user_id = u.id "
                  "ORDER BY 2");
    if (!query.exec()) {
        ctx->stash()["error_msg"] = query.lastError().text();
        return;
    } else if (query.size() == 0){
        ctx->res()->redirect("/setup");
        return;
    }

    ctx->stash()["posts"] = Root::sqlQueryToStash(&query);

    ctx->stash()["template"] = "posts/index.html";
}

void AdminPosts::create(Context *ctx, Controller::Args)
{
    qDebug() << Q_FUNC_INFO;
    QString title = ctx->req()->param().value("title");
    QString content = ctx->req()->param().value("content");
    if (ctx->req()->method() == "POST") {
        Authentication *auth = ctx->plugin<Authentication*>();

        qDebug() << title << content;
        QSqlQuery query;
        query.prepare("INSERT INTO u_posts (user_id, title, content) "
                      "VALUES (:user_id, :title, :content)");
        query.bindValue(":user_id", auth->user().id());
        query.bindValue(":title", title);
        query.bindValue(":content", content);
        if (!query.exec()) {
            ctx->stash()["error_msg"] = query.lastError().text();
        } else {
            ctx->res()->redirect("/");
            return;
        }
    }

    ctx->stash()["title"] = title;
    ctx->stash()["content"] = content;
    ctx->stash()["template"] = "posts/create.html";
}
