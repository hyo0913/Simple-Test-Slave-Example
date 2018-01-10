#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SlaveThread.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SlaveThread* m_slave;

private slots:
    void onSlaveStart();
    void onPortsRefresh();
    void onSlaveStarted(bool ok);
    void onReceived(const QByteArray &recv);
    void onResponsed(const QByteArray &resp);
    void onReceivedW60(const QString &data);
    void onReceivedW80_08(const QString &data);
    void onReceivedW80_10(const QString &data);
    void onReceivedW70(const QString &data);

    void onAddressChanged(int value);
    void onTerminatorChanged(const QString &text);
    void onR20Changed(const QString &changed);
    void onR26Changed(const QString &changed);
    void onR30Changed(const QString &changed);
    void onR40Changed(const QString &changed);
    void onR45Changed(const QString &changed);
    void onCmd50Changed(int value);
    void onR70_08Changed(const QString &changed);
    void onR70_10Changed(const QString &changed);
    void onR71Changed(const QString &changed);
    void onR79Changed(const QString &changed);
};

#endif // MAINWINDOW_H
