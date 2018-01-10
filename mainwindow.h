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
};

#endif // MAINWINDOW_H
