#include "SlaveThread.h"

#define STX 0x02
#define ETX 0x03
#define CR  0x0D
#define LF  0x0A

SlaveThread::SlaveThread() :
    QThread(NULL),
    m_running(false),
    m_baud(115200), m_databits(QSerialPort::Data8), m_stopbits(QSerialPort::OneStop), m_parity(QSerialPort::NoParity),
    m_address(0), m_useLF(true)
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

void SlaveThread::setAddress(uint address)
{
    QMutexLocker locker(&m_mutex);
    m_address = address;
}

void SlaveThread::setUseLF(bool use)
{
    QMutexLocker locker(&m_mutex);
    m_useLF = use;
}

void SlaveThread::setResponseR20(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_respR20 = message;
}

QByteArray SlaveThread::getResponseR20() const
{
    QMutexLocker locker(&m_mutex);
    return m_respR20.toLatin1();
}

void SlaveThread::setResponseR26(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_respR26 = message;
}

QByteArray SlaveThread::getResponseR26() const
{
    QMutexLocker locker(&m_mutex);
    return m_respR26.toLatin1();
}

void SlaveThread::setResponseR30(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_respR30 = message;
}

QByteArray SlaveThread::getResponseR30() const
{
    QMutexLocker locker(&m_mutex);
    return m_respR30.toLatin1();
}

void SlaveThread::setResponseR40(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_respR40 = message;
}

QByteArray SlaveThread::getResponseR40() const
{
    QMutexLocker locker(&m_mutex);
    return m_respR40.toLatin1();
}

void SlaveThread::setResponseR45(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_respR45 = message;
}

QByteArray SlaveThread::getResponseR45() const
{
    QMutexLocker locker(&m_mutex);
    return m_respR45.toLatin1();
}

void SlaveThread::setResponseCmd50(int error)
{
    QMutexLocker locker(&m_mutex);
    m_respCmd50 = error;
}

void SlaveThread::setResponseR70_08(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_respR70_08 = message;
}

QByteArray SlaveThread::getResponseR70_08() const
{
    QMutexLocker locker(&m_mutex);
    return m_respR70_08.toLatin1();
}

void SlaveThread::setResponseR70_10(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_respR70_10 = message;
}

QByteArray SlaveThread::getResponseR70_10() const
{
    QMutexLocker locker(&m_mutex);
    return m_respR70_10.toLatin1();
}

void SlaveThread::setResponseR71(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_respR71 = message;
}

QByteArray SlaveThread::getResponseR71() const
{
    QMutexLocker locker(&m_mutex);
    return m_respR71.toLatin1();
}

void SlaveThread::setResponseR79(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_respR79 = message;
}

QByteArray SlaveThread::getResponseR79() const
{
    QMutexLocker locker(&m_mutex);
    return m_respR79.toLatin1();
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
        if( recv.at(i) == '@' )
        {
            parsed.clear();
            parsed.append(recv.at(i));
            stx = true;
        }
        else if( stx )
        {
            parsed.append(recv.at(i));
            if( recv.at(i) == CR )
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
    QString cmd = recv.mid(3, 2);
    int cmdNo = 0;

    if( cmd.at(0) != '5' )
    {
        cmdNo = cmd.toInt();
    }

    ret.append('@');
    ret.append(QString("%1").arg(m_address, 2, 10, QChar('0')).trimmed());
    ret.append(cmd.trimmed());

    if( 20 <= cmdNo && cmdNo <= 25 )
    {
        ret.append(getResponseR20());
    }
    else if( cmdNo == 26 )
    {
        ret.append(getResponseR26());
    }
    else if( 30 <= cmdNo && cmdNo <= 39 )
    {
        ret.append(getResponseR30());
    }
    else if( 40 <= cmdNo && cmdNo <= 42 )
    {
        ret.append(getResponseR40());
    }
    else if( 45 <= cmdNo && cmdNo <= 46 )
    {
        ret.append(getResponseR45());
    }
    else if( cmd.at(0) == '5' )
    {
        if( m_respCmd50 > 0 )
        {
            ret.clear();
            ret.append('@');
            ret.append(QString("%1").arg(m_address, 2, 10, QChar('0')).trimmed());
            ret.append(QString("ER").trimmed());
            ret.append(QString("%1").arg(m_respCmd50, 2, 10, QChar('0')).trimmed());
        }
    }
    else if( 60 <= cmdNo && cmdNo <= 69 )
    {
        ret.append(recv.mid(5, 9));
        emit receivedW60(QString(recv.mid(5, 9)));
    }
    else if( 70 == cmdNo && (recv.indexOf(CR) - 7) == 6 )
    {
        ret.append(recv.mid(5, 2));
        ret.append(recv.mid(7, (recv.indexOf(CR) - 7)));
        emit receivedW70(recv.mid(7, (recv.indexOf(CR) - 7)));
    }
    else if( 71 == cmdNo && (recv.indexOf(CR) - 7) == 6 )
    {
        ret.append(recv.mid(5, 2));
        ret.append(getResponseR71());
    }
    else if( (70 <= cmdNo && cmdNo <= 75) && (recv.indexOf(CR) - 7) == 10 )
    {
        ret.append(recv.mid(5, 2));
        ret.append(getResponseR70_08());
    }
    else if( (70 <= cmdNo && cmdNo <= 75) && (recv.indexOf(CR) - 7) == 11 )
    {
        ret.append(recv.mid(5, 2));
        ret.append(getResponseR70_10());
    }
    else if( (80 <= cmdNo && cmdNo <= 85) && (recv.indexOf(CR) - 7) == 10 )
    {
        ret.append(recv.mid(7, (recv.indexOf(CR) - 7)));
        emit receivedW80_08(recv.mid(7, (recv.indexOf(CR) - 7)));
    }
    else if( (80 <= cmdNo && cmdNo <= 85) && (recv.indexOf(CR) - 7) == 11 )
    {
        ret.append(recv.mid(7, (recv.indexOf(CR) - 7)));
        emit receivedW80_10(recv.mid(7, (recv.indexOf(CR) - 7)));
    }
    else if( 79 == cmdNo )
    {
        ret.append(recv.mid(5, 2));
        ret.append(getResponseR79());
    }

    ret.append(CR);
    ret.append(LF);

    return ret;
}
