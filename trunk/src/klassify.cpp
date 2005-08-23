#include "klassify.h"

#include "dcop_classifier.h"
#include "naive_bayesian_classifier.h"
#include "filemanager.h"

#include <kdebug.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <kglobal.h>

#include <qstringlist.h>

#define MIN_RATIO_REJECTED 0.9

klassify::klassify() : DCOPObject("serviceInterface")
{
    kdDebug() << "Starting klassify... " << endl;
    m_classifierFactory = ClassifierFactory();
    m_classifiers = QMap<ClassificationTask, Classifier*>();
}

klassify::~klassify()
{
    kdDebug() << "Stopping klassify... " << endl;
    QMap<ClassificationTask, Classifier*>::Iterator it;
    for(it=m_classifiers.begin(); it!=m_classifiers.end(); it++)
    {
        delete *it;
    }
}

QStringList klassify::getClassifiers()
{
    return m_classifierFactory.getKnownClassifiers();
}

void klassify::addClassifier(const QString &dcopAddress)
{
    kdDebug() << "Adding " << dcopAddress << " to the list of classifiers" << endl;
    const QString dir = KGlobal::dirs()->saveLocation("data", "klassify/");
    QString id = DcopClassifier::getId(dcopAddress);
    const QString filename = dir + id;
//    DcopClassifier* classifier = new DcopClassifier(dcopAddress, filename);
    if( id != 0 )
    {
        // TODO: dcopClassifier should handle storage by itself
        //m_Classifiers.insert(m_Classifiers.end(), classifier);
    } 
    else 
    {
        kdWarning() << "Classifier " << dcopAddress << " is not accessible through dcop" << endl;
    }
}

bool klassify::removeClassifier(const QString &/*dcopClassifier*/)
{
/*    std::list<Classifier*>::iterator it;
    for(it=m_Classifiers.begin(); it!=m_Classifiers.end(); it++)
    {
        if((*it)->getId() == dcopClassifier)
        {
            m_Classifiers.remove(*it);
            // assertion: classifier listed only once in list
            return true;
        }
    }*/
    return false;
}

bool klassify::exit()
{
    kapp->quit();
    return true;
}


bool klassify::learn(const QString &classifierId, const QString &applicationId, const QString &category, const QString &text, bool storeLater)
{
    ClassificationTask classificationTask = ClassificationTask(classifierId, applicationId);
    Classifier* classifier = 0;
    if( !m_classifiers.contains(classificationTask) )
    {
        classifier = m_classifierFactory.produceClassifier(classificationTask);
        m_classifiers.insert(classificationTask, classifier);
    }
    else
    {
        classifier = m_classifiers[classificationTask];
    }

    kdDebug() << "Sending learn call to classifier " << classifierId << endl;
    FileManager::getInstance()->addCategory(category, classificationTask);
    bool ret = classifier->learn(category, text);
    if(!storeLater && ret) {
        classifier->store();
    }
    return ret;
}



bool klassify::forget(const QString &classifierId, const QString &applicationId, const QString &category, const QString &text)
{
    ClassificationTask classificationTask = ClassificationTask(classifierId, applicationId);
    Classifier* classifier = 0;
    if( !m_classifiers.contains(classificationTask) )
    {
        kdWarning() << "classifier " << classifierId << " does not exist" << endl;
        return false;
    }

    classifier = m_classifiers[classificationTask];

    kdDebug() << "Sending forget call to classifier " << classifierId << endl;
    return classifier->forget(category, text);
}



QString klassify::classify(const QString &classifierId, const QString &applicationId, const QString &text)
{
    double minProbability = std::numeric_limits<long double>::max();
    double minRatio = 1;

    QString result = CATEGORY_UNKNOWN;
    QMap<QString,double> probabilities = getProbabilities(classifierId, applicationId, text);
    kdDebug() << "Probability distribution after classification:" << endl;
    QValueList<QString> categories = probabilities.keys();
    for( QValueList<QString>::const_iterator it = categories.constBegin(); it != categories.constEnd(); ++it )
    {
        kdDebug() << *it << " = " << probabilities[*it] << endl;
        if(probabilities[*it] < minProbability)
        {
            minRatio = probabilities[*it] / minProbability;
            kdDebug() << "setting ratio to " << minRatio << endl;
            result = *it;
            minProbability = probabilities[*it];
        } else if(minProbability / probabilities[*it] > minRatio)
        {
            minRatio = minProbability / probabilities[*it];
            kdDebug() << "setting ratio to " << minRatio << endl;
        }
    }
    
    // TODO: this is only a poor heuristic
    QString end = minRatio > MIN_RATIO_REJECTED ? CATEGORY_REJECTED : result;
    return end;
}


QStringList klassify::classifyNBest(const QString &classifierId, const QString &applicationId, const QString &text)
{
    // TODO: implementation
    return QStringList();
}

QMap<QString,double> klassify::getProbabilities(const QString &classifierId, const QString &applicationId, const QString &text)
{
    ClassificationTask classificationTask = ClassificationTask(classifierId, applicationId);
    Classifier* classifier = 0;
    if( !m_classifiers.contains(classificationTask) )
    {
        kdWarning() << "classifier " << classifierId << " not yet created for application " << applicationId << endl;
        QMap<QString,double> unknown = QMap<QString,double>();
        unknown.insert(CATEGORY_UNKNOWN, 1);
        return unknown;
    }

    classifier = m_classifiers[classificationTask];

    kdDebug() << "Sending classification call to classifier " << classifierId << endl;
    return classifier->getProbabilities(text);
}


void klassify::store(const QString &classifierId, const QString &applicationId)
{
    ClassificationTask classificationTask = ClassificationTask(classifierId, applicationId);
    Classifier* classifier = 0;
    if( !m_classifiers.contains(classificationTask) )
    {
        kdWarning() << "classifier " << classifierId << " does not exist" << endl;
        return;
    }

    classifier = m_classifiers[classificationTask];

    kdDebug() << "Sending store call to " << classifierId << endl;
    classifier->store();
}
