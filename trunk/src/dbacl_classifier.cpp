#include "dbacl_classifier.h"
#include "filemanager.h"
#include "klassify.h"

#include <kdebug.h>
#include <kprocio.h>
#include <ktempfile.h>
#include <kstandarddirs.h>
#include <ksavefile.h>

#include <qdir.h>
#include <qtextstream.h>
#include <qregexp.h>

DbaclClassifier::DbaclClassifier() : Classifier()
{
    kdDebug() << "DbaclClassifier created" << endl;
}

bool DbaclClassifier::learn(const QString &category, const QString &text)
{
    kdDebug() << "dbacl learns text in " << category << " category" << endl;

    QString dumpFile = getDumpFile(category);
    QFile out(dumpFile);
    out.open(IO_WriteOnly | IO_Append);
    QTextStream stream(&out);
    stream << text << "\n";
    out.close();

    return true;
}

bool DbaclClassifier::forget(const QString &category, const QString &text)
{
    // FIXME: There is probably an easier way to remove a string from a textfile.
    QString forgetText = text;
    QFile in(getDumpFile(category));
    if(in.open(IO_ReadOnly))
    {
        QTextStream input(&in);
        KSaveFile out(getDumpFile(category));
        QTextStream* output = out.textStream();
        QString line;
        while(!in.atEnd())
        {
            line = input.readLine();
            if(forgetText.contains(line)) 
            {
                forgetText.replace(line, "");
            }
            else
            {
                *output << line << "\n";
            }
        }
        in.close();
        return out.close();
    }

    return false;
}

QString DbaclClassifier::getId()
{
    return "com.lbreyer.classifiers.dbacl";
}

bool DbaclClassifier::available()
{
    KProcIO *dbacl = new KProcIO();
    *dbacl << "dbacl";
    return dbacl->start();
}

QMap<QString,double> DbaclClassifier::getProbabilities(const QString &text)
{
    QMap<QString,double> result = QMap<QString,double>();

    KProcIO *dbacl = new KProcIO();
    dbacl->setWorkingDirectory(getDirectory());

    *dbacl << "dbacl";
    *dbacl << "-U";

    QStringList categories = getCategories();
    for( QStringList::Iterator it = categories.begin(); it != categories.end(); ++it )
    {
        *dbacl << "-c" << getFilename(*it);
    }

    dbacl->start(KProcess::NotifyOnExit, false);
    if( !dbacl->writeStdin(text) )
    {
        kdWarning() << "Couldn't transmit text to dbacl. Exiting." << endl;
        result.clear();
        result.insert(CATEGORY_UNKNOWN, 1);
        return result;
    }

    dbacl->closeStdin();
    dbacl->wait(5);
    QString line;
    dbacl->readln(line);

    kdDebug() << "dbacl classified " << text << " as " << line << endl;

    QTextStream stream(line, IO_ReadOnly);
    QString category;
    QString sharp;
    float percent;
    stream >> category;
    stream >> sharp;
    stream >> percent;
    category = getCategory(category);
    kdDebug() << "Found the category " << category << " in dbacl result (" << percent << ")" << endl;

    if( percent >= 50)
    {
        if( category == QString::null )
        {
            kdWarning() << "The filename " << category << " is not a known category" << endl;
            result.clear();
            result.insert(CATEGORY_UNKNOWN, 1);
        }

        result.insert(category, percent / 100);
        return result;
    }

    result.clear();
    result.insert(CATEGORY_REJECTED, 1);
    return result;
}

void DbaclClassifier::store()
{
    QStringList categories = getCategories();
    for( QStringList::Iterator it = categories.begin(); it != categories.end(); ++it )
    {
        KProcIO *dbacl = new KProcIO();
        dbacl->setWorkingDirectory(getDirectory());

        *dbacl << "dbacl";
        QString dumpFile = getDumpFile(*it);
        *dbacl << "-l" << getFilename(*it) << dumpFile;

        kdDebug() << "Full learning category " << *it << endl;

        dbacl->start(KProcess::NotifyOnExit, false);
        dbacl->wait();
    }
}

void DbaclClassifier::reset()
{
    // TODO: implementation
}


QString DbaclClassifier::getDumpFile(const QString &category)
{
    return getDirectory() + "/" + getFilename(category) + ".txt";
}
