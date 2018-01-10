#ifndef SLAVETHREAD_H
#define SLAVETHREAD_H

#include <QThread>
#include <QSerialPort>
#include <QMutex>

class SlaveThread : public QThread
{
    Q_OBJECT
public:
    explicit SlaveThread();
    ~SlaveThread();

    void slaveStart(const QString &portName, const QString &parms);
    void slaveStop();

    void setResponseMessage(const QString &message);
    QByteArray getResponseMessage() const;

signals:
    void started(bool ok);
    void received(const QByteArray &recv);
    void responsed(const QByteArray &send);

private:
    mutable QMutex m_mutex;
    bool m_running;
    QString m_portName;
    qint32 m_baud;
    QSerialPort::DataBits m_databits;
    QSerialPort::StopBits m_stopbits;
    QSerialPort::Parity m_parity;

    QString m_respMsg;

    void run() override;

    void response(QSerialPort &serial);
    QByteArray makeResp(const QByteArray &recv);
};

#endif // SLAVETHREAD_H
