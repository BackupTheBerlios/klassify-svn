#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <qstring.h>
#include <qstringlist.h>

class Classifier {
    public:
        Classifier();
        virtual ~Classifier() {};
        Classifier(const QStringList &categories, const QString &filename);

        static QString getId();
        QStringList getCategories();
        QString getFilename();

        virtual bool learn(const QString &category, const QString &text);
        virtual bool forget(const QString &category, const QString &text);

        virtual QString classify(const QString &text);

        virtual void store();
        virtual void reset();

        bool operator==(Classifier other);

    protected:
        QStringList categories;

    private:
        QString filename;
};

#endif // CLASSIFIER_H
