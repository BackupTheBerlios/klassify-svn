#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "classification_task.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>

class Classifier {
    public:
        Classifier();
        virtual ~Classifier() {};

        static QString getId();
        QStringList getCategories();
        void setClassificationTask(ClassificationTask classificationTask);
        QString getDirectory();
        QString getFilename(const QString &category);
        QString getCategory(const QString &filename);

        virtual bool learn(const QString &category, const QString &text);
        virtual bool forget(const QString &category, const QString &text);

        virtual QMap<QString,double> getProbabilities(const QString &text);

        virtual void store();
        virtual void reset();

        bool operator==(Classifier other);

    private:
        ClassificationTask m_classificationTask;
};

#endif // CLASSIFIER_H
