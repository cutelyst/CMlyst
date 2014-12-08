#ifndef FILEENGINE_P_H
#define FILEENGINE_P_H

#include <QDir>
#include <QHash>

#include "fileengine.h"

class FileEnginePrivate
{
public:
    QDir rootPath;
    QHash<QString, Page*> pages;
};

#endif // FILEENGINE_P_H

