#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "classification_task.h"

#include <qmap.h>
#include <qdom.h>
#include <qstring.h>
#include <qstringlist.h>

class FileManager {

public:
    static FileManager* getInstance();
    ~FileManager();

    QString getPath();
    QString getDirectory(const ClassificationTask &classificationTask);
    QString getFilename(const QString &path, const QString &category);
    QStringList getCategories(const QString &directory);
    void addCategory(const QString &category, const ClassificationTask &classificationTask);

private:
    FileManager();

    void readFromXML(const QDomDocument& doc);
    QString getStorageFile();
    QDomDocument toXML() const;
    void saveToXML();

    static FileManager* m_instance;
    QMap<ClassificationTask,QString> m_files;
    QMap<QString, QMap<QString, QString> >m_categories;
};

#endif // FILEMANAGER_H
