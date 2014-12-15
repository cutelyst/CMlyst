#ifndef FILEENGINE_P_H
#define FILEENGINE_P_H

#include <QDir>
#include <QHash>

#include "fileengine.h"

namespace CMS {

class FileEnginePrivate
{
public:
    QDir rootPath;
    QDir pagesPath;
    QHash<QString, Page*> pages;
};

}

#endif // FILEENGINE_P_H

