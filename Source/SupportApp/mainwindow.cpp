#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "selectclassdialog.h"


//============================
// Implementations of LendModel
//============================
//----------------------------
// Constructor
//----------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_blockSize(0),
    tcpSocket(new QTcpSocket(this)),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    ui->pushButton_connect->setEnabled(false);
    ui->pushButton_sendDB->setEnabled(false);
    ui->pushButton_disconnect->setEnabled(false);
    connect(ui->pushButton_connect, &QPushButton::clicked, this, &MainWindow::openConnection);
    connect(ui->pushButton_disconnect, &QPushButton::clicked, this, &MainWindow::closeConnection);
    connect(ui->pushButton_sendDB, &QPushButton::clicked, this, &MainWindow::sendDatabase);
    connect(ui->pushButton_selectClass, &QPushButton::clicked, this, &MainWindow::openSelectClass);
    connect(ui->pushButton_Settings, &QPushButton::clicked, this, &MainWindow::openSettings);
//    connect(ui->lineEdit_ipadress, &QLineEdit::textEdited, this, &MainWindow::enableConnectButton);
//    connect(ui->lineEdit_port, &QLineEdit::textEdited, this, &MainWindow::enableConnectButton);
    connect(tcpSocket, &QIODevice::readyRead, this, &MainWindow::readChunk);

    //remove FocusPolicy
    ui->pushButton_connect->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_disconnect->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_selectClass->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_sendDB->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_Settings->setFocusPolicy(Qt::NoFocus);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, &QNetworkSession::opened, this, &MainWindow::sessionOpened);

        ui->pushButton_connect->setEnabled(false);
        ui->label_conclusion->setText(tr("Opening network session."));
        networkSession->open();
    }
}

//----------------------------
// Destructor
//----------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

//----------------------------
// enableConnectButton(): enables the Connect Button if the lineEdit for the IP address and the port are filled
//----------------------------
void MainWindow::enableConnectButton()
{
    ui->pushButton_connect->setEnabled(!ui->lineEdit_ipadress->text().isEmpty() && !ui->lineEdit_port->text().isEmpty());
}

//----------------------------
// openConnection(): connects to the Port on the IP adress from the LineEdits
//----------------------------
void MainWindow::openConnection()
{
    counter = 0;
    ui->pushButton_connect->setEnabled(false);
    QString buffer = ui->lineEdit_ipadress->text();
    int buffer2 = ui->lineEdit_port->text().toInt();
    tcpSocket->connectToHost(ui->lineEdit_ipadress->text(), ui->lineEdit_port->text().toInt());

    ui->pushButton_disconnect->setEnabled(true);
    ui->pushButton_sendDB->setEnabled(true);
}

//----------------------------
// closeConnection(): ends the current Connection
//----------------------------
void MainWindow::closeConnection()
{
    tcpSocket->abort();
    tcpSocket->disconnectFromHost();
    ui->pushButton_connect->setEnabled(true);
}

//----------------------------
// readChunk(): reads a data chunk from the connection
//----------------------------
void MainWindow::readChunk()
{
    if(counter == 0)
    {
        QFile dbfile("./database.db");
        if (dbfile.exists())
        {
            dbfile.remove();
        }
    }
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_5_10);
    if (m_blockSize == 0)
    {
        if (tcpSocket->bytesAvailable() < sizeof(quint32))
            return;
    }
    QByteArray line = tcpSocket->readAll();
    QFile databasefile("./database.db");
    if(!databasefile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug() << "Can´t open file!";
        return;
    }
    databasefile.write(line);
//    QFile databasefile1("assets:/database.db");
//    if(!databasefile1.open(QIODevice::WriteOnly | QIODevice::Append))
//    {
//        qDebug() << "Can´t open file 2!";
//        return;
//    }
//    databasefile1.write(line);
    databasefile.close();
//    databasefile1.close();

    counter+= 1;
    ui->pushButton_connect->setEnabled(true);
    ui->label_conclusion->setText("Datenbank erfolgreich empfangen");
}

//----------------------------
// sendDatabase(): sends the current database over the current Connection
//----------------------------
void MainWindow::sendDatabase()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_10);

    QFile file("./database.db");
    if(!file.open(QIODevice::ReadOnly))
        return;
    QByteArray q = file.readAll();
    block.append(q);
    file.close();

    out.device()->seek(0);

    qint64 x=0;
    while(x < block.size())
    {
        qint64 y=tcpSocket->write(block);
        x += y;
    }
//    tcpSocket->abort();
    tcpSocket->disconnectFromHost();
    ui->pushButton_sendDB->setEnabled(false);
    ui->pushButton_connect->setEnabled(true);
    ui->label_conclusion->setText("Datenbank erfolgreich gesendet");

}

//----------------------------
// sessionOpened(): a new network session is opened
//----------------------------
void MainWindow::sessionOpened()
{
    // Save the used configuration
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

    ui->label_conclusion->setText(tr("This examples requires that you run the "
                            "Fortune Server example as well."));

    //    enableGetFortuneButton();
}

//----------------------------
// openSelectClass(): opens the selectClassDialog
//----------------------------
void MainWindow::openSelectClass()
{
    selectClassDialog dialog(this);
    dialog.showFullScreen();
    dialog.exec();
}

//----------------------------
// openSettings(): opens the settingsDialog
//----------------------------
void MainWindow::openSettings()
{

}
