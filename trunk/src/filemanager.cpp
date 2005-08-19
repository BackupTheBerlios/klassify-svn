#include "filemanager.h"

#include <kapplication.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kdebug.h>

#include <qfile.h>
#include <qdir.h>


FileManager* FileManager::m_instance = 0;

FileManager::FileManager() 
{
    m_files = QMap<ClassificationTask,QString>();
    m_categories = QMap<QString, QString>();

    QFile file(getStorageFile());
    if (file.open(IO_ReadOnly))
    {
        QDomDocument doc;
        if (doc.setContent(file.readAll()))
        {
            readFromXML(doc);
        }
        file.close();
    }
    else
    {
        kdWarning() << "Cannot read storage.xml: File not found" << endl;
    }
}

QString FileManager::getStorageFile()
{
    const QString dir = KGlobal::dirs()->saveLocation("config", "klassify/");
    return dir + "/storage.xml";
}

FileManager::~FileManager() 
{
    delete m_instance;
}

FileManager* FileManager::getInstance() 
{
    if( m_instance == 0 ) 
    {
        m_instance = new FileManager();
    }

    return m_instance;
}

QString FileManager::getDirectory(const ClassificationTask &classificationTask)
{
    const QString dir = KGlobal::dirs()->saveLocation("data", "klassify/");

    if(m_files.contains(classificationTask))
    {
        return dir + m_files[classificationTask];
    }

    // FIXME: use portable directory separator
    QString subdir = KApplication::randomString(8);
    QString path = dir + subdir;
    QDir directory(path);

    if(!directory.mkdir(path))
    {
        kdError() << "The directory " << path << " can't be created" << endl;
        return QString::null;
    }
    else
    {
        m_files.insert(classificationTask, subdir);
        saveToXML();
        return path;
    }
}

QString FileManager::getFilename(const ClassificationTask &classificationTask)
{
    // FIXME: use portable directory separator
    return getDirectory( classificationTask ) + "/database.db";
}

QString FileManager::getFilename(const QString &category)
{
    if(m_categories.contains(category))
    {
        return m_categories[category];
    }

    QString file = KApplication::randomString(8);

    kdDebug() << "Assigning new filename " << file << " to category " << category << endl;
    m_categories.insert(category, file);
    saveToXML();
    return file;
}

QString FileManager::getCategory(const QString &filename)
{
    QValueList<QString> categories = m_categories.keys();
    for (QValueList<QString>::ConstIterator it = categories.constBegin(); it != categories.constEnd(); ++it)
    if( m_categories[*it] == filename )
    {
        return *it;
    }

    return QString::null;
}

void FileManager::readFromXML(const QDomDocument& doc)
{
    QDomElement root = doc.documentElement();

    if (root.isNull())
        return;

    QDomNodeList list = root.elementsByTagName(QString::fromLatin1("directory"));

    for (uint i = 0; i < list.length(); ++i)
    {
        QDomElement e = list.item(i).toElement();
        if (!e.isNull())
        {
            if (e.hasAttribute(QString::fromLatin1("classifier")) && e.hasAttribute(QString::fromLatin1("application")) && e.hasAttribute(QString::fromLatin1("path")))
            {
                QString classifierId = e.attribute(QString::fromLatin1("classifier"));
                QString applicationId = e.attribute(QString::fromLatin1("application"));
                QString path = e.attribute(QString::fromLatin1("path"));
                ClassificationTask classificationTask = ClassificationTask(classifierId, applicationId);
                m_files.insert(classificationTask, path);
            }
        }
    }

    list = root.elementsByTagName(QString::fromLatin1("file"));

    for (uint i = 0; i < list.length(); ++i)
    {
        QDomElement e = list.item(i).toElement();
        if (!e.isNull())
        {
            if (e.hasAttribute(QString::fromLatin1("category")) && e.hasAttribute(QString::fromLatin1("path")))
            {
                QString category = e.attribute(QString::fromLatin1("category"));
                QString path = e.attribute(QString::fromLatin1("path"));
                m_categories.insert(category, path);
            }
        }
    }
}

void FileManager::saveToXML()
{
    QFile file(getStorageFile());
    if ( file.open(IO_WriteOnly) )
    {
        QTextStream stream(&file);
        stream.setEncoding(QTextStream::UnicodeUTF8);
        stream << toXML().toString() << "\n";
        file.close();
    }
}

QDomDocument FileManager::toXML() const
{
    QDomDocument doc;
    doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement root = doc.createElement("storage");
    doc.appendChild(root);

    QDomElement directories = doc.createElement("directories");
    root.appendChild(directories);

    QValueList<ClassificationTask> classificationTasks = m_files.keys();
    for (QValueList<ClassificationTask>::ConstIterator it = classificationTasks.constBegin(); it != classificationTasks.constEnd(); ++it)
    {
        QDomElement node = doc.createElement("directory");

        node.setAttribute(QString::fromLatin1("classifier"), (*it).getClassifierId());
        node.setAttribute(QString::fromLatin1("application"), (*it).getApplicationId());
        node.setAttribute(QString::fromLatin1("path"), m_files[*it]);
        directories.appendChild(node);
    }

    QDomElement files = doc.createElement("files");
    root.appendChild(files);

    QValueList<QString> categories = m_categories.keys();
    for (QValueList<QString>::ConstIterator it = categories.constBegin(); it != categories.constEnd(); ++it)
    {
        QDomElement node = doc.createElement("file");

        node.setAttribute(QString::fromLatin1("category"), *it);
        node.setAttribute(QString::fromLatin1("path"), m_categories[*it]);
        files.appendChild(node);
    }

    return doc;
}
