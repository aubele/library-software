#include "appwidget.h"
#include "ui_appwidget.h"

#include <QMessageBox>

//========================================
// Implementations of AppWidget
//========================================

//----------------------------------------
// Constructor
//----------------------------------------
AppWidget::AppWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppWidget),
    clientConnection(new QTcpSocket(this)),
    counter(0)
{
    ui->setupUi(this);
    connect(ui->closeBtn, SIGNAL(clicked()), this, SLOT(endConnection()));
    connect(ui->closeBtn, SIGNAL(clicked()), this, SIGNAL(closewidget()));
    connect(ui->startBtn, SIGNAL(clicked()), this, SLOT(startConnection()));
    connect(ui->sendBtn, SIGNAL(clicked()), this, SLOT(sendDatabase()));
    connect(ui->endBtn, SIGNAL(clicked()), this, SLOT(endConnection()));
    connect(ui->receiveBtn, SIGNAL(clicked()), this, SLOT(receiveDatabase()));

    ui->sendBtn->setEnabled(false);
    ui->receiveBtn->setEnabled(false);
}

//----------------------------------------
// Destructor
//----------------------------------------
AppWidget::~AppWidget()
{
    delete ui;
}

void AppWidget::startConnection()
{
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
    {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered)
        {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        ui->statusServer->setText("Öffnet Netzwerkverbindung.");
        ui->statusDb->setText("");
        ui->statusGetDb->setText("");
        networkSession->open();
    }
    else
    {
        sessionOpened();
    }
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(activateConnection()));
}

void AppWidget::activateConnection()
{
    ui->sendBtn->setEnabled(true);
    ui->receiveBtn->setEnabled(true);
}

void AppWidget::endConnection()
{
    if(clientConnection && clientConnection->isValid())
    {
        clientConnection->abort();
        clientConnection->disconnectFromHost();
    }

    tcpServer->close();
    ui->sendBtn->setEnabled(false);
    ui->receiveBtn->setEnabled(false);
    ui->statusServer->setText("Status - Server");
    ui->statusDb->setText("");
    ui->statusGetDb->setText("");
}

void AppWidget::sendDatabase()
{
    ui->sendBtn->setEnabled(false);
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_10);

    clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, &QAbstractSocket::disconnected,
                clientConnection, &QObject::deleteLater);

    // TODO correct path?
    QFile file("./database.db");
    if (!file.open(QIODevice::ReadOnly))
        return;
    QByteArray q = file.readAll();
    block.append(q);
    file.close();

    out.device()->seek(0);

    qint64 x = 0;
    while (x < block.size()) {
        qint64 y = clientConnection->write(block);
        x += y;
    }
    clientConnection->disconnectFromHost();
    ui->statusDb->setText("Datenbank erfolgreich verschickt!");
}

void AppWidget::sessionOpened()
{
    // Save the used configuration
    if (networkSession)
    {
        QNetworkConfiguration config = networkSession->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice)
            id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        else
            id = config.identifier();

        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
    }

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen())
    {
        QMessageBox::critical(this, "Server", "Kann Server nicht starten: " + tcpServer->errorString());
        close();
        return;
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    ui->statusServer->setText(tr("Server läuft unter\n\nIP: %1\nPort: %2\n\nWarte auf Client.")
                         .arg(ipAddress).arg(tcpServer->serverPort()));
    ui->statusDb->setText("");
    ui->statusGetDb->setText("");
}

void AppWidget::readChunk()
{
    if(counter == 0)
    {
        QFile dbfile("./database.db");
        if (dbfile.exists())
        {
            dbfile.remove();
        }
    }
    QDataStream in;
    in.setVersion(QDataStream::Qt_5_10);
    in.setDevice(clientConnection);

    QByteArray line = clientConnection->readAll();
    // TODO correct path?
    QFile newDbFile("./database.db");
    if(!newDbFile.open(QIODevice::WriteOnly | QIODevice::Append))
        return;

    newDbFile.write(line);
    newDbFile.close();
    counter+= 1;

    ui->statusGetDb->setText("Datenbank erfolgreich empfangen!");
}

void AppWidget::receiveDatabase()
{
    ui->statusGetDb->setText("Warte auf Übertragung der Datenbank.");
    ui->receiveBtn->setEnabled(false);
    clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, &QIODevice::readyRead, this, &AppWidget::readChunk);
}
