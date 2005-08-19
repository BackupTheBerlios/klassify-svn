#ifndef CLASSIFICATION_TASK_H
#define CLASSIFICATION_TASK_H

#include <qstring.h>

class ClassificationTask 
{
    public:
        ClassificationTask();
        ClassificationTask( const QString &classifierId, const QString &applicationId );
        bool operator<(const ClassificationTask &other) const;
        bool operator==(const ClassificationTask &other);

        QString getClassifierId() const;
        QString getApplicationId() const;

    private:
        QString m_classifierId;
        QString m_applicationId;
};

#endif // CLASSIFICATION_TASK_H
