#include "classification_task.h"

ClassificationTask::ClassificationTask()
{
    m_classifierId = QString::null;
    m_applicationId = QString::null;
}

ClassificationTask::ClassificationTask( const QString &classifierId, const QString &applicationId )
{
    m_classifierId = classifierId;
    m_applicationId = applicationId;
}

bool ClassificationTask::operator<(const ClassificationTask &other) const
{
    return getClassifierId() < other.getClassifierId() || getApplicationId() < other.getApplicationId();
}

bool ClassificationTask::operator==(const ClassificationTask &other)
{
    return getClassifierId() == other.getClassifierId() && getApplicationId() == other.getApplicationId();
}

QString ClassificationTask::getClassifierId() const
{
    return m_classifierId;
}

QString ClassificationTask::getApplicationId() const
{
    return m_applicationId;
}
