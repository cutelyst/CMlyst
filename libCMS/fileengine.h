#ifndef FILEENGINE_H
#define FILEENGINE_H

#include <QObject>

#include "engine.h"

class FileEngine : public Engine
{
    Q_OBJECT
public:
    FileEngine();
    ~FileEngine();

    bool init(const QHash<QString, QString> &settings);

    virtual Page *getPage(const QString &path) const;

    virtual Page *getPageToEdit(const QString &path) const;
    virtual bool savePage(Page *page);

    virtual QList<Page *> listPages() const;
};

#endif // FILEENGINE_H
