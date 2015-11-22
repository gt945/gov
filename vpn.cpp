#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QHostAddress>
#include <QDateTime>
#include "vpn.h"


int Vpn::count = 0;

Vpn::Vpn(QObject *parent) : QObject(parent)
{

}

Vpn::Vpn(QString str)
{
    url = str;
    QRegularExpression re(QString("ip=(.*?)&.*udp=(.*?)&.*sid=(.*?)&hid=(.*)"));
    QRegularExpressionMatchIterator matchi = re.globalMatch(url);
    if (matchi.hasNext()) {
        QRegularExpressionMatch match = matchi.next();
        if (match.hasMatch()) {
            ip = match.captured(1);
            port = match.captured(2);
            sid = match.captured(3);
            hid = match.captured(4);
            //qDebug() <<  match.captured(1) << match.captured(2) << match.captured(3) << match.captured(4);
        }
    }
    count = 0;
    out = true;

}

Vpn::~Vpn()
{
    sock.abort();
}

void Vpn::rank()
{
    if(!ip.length() || !port.length()) {
        return;
    }
    QHostAddress addr(ip);
    QByteArray data;

    data.resize(14);
    data[0] = 0x38;

    data[1] = 0x1a;
    data[2] = 0x2b;
    data[3] = 0x3c;
    data[4] = 0x4d;
    data[5] = 0x5e;
    data[6] = 0x6f;
    data[7] = 0x7a;
    data[8] = 0x8b;

    data[9] = 0x00;
    data[10] = 0x00;
    data[11] = 0x00;
    data[12] = 0x00;

    data[13] = 0x00;
    timescore = QDateTime::currentMSecsSinceEpoch();
    sock.bind();
    connect(&sock, SIGNAL(readyRead()), this, SLOT(receiveFinished()));
    sock.writeDatagram(data, addr, port.toShort());

}


void Vpn::receiveFinished()
{

    timescore = QDateTime::currentMSecsSinceEpoch() - timescore;
    qDebug() << ip << timescore;
    sock.close();
    if (count < 10) {
        out = false;
        count++;
    } else if (count == 10) {
        emit enough();
    }
}


