#ifndef NAIVE_BAYESIAN_CLASSIFIER_H
#define NAIVE_BAYESIAN_CLASSIFIER_H

#include "classifier.h"

#include <qstringlist.h>
class QStringList;

#include <mk4.h>
#include <qmap.h>

class NaiveBayesianClassifier : public Classifier
{
    public:
        NaiveBayesianClassifier(const QStringList &categories, const QString &filename);

        using Classifier::learn;
        bool learn(const QString &category, const QString &text);
        bool forget(const QString &category, const QString &text);
        static QString getId();

        QString classify(const QString &text);

        void store();
        void reset();

    private:
        int getWordCount(const QString &category, const QString &word);
        void changeWordCount(const QString &category, const QString &word, int change);
        c4_Storage& getDatabase();
        c4_View& getCategory(const QString &category);
        int getCount(const QString &category);
        void changeCount(const QString &category, int change);
        QMap<QString,double> getProbabilities(const QString &text);

        QStringList parse(const QString &text);
        double getProbability(QString category, const QStringList &text);

        c4_Storage database;
        QMap<QString, c4_View> m_categories;
};

#endif // NAIVE_BAYESIAN_CLASSIFIER_H
