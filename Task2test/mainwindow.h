#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void connectToSerialPort();
    void startAutoSend();
    void sendMessage();
    void readData();

private:
    Ui::MainWindow *ui;
    QSerialPort serial;
    QTimer *sendTimer;  // Таймер для автоматичного відправлення
    int sendDelay = 1000;  // Затримка між повідомленнями в мс

    void populatePortList();
    void populateBaudRateList();
    bool loadConfig();
    bool autoSendEnabled = true; // За замовчуванням автоматична відправка увімкнена

};

#endif // MAINWINDOW_H
