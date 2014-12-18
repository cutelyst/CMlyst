#include "adminsettings.h"

#include <QSettings>

AdminSettings::AdminSettings(QObject *parent) :
    Controller(parent)
{

}

void AdminSettings::index(Context *ctx)
{
    QSettings settings("site.conf", QSettings::IniFormat);
    settings.beginGroup("General");

    if (ctx->req()->method() == "POST") {
        ParamsMultiMap params = ctx->request()->bodyParam();
        settings.setValue("title", params.value("title"));
        settings.setValue("tagline", params.value("tagline"));
        settings.setValue("theme", params.value("theme"));
    }
    ctx->stash({
                   {"template", "settings/index.html"},
                   {"title", settings.value("title")},
                   {"tagline", settings.value("tagline")},
                   {"currentTheme", settings.value("theme")}
               });



    settings.endGroup();
}
