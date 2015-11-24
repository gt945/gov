#include "website.h"

#include <QUrl>
#include <QDebug>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

Website::Website(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    aborted = 0;
    started = 0;
    file = NULL;
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

Website::~Website()
{
    //qDebug() <<  "~Website";
    m_manager->deleteLater();
}

void Website::fetch(QString url, QString filename)
{
    fetchLater(url, filename);
    emit start();
}

void Website::fetchLater(QString url, QString filename)
{
    this->url = url;
    if (!filename.isNull()) {
        file = new QFile(filename);
        if (!file->open(QIODevice::WriteOnly)) {
            aborted = 1;
            delete file;
            file = NULL;
        }
    }
    connect(this, SIGNAL(start()), this, SLOT(startFetch()));
}


void Website::replyFinished(QNetworkReply *reply)
{
    QByteArray data;

    reply->deleteLater();
    if (file) {
        file->flush();
        file->close();
    }
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        //if (!aborted) {
            emit finished();
        //}
        if (file) {
            file->remove();
        }
        this->deleteLater();
    } else if (!redirectionTarget.isNull()){
        started = 0;
        QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        newUrl = reply->url().resolved(newUrl);
        if (file) {
            file->open(QIODevice::WriteOnly);
            file->resize(0);
        }
        fetch(newUrl.toString());

        return;
    } else {
        if (!file) {
            data = reply->readAll();
            emit replyContent(reply->url().toString(), data);
        }
        qDebug() << __LINE__ << "finished";
        emit finished();
        this->deleteLater();
    }
    if (file) {
        delete file;
        file = NULL;
    }

}

void Website::replyReadyRead()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    if (file) {
        file->write(reply->readAll());
    }
}

void Website::abortFetch()
{
    qDebug() <<  "Abort:" << url;
    aborted = 1;
    emit cancel();
    if (file) {
        file->remove();
    }
    this->deleteLater();
}

void Website::startFetch()
{
    if (!aborted && !started) {
        started = 1;
        QNetworkRequest request(url);
        request.setRawHeader( "User-Agent", "Mozilla/5.0 (X11; U; Linux i686 (x86_64);"
                               "en-US; rv:1.9.0.1) Gecko/2008070206 Firefox/3.0.1");
        request.setRawHeader( "Accept-Charset", "win1251,utf-8;q=0.7,*;q=0.7");
        request.setRawHeader( "Accept-Language", "en-US");
        request.setRawHeader( "Charset", "utf-8" );
        QNetworkReply *reply = m_manager->get(request);
        connect(this, SIGNAL(cancel()), reply, SLOT(abort()));
        connect(reply, SIGNAL(readyRead()), this, SLOT(replyReadyRead()));

        qDebug() <<  "Fetch:" << url;
    } else {
        qDebug() <<  "Abort Fetch:" << url;
        emit finished();
    }

}



