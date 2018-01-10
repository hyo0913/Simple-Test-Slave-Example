#include "SlaveThread.h"

#define STX 0x02
#define ETX 0x03

SlaveThread::SlaveThread() :
    QThread(NULL),
    m_running(false),
    m_baud(115200), m_databits(QSerialPort::Data8), m_stopbits(QSerialPort::OneStop), m_parity(QSerialPort::NoParity)
{
}

SlaveThread::~SlaveThread()
{
    if( this->isRunning() )
        this->quit();
}

void SlaveThread::slaveStart(const QString &portName, const QString &parms)
{
    m_portName = portName;
    QStringList lstTemp = parms.split("/");
    m_baud = static_cast<QString>(lstTemp.at(0)).toInt();
    m_databits = static_cast<QSerialPort::DataBits>(static_cast<QString>(lstTemp.at(1)).toInt());
    m_stopbits = static_cast<QSerialPort::StopBits>(static_cast<QString>(lstTemp.at(2)).toInt());
    if( lstTemp.at(3) == "None" ) m_parity = QSerialPort::NoParity;
    else if( lstTemp.at(3) == "Even" ) m_parity = QSerialPort::EvenParity;
    else if( lstTemp.at(3) == "Odd" ) m_parity = QSerialPort::OddParity;

    if( this->isRunning() == false )
    {
        m_running = true;
        this->start();
    }
}

void SlaveThread::slaveStop()
{
    m_running = false;
}

void SlaveThread::setResponseMessage(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_respMsg = message;
}

QByteArray SlaveThread::getResponseMessage() const
{
    QMutexLocker locker(&m_mutex);
    return m_respMsg.toLatin1();
}

void SlaveThread::run()
{
    QSerialPort serial(m_portName);
    serial.setBaudRate(m_baud);
    serial.setDataBits(m_databits);
    serial.setStopBits(m_stopbits);
    serial.setParity(m_parity);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    serial.setReadBufferSize(1024);
    emit started(serial.open(QIODevice::ReadWrite));

    while( m_running )
    {
        msleep(1);
        if( serial.isOpen() )
        {
            if( serial.waitForReadyRead(1) )
            {
                response(serial);
            }
        }
    }
}

void SlaveThread::response(QSerialPort &serial)
{
    QByteArray recv = serial.readAll();
    QByteArray resp;
    QByteArray parsed;
    bool stx = false;
    bool etx = false;

    for( int i = 0; i < recv.size(); i++ )
    {
        if( recv.at(i) == STX )
        {
            parsed.clear();
            parsed.append(recv.at(i));
            stx = true;
        }
        else if( stx )
        {
            parsed.append(recv.at(i));
            if( recv.at(i) == ETX )
            {
                etx = true;
                break;
            }
        }

        if( stx == true && i+1 == recv.size() )
        {
            if( serial.waitForReadyRead(100) )
            {
                recv += serial.readAll();
            }
        }
    }

    if( parsed.size() > 0 && etx == true )
    {
        emit received(parsed);
        resp = makeResp(parsed);
        if( resp.isEmpty() == false )
        {
            serial.write(resp);
            emit responsed(resp);
        }
    }
}

QByteArray SlaveThread::makeResp(const QByteArray &recv)
{
    QByteArray ret;

    ret.append(STX);
    ret.append(getResponseMessage());
    ret.append(ETX);

    return ret;
}
