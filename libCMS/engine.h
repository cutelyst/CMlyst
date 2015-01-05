#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>

namespace CMS {

class Page;
class EnginePrivate;
class Engine : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Engine)
public:
    explicit Engine(QObject *parent = 0);
    virtual ~Engine();

    bool init(const QHash<QString, QString> &settings);

    virtual Page *getPage(const QString &path);

    virtual Page *getPageToEdit(const QString &path) const;
    virtual bool savePage(Page *page);

    /**
     * Returns the available pages,
     * when depth is -1 all pages are listed
     */
    virtual QList<Page *> listPages(int depth = -1);

protected:
    EnginePrivate *d_ptr;
};

}

#endif // ENGINE_H
