#include "classifier.h"
#include "filemanager.h"

#include <kdebug.h>


Classifier::Classifier() {}

bool Classifier::operator==(Classifier other)
{
    return getId() == other.getId();
}


QString Classifier::getId()
{
    return "de.berlios.klassify.classifiers.DummyClassifier";
}


QStringList Classifier::getCategories()
{
    QString directory = FileManager::getInstance()->getDirectory(m_classificationTask);
    return FileManager::getInstance()->getCategories(directory);
}


QString Classifier::getFilename(const QString &category)
{
    QString directory = FileManager::getInstance()->getDirectory(m_classificationTask);
    return FileManager::getInstance()->getFilename(directory, category);
}


bool Classifier::learn(const QString &, const QString &)
{
    return false;
}


bool Classifier::forget(const QString &, const QString &)
{
    return false;
}


QMap<QString, double> Classifier::getProbabilities(const QString &)
{
    return QMap<QString, double>();
}

void Classifier::store()
{
}

void Classifier::reset()
{

}


QString Classifier::getDirectory()
{
    return FileManager::getInstance()->getPath() + "/" + FileManager::getInstance()->getDirectory(m_classificationTask);
}


void Classifier::setClassificationTask(ClassificationTask classificationTask)
{
    m_classificationTask = classificationTask;
}


QString Classifier::getCategory(const QString &filename)
{
    QStringList categories = getCategories();
    for(QStringList::ConstIterator it = categories.constBegin(); it != categories.constEnd(); ++it)
    {
        if(getFilename(*it) == filename)
        {
            return *it;
        }
    }

    return QString::null;
}

