
#include "classifier_factory.h"

#include "classification_task.h"
#include "filemanager.h"

#include "classifier.h"
#include "naive_bayesian_classifier.h"
#include "dbacl_classifier.h"

#include <kdebug.h>

ClassifierFactory::ClassifierFactory()
{

}

Classifier* ClassifierFactory::produceClassifier( const ClassificationTask &classificationTask )
{
    if( classificationTask.getClassifierId() == "de.berlios.klassify.classifiers.NaiveBayesianClassifier" )
    {
        QString filename = FileManager::getInstance()->getDirectory(classificationTask) + "/database.db";
        kdDebug() << "Factory creates new NaiveBayesianClassifier with filename " << filename << endl;
        NaiveBayesianClassifier* classifier = new NaiveBayesianClassifier(filename);
        classifier->setClassificationTask(classificationTask);
        return classifier;
    }

    if( classificationTask.getClassifierId() == "com.lbreyer.classifiers.dbacl" )
    {
        if( !DbaclClassifier::available() )
        {
            kdWarning() << "dbacl can't be started, is it installed properly? Using dummy classifier instead" << endl;
            Classifier* classifier = new Classifier();
            classifier->setClassificationTask(classificationTask);
            return classifier;
        }
        else
        {
            QString dir = FileManager::getInstance()->getDirectory( classificationTask );
            kdDebug() << "Factory creates new DbaclClassifier with directory " << dir << endl;
            DbaclClassifier* dbacl = new DbaclClassifier();
            dbacl->setClassificationTask(classificationTask);
            return dbacl;
        }
    }

    return new Classifier();
}


QStringList ClassifierFactory::getKnownClassifiers()
{
    QStringList result = QStringList();
    result << "de.berlios.klassify.classifiers.NaiveBayesianClassifier";
    if( DbaclClassifier::available() )
    {
        result << "com.lbreyer.classifiers.dbacl";
    }
    return result;
}
