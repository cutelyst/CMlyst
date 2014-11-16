#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>

#include <Cutelyst/Application>
#include <Cutelyst/enginehttp.h>

#include "untitled.h"

using namespace Cutelyst;

void registerPlugins(Context *ctx)
{

}

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Cecilet Ti");
    QCoreApplication::setOrganizationDomain("ceciletti.com.br");
    QCoreApplication::setApplicationName("CuteWeb");
    QCoreApplication::setApplicationVersion("0.0.1");

    QCoreApplication app(argc, argv);
    Untitled *untitled = new Untitled;

    EngineHttp *engine = new EngineHttp;
    if (engine->initApplication(untitled, false)) {
        return app.exec();
    }
    return 1;
}

