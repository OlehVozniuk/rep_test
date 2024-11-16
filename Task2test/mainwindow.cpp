#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QFile>
#include <QXmlStreamReader>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sendTimer(new QTimer(this)) // Ініціалізація таймера
{
    ui->setupUi(this);

    // Заповнення списків портів та швидкостей
    populatePortList();
    populateBaudRateList();

    // Завантаження конфігурації
    if (!loadConfig()) {
        ui->outputText->append("Failed to load configuration.");
    }

    // Підключення сигналів і слотів
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connectToSerialPort);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::startAutoSend);
    connect(sendTimer, &QTimer::timeout, this, &MainWindow::sendMessage);
}

MainWindow::~MainWindow()
{
    if (serial.isOpen()) {
        serial.close();
    }
    delete ui;
}

void MainWindow::populatePortList()
{
    ui->portComboBox->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        ui->portComboBox->addItem(port.portName());
    }
}

void MainWindow::populateBaudRateList()
{
    ui->baudRateComboBox->clear();
    const QList<qint32> baudRates = QSerialPortInfo::standardBaudRates();
    for (qint32 baudRate : baudRates) {
        ui->baudRateComboBox->addItem(QString::number(baudRate));
    }
}

bool MainWindow::loadConfig()
{
    QFile file("/Users/olegvozniuk/Desktop/sketch_sep26a/Task2test/config.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == QLatin1String("SendDelay")) {
                sendDelay = xml.readElementText().toInt();
                ui->outputText->append("Send delay set to " + QString::number(sendDelay) + " ms");
            } else if (xml.name() == QLatin1String("AutoSend")) {
                autoSendEnabled = (xml.readElementText().toLower() == "true");
                ui->outputText->append("Auto-send set to " + QString(autoSendEnabled ? "enabled" : "disabled"));
            }
        }
    }

    file.close();
    return !xml.hasError();
}



void MainWindow::connectToSerialPort()
{
    if (serial.isOpen()) {
        serial.close();
        ui->outputText->append("Serial port closed.");
    }

    QString portName = ui->portComboBox->currentText();
    qint32 baudRate = ui->baudRateComboBox->currentText().toInt();

    if (portName.isEmpty()) {
        QMessageBox::warning(this, "Error", "No serial port selected. Please select a port from the list.");
        return;
    }

    serial.setPortName(portName);
    serial.setBaudRate(baudRate);

    if (!serial.open(QIODevice::ReadWrite)) {
        QMessageBox::critical(this, "Connection Error",
                              QString("Failed to connect to port '%1'.\n\nPossible reasons:\n"
                                      "- The board is not connected.\n"
                                      "- The selected port is incorrect.\n"
                                      "- Another application is using the port.")
                                  .arg(portName));
        ui->outputText->append("Error: Could not open port " + portName);
        return;
    }

    ui->outputText->append("Successfully connected to " + portName + " at " + QString::number(baudRate) + " baud.");
    connect(&serial, &QSerialPort::readyRead, this, &MainWindow::readData);
}


void MainWindow::startAutoSend()
{
    if (!serial.isOpen()) {
        ui->outputText->append("Error: Serial port is not open. Cannot send message.");
        return;
    }

    QString message = ui->messageInput->text();
    if (message.isEmpty()) {
        ui->outputText->append("Error: Message input is empty. Cannot send message.");
        return;
    }

    if (autoSendEnabled) {
        sendTimer->start(sendDelay);
        ui->outputText->append("Auto-send started with a delay of " + QString::number(sendDelay) + " ms.");
    } else {
        serial.write(message.toUtf8() + '\n');
        ui->outputText->append("Message sent: " + message);
    }
}


void MainWindow::sendMessage()
{
    QString message = ui->messageInput->text();
    if (!message.isEmpty() && serial.isOpen()) {
        serial.write(message.toUtf8() + '\n');
        ui->outputText->append("Message sent: " + message);
    }
}

void MainWindow::readData()
{
    QByteArray data = serial.readAll();
    ui->outputText->append("Received from Arduino: " + QString(data).trimmed());
}
