#include "naive_bayesian_classifier.h"
#include "klassify.h"

#include <kdebug.h>

#include <qstring.h>
#include <qstringlist.h>

#include <limits>
#include <algorithm>

#define MIN_RATIO_REJECTED 0.9
#define MIN_CLASS_OCCURRENCE 0.1

NaiveBayesianClassifier::NaiveBayesianClassifier(const QStringList &categories, const QString &filename) 
    : Classifier(categories, filename)
{
    kdDebug() << "Creating NaiveBayesianClassifer" << endl;

    database = c4_Storage(getFilename(), true);
    database.AutoCommit();

    m_categories = QMap<QString, c4_View>();

//     for ( QStringList::ConstIterator it = categories.constBegin(); it != categories.constEnd(); ++it ) {
//         getCategory(*it);
//     }

    c4_View cats = getDatabase().GetAs("categories[name:S,count:I]");
    c4_Row currentRow;
    c4_StringProp name("name");
    long idx;
    for (idx=0;idx<cats.GetSize();idx++)
    {
        QString cat = (const char*) name(cats[idx]);
        getCategory(cat);
    }
}


QString NaiveBayesianClassifier::getId() {
    return "de.berlios.klassify.classifiers.NaiveBayesianClassifier";
}

QString NaiveBayesianClassifier::classify(const QString &text)
{
    double minProbability = std::numeric_limits<long double>::max();
    double minRatio = 1;
//    double eProb = std::exp(-1 * probability);

    QString result = CATEGORY_UNKNOWN;
    QMap<QString,double> probabilities = getProbabilities(text);
    kdDebug() << "Probability distribution after classification:" << endl;
    QValueList<QString> categories = m_categories.keys();
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

bool NaiveBayesianClassifier::learn(const QString &category, const QString &text)
{
    kdDebug() << "NaiveBayesianClassifer learns text in " << category << " category." << endl;

    QStringList splitted = this->parse(text);
    for ( QStringList::ConstIterator iter = splitted.constBegin(); iter != splitted.constEnd(); ++iter ) {
        changeWordCount(category, *iter, 1);
    }

    changeCount(category, splitted.size());

    return true;
}


bool NaiveBayesianClassifier::forget(const QString &category, const QString &text)
{
    kdDebug() << "NaiveBayesianClassifer forgets text in " << category << " category." << endl;

    QStringList splitted = this->parse(text);
    for ( QStringList::Iterator iter = splitted.begin(); iter != splitted.end(); ++iter ) {
        changeWordCount(category, *iter, -1);
    }

    changeCount(category, -1 * splitted.size());
    return true;
}


void NaiveBayesianClassifier::store()
{
    kdDebug() << "NaiveBayesianClassifier stores things to disk" << endl;
    getDatabase().Commit();
}


void NaiveBayesianClassifier::reset()
{
    // TODO: reset
}


QMap<QString,double> NaiveBayesianClassifier::getProbabilities(const QString &text)
{
    QStringList splitted = this->parse(text);
    QMap<QString,double> probabilities = QMap<QString,double>();

    QValueList<QString> categories = m_categories.keys();
    for( QValueList<QString>::const_iterator it = categories.constBegin(); it != categories.constEnd(); ++it ) {
        double probability = this->getProbability(*it, splitted);
        kdDebug() << "NaiveBayesianClassifiers::" << *it << " = " << probability << endl;
        probabilities.insert(*it, probability);
    }
    
    return probabilities;
}

int NaiveBayesianClassifier::getWordCount(const QString &category, const QString &word) 
{
    long idxsearch=-1;
    c4_Row findrow;
    c4_StringProp match("word");
    match(findrow)=word;

    c4_View cat = getCategory(category);
    idxsearch = getCategory(category).Find(findrow,idxsearch+1);
    if (idxsearch>=0) {
        c4_IntProp count("count");
        return count(cat[idxsearch]);
    } 
    else 
    {
        return 0;
    }
}


c4_Storage& NaiveBayesianClassifier::getDatabase()
{
    return database;
}

c4_View& NaiveBayesianClassifier::getCategory(const QString &category)
{
   if(!m_categories.contains(category))
    {
        kdDebug() << "Creating new category " << category << endl;
        QString view = category + "[word:S,count:I]";
        //QString view = category.left(3) + "[word:S,count:I]";
        kdDebug() << "alive" << endl;

        c4_View cat = getDatabase().GetAs(view);
        kdDebug() << "Creating hash for category " << category << endl;
        // FIXME: category.left(3) is only a hacky heuristic to avoid certain characters
        c4_View viewsec = getDatabase().GetAs(category.left(3) + "[_H:I,_R:I]");
        cat = cat.Hash(viewsec,1);
        kdDebug() << "category " << category << " is ready." << endl;
        m_categories.insert(category, cat);
    }

    return m_categories[category];
}


void NaiveBayesianClassifier::changeWordCount(const QString &category, const QString &word, int change)
{
    long idxsearch=-1;
    c4_Row findrow;
    c4_StringProp match("word");
    
    const char* test = word;
    match(findrow) = test;
    c4_IntProp count("count");

    c4_View cat = getCategory(category);

    idxsearch = cat.Find(findrow,idxsearch+1);

    if (idxsearch>=0) {
        long int zero = 0;
        count(cat[idxsearch]) = std::max(zero, count(cat[idxsearch]) + change);
    } 
    else 
    {
        c4_StringProp newWord("word");
        c4_IntProp newCount("count");
        c4_Row newRow;
        QString temp = QString(word);
        newWord(newRow)=temp;
        newCount(newRow)=1;
        cat.Add(newRow);
    }
}


int NaiveBayesianClassifier::getCount(const QString &category)
{
    long idxsearch=-1;
    c4_Row findrow;
    c4_StringProp match("name");
    match(findrow)=category;
    c4_IntProp count("count");

    c4_View catCount = getDatabase().GetAs("categories[name:S,count:I]");
    idxsearch = catCount.Find(findrow,idxsearch+1);
    if (idxsearch>=0) 
    {
        return count(catCount[idxsearch]);
    }
    else 
    {
        kdWarning() << "Requested getCount() in unknown category " << category << endl;
        return 0;
    }
}

void NaiveBayesianClassifier::changeCount(const QString &category, int change)
{
    long idxsearch=-1;
    c4_Row findrow;
    c4_StringProp match("name");
    match(findrow)=category;
    c4_IntProp count("count");

    c4_View catCount = getDatabase().GetAs("categories[name:S,count:I]");
    idxsearch = catCount.Find(findrow,idxsearch+1);
    if (idxsearch>=0) 
    {
        long zero = 0;
        count(catCount[idxsearch]) = std::max(zero, count(catCount[idxsearch]) + change);
    }
    else 
    {
        c4_StringProp newCat("name");
        c4_IntProp newCount("count");
        c4_Row newRow;
        newCat(newRow)=category;
        newCount(newRow)=std::max(0, change);
        catCount.Add(newRow);
    }
}


QStringList NaiveBayesianClassifier::parse(const QString &text) {
    // TODO: research word boundaries in different languages
    // TODO: filter dots etc.
    return QStringList::split(" ", text);
}


double NaiveBayesianClassifier::getProbability(QString category, const QStringList &text) {
    long totalWordCount = 0;

    // TODO: do caching
    QValueList<QString> categories = m_categories.keys();
    for( QValueList<QString>::const_iterator it = categories.constBegin(); it != categories.constEnd(); ++it ) {
        totalWordCount += getCount(*it);
    }

    double catCount = log(getCount(category));
    double probability = catCount;
    probability -= log(totalWordCount);

    for ( QStringList::ConstIterator iter = text.constBegin(); iter != text.constEnd(); ++iter ) {
        long wordCount = getWordCount(category, *iter);
        if( wordCount != 0) {
            //long wordProbability = wordCount;
            probability -= log(wordCount);
            probability += catCount;
        } else {
            probability -= log(MIN_CLASS_OCCURRENCE);
            probability += catCount;
        }
    }
    return probability;
}
