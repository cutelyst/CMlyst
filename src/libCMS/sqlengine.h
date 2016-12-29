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

    virtual Page *getPage(const QString &path, QObject *parent) override;

    virtual QVariantHash getPage(const QString &path) override;

    /**
     * Returns the available pages,
     * when depth is -1 all pages are listed
     */
    virtual QList<Page *> listPages(QObject *parent,
                                    Filters filters = NoFilter,
                                    SortFlags sort = SortFlags(Date | Reversed),
                                    int depth = -1,
                                    int limit = -1) override;

    virtual QHash<QString, QString> settings();

    virtual QString settingsValue(const QString &key, const QString &defaultValue = QString()) const override;
    virtual bool setSettingsValue(const QString &key, const QString &value) override;

    virtual QList<Menu *> menus() override;

    virtual QHash<QString, Menu *> menuLocations() override;

    virtual bool settingsIsWritable() const override;

private:
    virtual bool savePageBackend(Page *page) override;

    void loadMenus();
    void createDb();

    QList<CMS::Menu *> m_menus;
    QHash<QString, CMS::Menu *> m_menuLocations;
};

}

#endif // SQLENGINE_H
