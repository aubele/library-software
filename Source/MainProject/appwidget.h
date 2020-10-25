#ifndef APPWIDGET_H
#define APPWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QtNetwork>

namespace Ui {
class AppWidget;
}

class AppWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AppWidget(QWidget *parent = nullptr);
    ~AppWidget();

private:
    Ui::AppWidget *ui;

    QTcpServer *tcpServer = nullptr;
    QNetworkSession *networkSession = nullptr;
    QTcpSocket *clientConnection = nullptr;
    int counter = 0;

private slots:
    void startConnection();
    void endConnection();
    void sendDatabase();
    void sessionOpened();
    void activateConnection();
    void readChunk();
    void receiveDatabase();

signals:
    void closewidget();
};


#endif // APPWIDGET_H
