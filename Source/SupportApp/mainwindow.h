#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDataStream>
#include <QMainWindow>
#include <QTcpSocket>
#include <QtNetwork>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openConnection();
    void closeConnection();
    void readChunk();
    void sendDatabase();
    void enableConnectButton();
    void sessionOpened();
    void openSelectClass();
    void openSettings();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;
    QNetworkSession *networkSession = nullptr;
    qint32      m_blockSize;
    int counter = 0;
};

#endif // MAINWINDOW_H
