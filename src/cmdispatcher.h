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
    virtual QByteArray list() const Q_DECL_FINAL;

    /**
     * Return true if the dispatchType matches the given path
     */
    virtual DispatchType::MatchType match(Context *ctx, const QString &path, const QStringList &args) const Q_DECL_FINAL;

    /**
     * Returns an uri for an action
     */
    virtual QString uriForAction(Action *action, const QStringList &captures) const Q_DECL_FINAL;

    virtual bool registerAction(Action *action) Q_DECL_FINAL;

    virtual bool inUse() const Q_DECL_FINAL;

private:
    Action *m_pageAction = 0;
    Action *m_postAction = 0;
    Action *m_latestPostsAction = 0;
};

#endif // CMDISPATCHER_H
