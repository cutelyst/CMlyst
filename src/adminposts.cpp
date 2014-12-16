#include "adminposts.h"

#include "root.h"

#include <Cutelyst/Plugins/authentication.h>
#include <Cutelyst/Application>

#include "../libCMS/fileengine.h"
#include "../libCMS/page.h"

#include <QDebug>

AdminPosts::AdminPosts(QObject *parent) :
    Controller(parent)
{
}

void AdminPosts::index(Context *ctx)
{
    ctx->stash()["post_type"] = "post";
//    QSqlQuery query;
//    query.prepare("SELECT p.id, p.title, u.username AS author, p.modified "
//                  "FROM u_posts p, u_users u "
//                  "WHERE p.user_id = u.id "
//                  "ORDER BY 2");
//    if (!query.exec()) {
//        ctx->stash()["error_msg"] = query.lastError().text();
//        return;
//    } else if (query.size() == 0){
//        ctx->res()->redirect("/setup");
//        return;
//    }

//    ctx->stash()["posts"] = Root::sqlQueryToStash(&query);
qDebug() << ctx->request()->path() << ctx->req()->args();
    ctx->stash()["template"] = "posts/index.html";
}

void AdminPosts::create(Context *ctx)
{
    ctx->stash()["post_type"] = "post";
    qDebug() << Q_FUNC_INFO;
    ParamsMultiMap params = ctx->request()->bodyParam();
    QString title = params.value("title");
    QString path = params.value("path");
    QString content = params.value("content");
    if (ctx->req()->method() == "POST") {
        Authentication *auth = ctx->plugin<Authentication*>();

        qDebug() << title;
        qDebug() << path;
        qDebug() << content;


        CMS::FileEngine *engine = new CMS::FileEngine(ctx);
        engine->init({
                         {"root", qgetenv("CMS_ROOT_PATH")}
                     });

        CMS::Page *page = engine->getPageToEdit(path);
        page->setContent(content.toUtf8());
        page->setName(title);
        qDebug() << page->path();

        bool ret = engine->savePage(page);
        if (ret) {
            ctx->stash()["status"] = "Page saved";
        }

        qDebug() << "saved" << ret;



//        QSqlQuery query;
//        query.prepare("INSERT INTO u_posts (user_id, title, content) "
//                      "VALUES (:user_id, :title, :content)");
//        query.bindValue(":user_id", auth->user().id());
//        query.bindValue(":title", title);
//        query.bindValue(":content", content);
//        if (!query.exec()) {
//            ctx->stash()["error_msg"] = query.lastError().text();
//        } else {
//            ctx->res()->redirect("/");
//            return;
//        }
    }

    ctx->stash()["title"] = title;
    ctx->stash()["path"] = path;
    ctx->stash()["content"] = content;
    ctx->stash()["template"] = "posts/create.html";
}
