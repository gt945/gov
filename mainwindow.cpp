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
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    clearVpnList();
    delete ui;
}

void MainWindow::rankVPN(QString str)
{
    for( int i = 0; i < vpnList.count(); ++i )
    {
        connect(vpnList[i], SIGNAL(enough()), this, SLOT(downloadVPN()));
        vpnList[i]->rank();
    }
}

void MainWindow::downloadVPN()
{
    for( int i = 0; i < vpnList.count(); ++i )
    {
        vpnList[i]->rank();
    }
}

void MainWindow::clearVpnList()
{
    for( int i = 0; i < vpnList.count(); ++i ) {
        delete vpnList[i];
    }
    vpnList.clear();
}

void MainWindow::on_Fetch_clicked()
{
    fetched = 0;
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
        qDebug() << url;
        //qDebug() << url << QString(data);
        QRegularExpression re(QString("(do_openvpn.aspx.*?)'"));
        QRegularExpressionMatchIterator matchi = re.globalMatch(data);
        while (matchi.hasNext()) {
            QRegularExpressionMatch match = matchi.next();
            if (match.hasMatch()) {
                vpnList.append(new Vpn(match.captured(1)));
            }

        }
        rankVPN(url);
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
