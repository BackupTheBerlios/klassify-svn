#include "dcop_classifier.h"

#include <kdebug.h>
#include <kapplication.h>
#include <dcopclient.h>

#include <qcstring.h>

DcopClassifier::DcopClassifier(const QString &dcopAddress) : Classifier()
{
    this->dcopAddress = dcopAddress;
    kdDebug() << "DcopClassifier created" << endl;
}

bool DcopClassifier::learn(const QString &category, const QString &)
{
    QByteArray data, reply;
    QCString replyType;
    QDataStream ds(data, IO_WriteOnly);

    ds << category;
    kdDebug() << "Classification of text in " << category << endl;
    const QCString da = this->dcopAddress.utf8();
    kapp->dcopClient()->call(da, "classifier", "classify", data,
                             replyType, reply);

    if (replyType == "QString") {
        QDataStream replyStream(reply, IO_ReadOnly);
        QString result;
        replyStream >> result;
        kdDebug() << "dcop call returned category " << result << endl;
        return true;
    } else {
        kdWarning() << "failure in dcop call" << endl;
        return false;
    }
}


QString DcopClassifier::getId(const QString &dcopAddress) {
    QByteArray reply;
    QCString replyType;

    kapp->dcopClient()->call(dcopAddress.utf8(), "classifier", "id", 0, replyType, reply);

    if (replyType == "QString") {
        QDataStream replyStream(reply, IO_ReadOnly);
        QString result;
        replyStream >> result;
        kdDebug() << "dcop call returned id " << result << endl;
        return result;
    } else {
        kdWarning() << "failure in dcop call asking for classifier id" << endl;
        return 0;
    }
}


bool DcopClassifier::forget(const QString &, const QString &)
{
    // TODO: implementation
    return false;
}


QString DcopClassifier::classify(const QString &)
{
    return 0;
}


void DcopClassifier::store()
{

}


void DcopClassifier::reset()
{

}
