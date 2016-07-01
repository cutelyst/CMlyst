#ifndef SQLENGINE_H
#define SQLENGINE_H

#include <QObject>
#include <QUrl>

#include "engine.h"

namespace CMS {

class FileEnginePrivate;
class SqlEngine : public Engine
{
    Q_OBJECT
public:
    explicit SqlEngine(QObject *parent = 0);

    virtual bool init(const QHash<QString, QString> &settings) override;

    virtual Page *getPage(const QString &path) override;

    virtual Page *getPageToEdit(const QString &path) override;

    /**
     * Returns the available pages,
     * when depth is -1 all pages are listed
     */
    virtual QList<Page *> listPages(Filters filters = NoFilter,
                                    SortFlags sort = SortFlags(Date | Reversed),
                                    int depth = -1,
                                    int limit = -1) override;

private:
    virtual bool savePageBackend(Page *page) override;

    void createDb();
};

}

#endif // SQLENGINE_H
