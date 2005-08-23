#ifndef DBACL_CLASSIFIER_H
#define DBACL_CLASSIFIER_H

#include "classifier.h"

class DbaclClassifier : public Classifier
{
    public:
        DbaclClassifier();

        using Classifier::learn;
        bool learn(const QString &category, const QString &text);
        bool forget(const QString &category, const QString &text);
        static QString getId();
        static bool available();

        QMap<QString,double> getProbabilities(const QString &text);

        void store();
        void reset();

    private:
        QString getDumpFile(const QString &category);
};

#endif // DBACL_CLASSIFIER_H
