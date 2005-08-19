#ifndef KLASSIFY_H
#define KLASSIFY_H

#include "classifier.h"
#include "classification_task.h"
#include "classifier_factory.h"

#include <qstringlist.h>
#include <qstring.h>
#include <dcopobject.h>

#include <list>

class klassify :  public DCOPObject
{
	K_DCOP

	public:
		klassify();

		~klassify();

        #define CATEGORY_REJECTED "klassify::rejected"
        #define CATEGORY_UNKNOWN "klassify::unknown"

	k_dcop:
		QStringList getClassifiers();

		void addClassifier(const QString &dcopAddress);

		bool removeClassifier(const QString &dcopClassifierId);

        bool learn(const QString &classifierId, const QString &applicationId, const QString &category, const QString &text, bool storeLater = false);

        bool forget(const QString &classifierId, const QString &applicationId, const QString &category, const QString &text);

        void store(const QString &classifierId, const QString &applicationId);

        QString classify(const QString &classifierId, const QString &applicationId, const QString &text);

		bool exit();

    private:
        QMap<ClassificationTask, Classifier*> m_classifiers;
        ClassifierFactory m_classifierFactory;
};

#endif // KLASSIFY_H
