#ifndef DBACL_CLASSIFIER_H
#define DBACL_CLASSIFIER_H

#include "classifier.h"

class DbaclClassifier : public Classifier
{
    public:
        DbaclClassifier(const QStringList &categories, const QString &filename);

        using Classifier::learn;
        bool learn(const QString &category, const QString &text);
        bool forget(const QString &category, const QString &text);
        static QString getId();
        static bool available();

        QString classify(const QString &text);

        void store();
        void reset();
};

#endif // DBACL_CLASSIFIER_H
