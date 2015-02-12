#ifndef CMDISPATCHER_H
#define CMDISPATCHER_H

#include <Cutelyst/DispatchType>

#include "cmengine.h"

using namespace Cutelyst;

class CMDispatcher : public DispatchType, public CMEngine
{
    Q_OBJECT
public:
    explicit CMDispatcher(QObject *parent = 0);
    ~CMDispatcher();

    /**
     * @brief list the registered actions
     * To be implemented by subclasses
     */
    virtual QByteArray list() const;

    /**
     * Return true if the dispatchType matches the given path
     */
    virtual bool match(Context *ctx, const QString &path, const QStringList &args) const;

    /**
     * Returns an uri for an action
     */
    virtual QString uriForAction(Action *action, const QStringList &captures) const;

    virtual bool registerAction(Action *action);

    virtual bool inUse() const;

private:
    Action *m_pageAction = 0;
    Action *m_postAction = 0;
};

#endif // CMDISPATCHER_H
