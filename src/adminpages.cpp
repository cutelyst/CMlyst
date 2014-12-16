#include "adminpages.h"

#include <Cutelyst/Plugins/authentication.h>
#include <Cutelyst/Application>

#include "../libCMS/fileengine.h"
#include "../libCMS/page.h"

AdminPages::AdminPages()
{

}

AdminPages::~AdminPages()
{

}

void AdminPages::index(Context *ctx)
{
    ctx->stash()["post_type"] = "page";

    CMS::FileEngine *engine = new CMS::FileEngine(ctx);
    engine->init({
                     {"root", qgetenv("CMS_ROOT_PATH")}
                 });
    QList<CMS::Page *> pages = engine->listPages();
    ctx->stash()["posts"] = QVariant::fromValue(pages);

    ctx->stash()["template"] = "posts/index.html";
}

void AdminPages::create(Context *ctx)
{
    qDebug() << Q_FUNC_INFO;
    ctx->stash()["post_type"] = "page";

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
    }

    ctx->stash()["title"] = title;
    ctx->stash()["path"] = path;
    ctx->stash()["content"] = content;
    ctx->stash()["template"] = "posts/create.html";
}

void AdminPages::edit(Context *ctx)
{
    qDebug() << Q_FUNC_INFO;
    ctx->stash()["post_type"] = "page";

    CMS::FileEngine *engine = new CMS::FileEngine(ctx);
    engine->init({
                     {"root", qgetenv("CMS_ROOT_PATH")}
                 });

    QStringList args = ctx->request()->args();
    args.removeFirst();
    QString path = args.join(QLatin1Char('/'));
    qDebug() << Q_FUNC_INFO << path <<  ctx->request()->args();
    CMS::Page *page = engine->getPageToEdit(path);
    QString title = page->name();
    QString content = page->content();

    if (ctx->req()->method() == "POST") {
        ParamsMultiMap params = ctx->request()->bodyParam();
        title = params.value("title");
        path = params.value("path");
        content = params.value("content");

        Authentication *auth = ctx->plugin<Authentication*>();

        qDebug() << title;
        qDebug() << path;
        qDebug() << content;


        if (path != params.value("path")) {
            qDebug() << "not yet supported";
        }

        page->setContent(content.toUtf8());
        page->setName(title);
        qDebug() << page->path();

        bool ret = engine->savePage(page);
        if (ret) {
            ctx->stash()["status"] = "Page saved";
        }

        qDebug() << "saved" << ret;
    }

    ctx->stash()["title"] = title;
    ctx->stash()["path"] = path;
    ctx->stash()["content"] = content;
    ctx->stash()["template"] = "posts/create.html";
}

