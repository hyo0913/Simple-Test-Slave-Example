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

    connect(m_slave, SIGNAL(receivedW60(QString)), this, SLOT(onReceivedW60(QString)));
    connect(m_slave, SIGNAL(receivedW80_08(QString)), this, SLOT(onReceivedW80_08(QString)));
    connect(m_slave, SIGNAL(receivedW80_10(QString)), this, SLOT(onReceivedW80_10(QString)));
    connect(m_slave, SIGNAL(receivedW70(QString)), this, SLOT(onReceivedW70(QString)));

    // responses
    connect(ui->spinBoxAddress, SIGNAL(valueChanged(int)), this, SLOT(onAddressChanged(int)));
    m_slave->setAddress(static_cast<uint>(ui->spinBoxAddress->value()));
    connect(ui->comboBoxTerminator, SIGNAL(currentIndexChanged(QString)), this, SLOT(onTerminatorChanged(QString)));
    connect(ui->lineEditR20, SIGNAL(textChanged(QString)), this, SLOT(onR20Changed(QString)));
    m_slave->setResponseR20(ui->lineEditR20->text());
    connect(ui->lineEditR26, SIGNAL(textChanged(QString)), this, SLOT(onR26Changed(QString)));
    m_slave->setResponseR26(ui->lineEditR26->text());
    connect(ui->lineEditR30, SIGNAL(textChanged(QString)), this, SLOT(onR30Changed(QString)));
    m_slave->setResponseR30(ui->lineEditR30->text());
    connect(ui->lineEditR40, SIGNAL(textChanged(QString)), this, SLOT(onR40Changed(QString)));
    m_slave->setResponseR40(ui->lineEditR40->text());
    connect(ui->lineEditR45, SIGNAL(textChanged(QString)), this, SLOT(onR45Changed(QString)));
    m_slave->setResponseR45(ui->lineEditR45->text());
    connect(ui->spinBoxCmd50, SIGNAL(valueChanged(int)), this, SLOT(onCmd50Changed(int)));
    connect(ui->lineEditR70_08, SIGNAL(textChanged(QString)), this, SLOT(onR70_08Changed(QString)));
    m_slave->setResponseR70_08(ui->lineEditR70_08->text());
    connect(ui->lineEditR70_10, SIGNAL(textChanged(QString)), this, SLOT(onR70_10Changed(QString)));
    m_slave->setResponseR70_10(ui->lineEditR70_10->text());
    connect(ui->lineEditR71, SIGNAL(textChanged(QString)), this, SLOT(onR71Changed(QString)));
    m_slave->setResponseR71(ui->lineEditR71->text());
    connect(ui->lineEditR79, SIGNAL(textChanged(QString)), this, SLOT(onR79Changed(QString)));
    m_slave->setResponseR79(ui->lineEditR79->text());
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
    ui->textEditLog->append(QString("[%1] RECV : %2").arg(QTime::currentTime().toString()).arg(QString(recv).remove("\n").remove("\r")));
}

void MainWindow::onResponsed(const QByteArray &resp)
{
    ui->textEditLog->append(QString("[%1] RESP : %2").arg(QTime::currentTime().toString()).arg(QString(resp).remove("\n").remove("\r")));
}

void MainWindow::onReceivedW60(const QString &data)
{
    ui->lineEditR30->setText(data);
}

void MainWindow::onReceivedW80_08(const QString &data)
{
    ui->lineEditR70_08->setText(data);
}

void MainWindow::onReceivedW80_10(const QString &data)
{
    ui->lineEditR70_10->setText(data);
}

void MainWindow::onReceivedW70(const QString &data)
{
    ui->lineEditR71->setText(data);
}

void MainWindow::onAddressChanged(int value)
{
    m_slave->setAddress(static_cast<uint>(value));
}

void MainWindow::onTerminatorChanged(const QString &text)
{
    if( text.contains("LF", Qt::CaseInsensitive) )
    {
        m_slave->setUseLF(true);
    }
    else
    {
        m_slave->setUseLF(false);
    }
}

void MainWindow::onR20Changed(const QString &changed)
{
    m_slave->setResponseR20(changed);
}

void MainWindow::onR26Changed(const QString &changed)
{
    m_slave->setResponseR26(changed);
}

void MainWindow::onR30Changed(const QString &changed)
{
    m_slave->setResponseR30(changed);
}

void MainWindow::onR40Changed(const QString &changed)
{
    m_slave->setResponseR40(changed);
}

void MainWindow::onR45Changed(const QString &changed)
{
    m_slave->setResponseR45(changed);
}

void MainWindow::onCmd50Changed(int value)
{
    m_slave->setResponseCmd50(value);
}

void MainWindow::onR70_08Changed(const QString &changed)
{
    m_slave->setResponseR70_08(changed);
}

void MainWindow::onR70_10Changed(const QString &changed)
{
    m_slave->setResponseR70_10(changed);
}

void MainWindow::onR71Changed(const QString &changed)
{
    m_slave->setResponseR71(changed);
}

void MainWindow::onR79Changed(const QString &changed)
{
    m_slave->setResponseR79(changed);
}
