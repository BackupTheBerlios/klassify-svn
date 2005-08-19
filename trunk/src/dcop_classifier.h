#ifndef DCOP_CLASSIFIER_H
#define DCOP_CLASSIFIER_H

#include "classifier.h"

#include <qstring.h>

class DcopClassifier : public Classifier {
    public:
        DcopClassifier(const QString &dcopAddress, const QString &filename);

        static QString getId(const QString &dcopAddress);

        bool learn(const QString &category, const QString &text);
        bool forget(const QString &category, const QString &text);

        QString classify(const QString &text);

        void store();
        void reset();

    private:
        QString dcopAddress;
};

#endif // DCOP_CLASSIFIER_H
