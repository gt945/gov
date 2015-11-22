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
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

Website::~Website()
{
    //qDebug() <<  "~Website";
    m_manager->deleteLater();
}

void Website::fetch(QString url)
{
    this->url = url;
    connect(this, SIGNAL(start()), this, SLOT(startFetch()));
    emit start();
}

void Website::fetchLater(QString url)
{
    this->url = url;
    connect(this, SIGNAL(start()), this, SLOT(startFetch()));
}


void Website::replyFinished(QNetworkReply *reply)
{
    QByteArray data;

    reply->deleteLater();

    if(reply->error() == QNetworkReply::NoError) {
        int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (v >= 200 && v < 300) {
            data = reply->readAll();
            emit replyContent(reply->url().toString(), data);
            this->deleteLater();
            emit finished();

        } else if (v >= 300 && v < 400) {
            started = 0;
            QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            newUrl = reply->url().resolved(newUrl);
            fetch(newUrl.toString());

            return;
        }
    } else {
        if (!aborted) {
            emit finished();
        }

        this->deleteLater();
    }
}

void Website::abortFetch()
{
//    qDebug() <<  "Abort:" << url;
    aborted = 1;
    emit cancel();
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
//        qDebug() <<  "Fetch:" << url;
    } else {
        emit finished();
    }

}



