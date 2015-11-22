#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "website.h"
#include "vpn.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void rankVPN(QString str);

    void clearVpnList();

signals:
    void abortFetch();
    void abortDownload();
private slots:
    void on_Fetch_clicked();
    void getContent(QString url, QByteArray data);
    void getVPN(QString url, QByteArray data);
    void downloadVPN(QString str);
    void on_Quit_clicked();
    void on_Browse_clicked();

private:
    Ui::MainWindow *ui;
    int fetched;
    QList<Vpn *> vpnList;
};

#endif // MAINWINDOW_H
