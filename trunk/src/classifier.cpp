#include "classifier.h"

#include <kdebug.h>

Classifier::Classifier()
{

}

Classifier::Classifier(const QStringList &categories, const QString &filename)
{
    this->categories = categories;
    this->filename = filename;
}

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
    return categories;
}


QString Classifier::getFilename()
{
    return filename;
}


bool Classifier::learn(const QString &, const QString &)
{
    return false;
}


bool Classifier::forget(const QString &, const QString &)
{
    return false;
}


QString Classifier::classify(const QString &)
{
    return 0;
}

void Classifier::store()
{
}

void Classifier::reset()
{

}
