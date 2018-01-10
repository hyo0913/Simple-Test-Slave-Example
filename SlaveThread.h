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

    void setAddress(uint address);
    void setUseLF(bool use);
    void setResponseR20(const QString &message);
    QByteArray getResponseR20() const;
    void setResponseR26(const QString &message);
    QByteArray getResponseR26() const;
    void setResponseR30(const QString &message);
    QByteArray getResponseR30() const;
    void setResponseR40(const QString &message);
    QByteArray getResponseR40() const;
    void setResponseR45(const QString &message);
    QByteArray getResponseR45() const;
    void setResponseCmd50(int error);
    void setResponseR70_08(const QString &message);
    QByteArray getResponseR70_08() const;
    void setResponseR70_10(const QString &message);
    QByteArray getResponseR70_10() const;
    void setResponseR71(const QString &message);
    QByteArray getResponseR71() const;
    void setResponseR79(const QString &message);
    QByteArray getResponseR79() const;

signals:
    void started(bool ok);
    void received(const QByteArray &recv);
    void responsed(const QByteArray &send);

    void receivedW60(const QString &data);
    void receivedW80_08(const QString &data);
    void receivedW80_10(const QString &data);
    void receivedW70(const QString &data);

private:
    mutable QMutex m_mutex;
    bool m_running;
    QString m_portName;
    qint32 m_baud;
    QSerialPort::DataBits m_databits;
    QSerialPort::StopBits m_stopbits;
    QSerialPort::Parity m_parity;

    uint m_address;
    bool m_useLF;
    QString m_respR20;
    QString m_respR26;
    QString m_respR30;
    QString m_respR40;
    QString m_respR45;
    int m_respCmd50;
    QString m_respR70_08;
    QString m_respR70_10;
    QString m_respR71;
    QString m_respR79;

    void run() override;

    void response(QSerialPort &serial);
    QByteArray makeResp(const QByteArray &recv);
};

#endif // SLAVETHREAD_H
