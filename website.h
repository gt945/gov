#ifndef WEBSITE_H
#define WEBSITE_H

#include <QFile>
#include <QtNetwork/QNetworkAccessManager>

class Website : public QObject
{
    Q_OBJECT
public:
    explicit Website(QObject *parent = 0);
    ~Website();
    void fetch(QString url, QString filename = NULL);
    void fetchLater(QString url, QString filename = NULL);
private:
    QString url;
    QNetworkAccessManager* m_manager;
    int aborted;
    int started;
    QFile *file;
signals:
    void replyContent(QString, QByteArray);
    void cancel();
    void start();
    void finished();
public slots:
    void replyFinished(QNetworkReply* reply);
    void replyReadyRead();
    void abortFetch();
    void startFetch();
};

#endif // WEBSITE_H
