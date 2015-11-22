#ifndef WEBSITE_H
#define WEBSITE_H

#include <QtNetwork/QNetworkAccessManager>

class Website : public QObject
{
    Q_OBJECT
public:
    explicit Website(QObject *parent = 0);
    ~Website();
    void fetch(QString url);
    void fetchLater(QString url);
private:
    QString url;
    QNetworkAccessManager* m_manager;
    int aborted;
    int started;
signals:
    void replyContent(QString, QByteArray);
    void cancel();
    void start();
    void finished();
public slots:
    void replyFinished(QNetworkReply* reply);
    void abortFetch();
    void startFetch();
};

#endif // WEBSITE_H
