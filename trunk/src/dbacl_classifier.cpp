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

DbaclClassifier::DbaclClassifier(const QStringList &categories, const QString &filename) : Classifier(categories, filename)
{
    kdDebug() << "DbaclClassifier created" << endl;
}

bool DbaclClassifier::learn(const QString &category, const QString &text)
{
    kdDebug() << "dbacl learns text in " << category << " category" << endl;

    QString dumpFile = getFilename() + "/" + FileManager::getInstance()->getFilename(category) + ".txt";
    QFile out(dumpFile);
    out.open(IO_WriteOnly | IO_Append);
    QTextStream stream(&out);
    stream << text << "\n";
    out.close();

    return true;
}

bool DbaclClassifier::forget(const QString &/*category*/, const QString &/*text*/)
{
    // TODO: this is not true due to dumpFile
    kdWarning() << "dbacl does not support unlearning" << endl;
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

QString DbaclClassifier::classify(const QString &text)
{
    QString result = CATEGORY_UNKNOWN;

    QDir dir(getFilename());
    QStringList categories = dir.entryList();

    // TODO: keep track of known categories elsewhere
    categories = categories.grep( "dbacl" );

    KProcIO *dbacl = new KProcIO();
    dbacl->setWorkingDirectory(getFilename());

    *dbacl << "dbacl";
    *dbacl << "-U";

    for( QStringList::Iterator it = categories.begin(); it != categories.end(); ++it )
    {
        *dbacl << "-c" << *it;
    }

    dbacl->start(KProcess::NotifyOnExit, false);
    if( !dbacl->writeStdin(text) )
    {
        kdWarning() << "Couldn't transmit text to dbacl. Exiting." << endl;
        return result;
    }

    dbacl->closeStdin();
    dbacl->wait(5);
    dbacl->readln(result);

    kdDebug() << "dbacl classified " << text << " as " << result << endl;

    QTextStream stream(result, IO_ReadOnly);
    QString category;
    QString sharp;
    float percent;
    stream >> category;
    stream >> sharp;
    stream >> percent;
    kdDebug() << "Found the category " << category << " in dbacl result (" << percent << ")" << endl;

    if( percent >= 50)
    {
        QString name = category.left(category.length() - 6);
        QString cat = FileManager::getInstance()->getCategory(name);
        if( cat == QString::null )
        {
            kdWarning() << "The filename " << category << " is not a known category" << endl;
            return CATEGORY_UNKNOWN;
        }
        return cat;
    }

    return CATEGORY_REJECTED;
}

void DbaclClassifier::store()
{
    QDir dir(getFilename());
    QStringList categories = dir.entryList();
    QRegExp pattern("txt$");
    categories = categories.grep( pattern );

    for( QStringList::Iterator it = categories.begin(); it != categories.end(); ++it )
    {
        KProcIO *dbacl = new KProcIO();
        dbacl->setWorkingDirectory(getFilename());

        QString dbaclFile = *it;
        dbaclFile.replace("txt", "dbacl");

        *dbacl << "dbacl";
        *dbacl << "-l" << dbaclFile << *it;

        kdDebug() << "Full learning category " << *it << endl;

        dbacl->start(KProcess::NotifyOnExit, false);
        dbacl->wait();
    }
}

void DbaclClassifier::reset()
{
    // TODO: implementation
}
