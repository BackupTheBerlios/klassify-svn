#ifndef CLASSIFIER_FACTORY_H
#define CLASSIFIER_FACTORY_H

#include "classifier.h"

#include <qstringlist.h>

class ClassificationTask;

class ClassifierFactory
{
    public:
        ClassifierFactory();

        Classifier* produceClassifier( const ClassificationTask &classificationTask );
        QStringList getKnownClassifiers();
};

#endif // CLASSIFIER_FACTORY_H
