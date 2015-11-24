#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QUrl>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    fetched = 0;
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    clearVpnList();
    delete ui;
}

void MainWindow::rankVPN()
{
    for( int i = 0; i < vpnList.count(); ++i )
    {
        connect(vpnList[i], SIGNAL(enough()), this, SLOT(downloadVPN()));
        vpnList[i]->rank();
    }
}

void MainWindow::downloadVPN()
{
    Website *last = NULL;
    emit abortFetch();
    for( int i = 0; i < vpnList.count(); ++i )
    {
        if (vpnList[i]->out == false) {
               //qDebug() << vpnList[i]->timescore;
            Website *w = new Website;
            QString filename = QString("%1/%2").arg(this->ui->filePath->text(), vpnList[i]->getFilename());
            w->fetchLater(vpnList[i]->getDownloadURL(), filename);
            if (last) {
                connect(w, SIGNAL(finished()), last, SLOT(startFetch()));
            } else {
                connect(w, SIGNAL(finished()), this, SLOT(allDone()));
            }
            last = w;
            connect(this, SIGNAL(abortFetch()), w, SLOT(abortFetch()));
        }
    }

    last->startFetch();
}

void MainWindow::allDone()
{
    qDebug() << "all done";
    clearVpnList();
}

void MainWindow::clearVpnList()
{
    for( int i = 0; i < vpnList.count(); ++i ) {
        delete vpnList[i];
    }
    vpnList.clear();
    qDebug() << "clearVpnList";
    fetched = 0;
}

void MainWindow::on_Fetch_clicked()
{
    emit abortFetch();
    QString text = ui->UrlList->toPlainText();
    QRegularExpression re(QString("http://.*/"));
    QRegularExpressionMatchIterator matchi = re.globalMatch(text);

    while (matchi.hasNext()) {
        QRegularExpressionMatch match = matchi.next();
        if (match.hasMatch()) {
            Website *w = new Website;
            w->fetch(match.captured(0));
            connect(w, SIGNAL(replyContent(QString, QByteArray)), this, SLOT(getContent(QString, QByteArray)));
            connect(this, SIGNAL(abortFetch()), w, SLOT(abortFetch()));

        }
    }
}

void MainWindow::getContent(QString url, QByteArray data)
{
    if (!fetched) {
        fetched = 1;
        emit abortFetch();
        qDebug() << __LINE__ << url;
        //qDebug() << url << QString(data);
        QRegularExpression re(QString("(do_openvpn.aspx.*?)'"));
        QRegularExpressionMatchIterator matchi = re.globalMatch(data);
        while (matchi.hasNext()) {
            QRegularExpressionMatch match = matchi.next();
            if (match.hasMatch()) {
                vpnList.append(new Vpn(url, match.captured(1)));
            }

        }
        rankVPN();
    }

}

void MainWindow::getVPN(QString url, QByteArray data)
{

    qDebug() << url;
}

void MainWindow::on_Quit_clicked()
{
    emit abortFetch();
    emit abortDownload();
    this->close();
}

void MainWindow::on_Browse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    this->ui->filePath->setText(dir);

}
