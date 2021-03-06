#ifndef VPN_H
#define VPN_H

#include <QObject>
#include <QString>
#include <QUdpSocket>

class Vpn : public QObject
{
    Q_OBJECT
public:
    explicit Vpn(QObject *parent = 0);
    Vpn(QString str1, QString url);
    ~Vpn();
    void rank();
    void abort();
    void dump();
    QString getDownloadURL();
    QString getFilename();

    static int count;
    bool out;
    int timescore;
signals:
    void enough();
public slots:
    void receiveFinished();
private:
    QString host;
    QString url;
    QString ip;
    QString port;
    QString sid;
    QString hid;
    QUdpSocket sock;


};

#endif // VPN_H
