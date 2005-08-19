#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "classification_task.h"

#include <qmap.h>
#include <qdom.h>
#include <qstring.h>

class FileManager {

public:
    static FileManager* getInstance();
    ~FileManager();

    QString getDirectory(const ClassificationTask &classificationTask);
    QString getFilename(const ClassificationTask &classificationTask);
    QString getFilename(const QString &category);
    QString getCategory(const QString &filename);

private:
    FileManager();

    void readFromXML(const QDomDocument& doc);
    QString getStorageFile();
    QDomDocument toXML() const;
    void saveToXML();

    static FileManager* m_instance;
    QMap<ClassificationTask,QString> m_files;
    QMap<QString, QString>m_categories;
};

#endif // FILEMANAGER_H
