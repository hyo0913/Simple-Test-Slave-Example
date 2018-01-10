#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QSerialPortInfo"
#include "QMessageBox"
#include "QTime"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_slave(new SlaveThread())
{
    ui->setupUi(this);
    onPortsRefresh();

    connect(ui->pushButtonStart, SIGNAL(clicked(bool)), this, SLOT(onSlaveStart()));
    connect(ui->pushButtonPortRefresh, SIGNAL(clicked(bool)), this, SLOT(onPortsRefresh()));

    //
    connect(m_slave, SIGNAL(started(bool)), this, SLOT(onSlaveStarted(bool)));
    connect(m_slave, SIGNAL(received(QByteArray)), this, SLOT(onReceived(QByteArray)));
    connect(m_slave, SIGNAL(responsed(QByteArray)), this, SLOT(onResponsed(QByteArray)));
}

MainWindow::~MainWindow()
{
    if( m_slave->isRunning() )
    {
        m_slave->quit();
        m_slave->wait();
    }
    delete m_slave;
    delete ui;
}

void MainWindow::onSlaveStart()
{
    if( ui->pushButtonStart->text() == "Start" )
    {
        QString parms = QString("%1/%2/%3/%4")
                .arg(ui->comboBoxBaudrate->currentText())
                .arg(ui->comboBoxDataBits->currentText())
                .arg(ui->comboBoxStopBits->currentText())
                .arg(ui->comboBoxParity->currentText());
        m_slave->slaveStart(ui->comboBoxSerialPort->currentText(), parms);
    }
    else
    {
        m_slave->slaveStop();
        m_slave->quit();
        m_slave->wait();
        ui->pushButtonStart->setText("Start");
    }
}

void MainWindow::onPortsRefresh()
{
    ui->comboBoxSerialPort->clear();

    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();

    for( int i = infos.count(); i > 0 ; i-- )
    {
        ui->comboBoxSerialPort->addItem(infos.at(i-1).portName());
    }
}

void MainWindow::onSlaveStarted(bool ok)
{
    if( ok )
    {
        ui->pushButtonStart->setText("Stop");
    }
    else
    {
        m_slave->slaveStop();
        m_slave->quit();
        m_slave->wait();
        QMessageBox box;
        box.addButton(QMessageBox::Close);
        box.setText("Cannot start");
        box.exec();
    }
}

void MainWindow::onReceived(const QByteArray &recv)
{
    ui->textEditLog->append(QString("[%1] RECV : %2").arg(QTime::currentTime().toString()).arg(QString(recv)));
}

void MainWindow::onResponsed(const QByteArray &resp)
{
    ui->textEditLog->append(QString("[%1] RESP : %2").arg(QTime::currentTime().toString()).arg(QString(resp)));
}
