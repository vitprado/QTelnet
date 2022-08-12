#ifndef QTELNETTESTER_H
#define QTELNETTESTER_H

#include <QMainWindow>
#include "QTelnet.h"

namespace Ui
{
	class QTelnetTester;
}

class QTelnetTester : public QMainWindow
{
	Q_OBJECT

	Ui::QTelnetTester *ui;
    QTelnet telnet;
    QMap<QString, QString> deviceList;
    QMap<QString, QString> portList;

    bool manualAddressPort;

public:
	explicit QTelnetTester(QWidget *parent = 0);
	~QTelnetTester();

private slots:
	void onStateChanged(QAbstractSocket::SocketState s);

	void on_btConnect_clicked();
	void onCommand(const QString &cmd);

    void on_deviceListComboBox_currentTextChanged(const QString &arg1);
    void on_portListComboBox_currentTextChanged(const QString &arg1);

    void on_actionClear_triggered();

    void on_actionSave_as_triggered();

    void on_actionManual_Address_Port_toggled(bool arg1);


public slots:
	void setStatusText(const QString &msg, bool onMainWindow = false);
	void addText(const char *msg, int count);
};

#endif // QTELNETTESTER_H
