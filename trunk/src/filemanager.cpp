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
    QMap<QString,QString> tmp = QMap<QString,QString>();
    m_categories = QMap<QString, QMap<QString,QString> >();

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

QString FileManager::getPath()
{
    return KGlobal::dirs()->saveLocation("data", "klassify/");
}

QString FileManager::getDirectory(const ClassificationTask &classificationTask)
{
    if(m_files.contains(classificationTask))
    {
        return m_files[classificationTask];
    }

    // FIXME: use portable directory separator
    QString subdir = KApplication::randomString(8);
    QString path = getPath() + "/" + subdir;
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
        return subdir;
    }
}

QString FileManager::getFilename(const QString &path, const QString &category)
{
    // TODO: search for value, not key!
//     if(m_files.find(path) != 0)
//     {
//         kdWarning() << "Filename for unknown path " << path << " requested." << endl;
//         return QString::null;
//     }

    // TODO: assert m_categories contains path

    if(!m_categories.contains(path))
    {
        m_categories.insert(path, QMap<QString, QString>());
    }

    if(m_categories[path].contains(category))
    {
        return m_categories[path][category];
    }

    QString file = KApplication::randomString(8);
    kdDebug() << "Assigning new filename " << file << " to category " << category << " (path: " << path << endl;
    m_categories[path].insert(category, file);
    saveToXML();
    return file;
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
            if (e.hasAttribute(QString::fromLatin1("classifier")) && e.hasAttribute(QString::fromLatin1("application")) && e.hasAttribute(QString::fromLatin1("name")))
            {
                QString classifierId = e.attribute(QString::fromLatin1("classifier"));
                QString applicationId = e.attribute(QString::fromLatin1("application"));
                QString path = e.attribute(QString::fromLatin1("name"));
                ClassificationTask classificationTask = ClassificationTask(classifierId, applicationId);
                m_files.insert(classificationTask, path);

                QMap<QString, QString> categoriesMap = QMap<QString, QString>();
                QDomNodeList categories = e.elementsByTagName(QString::fromLatin1("file"));
                for( uint j = 0; j < categories.length(); ++j )
                {
                    QDomElement c = categories.item(j).toElement();
                    if(!c.isNull())
                    {
                        if(c.hasAttribute(QString::fromLatin1("name")) && c.hasAttribute(QString::fromLatin1("category")))
                        {
                            QString categoryName = c.attribute(QString::fromLatin1("category" ));
                            QString categoryPath = c.attribute(QString::fromLatin1("name"));
                            categoriesMap.insert(categoryName, categoryPath);
                        }
                    }
                }
                m_categories.insert(path, categoriesMap);
            }
        }
    }
    kdDebug() << "restored " << m_files.size() << " directories" << endl;
}

//     list = root.elementsByTagName(QString::fromLatin1("file"));
// 
//     for (uint i = 0; i < list.length(); ++i)
//     {
//         QDomElement e = list.item(i).toElement();
//         if (!e.isNull())
//         {
//             if (e.hasAttribute(QString::fromLatin1("category")) && e.hasAttribute(QString::fromLatin1("path")))
//             {
//                 QString category = e.attribute(QString::fromLatin1("category"));
//                 QString path = e.attribute(QString::fromLatin1("path"));
//                 m_categories.insert(category, path);
//             }
//         }
//     }
// }

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

    QValueList<ClassificationTask> classificationTasks = m_files.keys();
    for (QValueList<ClassificationTask>::ConstIterator it = classificationTasks.constBegin(); it != classificationTasks.constEnd(); ++it)
    {
        QDomElement node = doc.createElement("directory");

        node.setAttribute(QString::fromLatin1("classifier"), (*it).getClassifierId());
        node.setAttribute(QString::fromLatin1("application"), (*it).getApplicationId());
        QString directory = m_files[*it];
        node.setAttribute(QString::fromLatin1("name"), directory);

        if(!m_categories.contains(directory))
        {
            kdDebug() << "Strange, category " << directory << " is empty" << endl;
        }
        else
        {
            kdDebug() << "There are " << m_categories[directory].size() << " items in it" << endl;
        }
        QValueList<QString> categories = m_categories[directory].keys();
        for (QValueList<QString>::ConstIterator iter = categories.constBegin(); iter != categories.constEnd(); ++iter)
        {
            QDomElement file = doc.createElement("file");
    
            file.setAttribute(QString::fromLatin1("category"), *iter);
            file.setAttribute(QString::fromLatin1("name"), m_categories[directory][*iter]);
            node.appendChild(file);

            kdDebug() << "foo, alife" << endl;
        }

            kdDebug() << "bar, alife" << endl;
        root.appendChild(node);
    }

    return doc;
}


QStringList FileManager::getCategories(const QString &directory)
{
    QMap<QString, QString> dirMapping = m_categories[directory];
    return dirMapping.keys();
}


void FileManager::addCategory(const QString &category, const ClassificationTask &classificationTask)
{
    kdDebug() << "addCategory called for " << classificationTask.getClassifierId() << ", " << classificationTask.getApplicationId() << " and category " << category << endl;
    QString directory = this->getDirectory(classificationTask);
    this->getFilename(directory, category);
}
