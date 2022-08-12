#include "QTelnetTester.h"
#include "ui_QTelnetTester.h"
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QSettings>

QTelnetTester::QTelnetTester(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::QTelnetTester),
	telnet(this)
{
    ui->setupUi(this);
	connect( &telnet, SIGNAL(newData(const char*,int)), this, SLOT(addText(const char*,int)) );
	connect( &telnet, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)) );
	connect( ui->cbCmd, SIGNAL(command(QString)), this, SLOT(onCommand(QString)));

    QSettings settings("settings.ini", QSettings::IniFormat);

    manualAddressPort = settings.value("MANUAL_ADDRESS_PORT", true).toBool();
    ui->actionManual_Address_Port->setChecked(manualAddressPort);
    ui->leAddr->setEnabled(manualAddressPort);
    ui->sbPort->setEnabled(manualAddressPort);

    settings.beginGroup("DEVICES");
    QStringList DeviceKeys = settings.childKeys();
    foreach (QString key, DeviceKeys) {
         deviceList[key] = settings.value(key).toString();
    }
    settings.endGroup();
    ui->leAddr->setText(deviceList.first());

    settings.beginGroup("PORTS");
    QStringList portKeys = settings.childKeys();
    foreach (QString key, portKeys) {
         portList[key] = settings.value(key).toString();
    }
    settings.endGroup();
    ui->sbPort->setValue(portList.first().toInt());

    ui->portListComboBox->addItems(QStringList(portList.keys()));
    ui->deviceListComboBox->addItems(QStringList(deviceList.keys()));
}

QTelnetTester::~QTelnetTester()
{
	telnet.disconnectFromHost();
	delete ui;
	ui = Q_NULLPTR;
}

void QTelnetTester::onStateChanged(QAbstractSocket::SocketState s)
{
	switch( s )
	{
	case QAbstractSocket::UnconnectedState:
		setStatusText( tr("Desconectado"), true );
		ui->btConnect->setText( tr("Conectar") );
		break;
	case QAbstractSocket::HostLookupState:
		setStatusText( tr("Resolviendo DNS %1").arg(telnet.peerName()), true );
		ui->btConnect->setText( tr("Cancelar") );
		break;
	case QAbstractSocket::ConnectingState:
		setStatusText( tr("Conectando a %1").arg(telnet.peerInfo()), true );
		ui->btConnect->setText( tr("Cancelar") );
		break;
	case QAbstractSocket::ConnectedState:
		setStatusText( tr("Conectado a %1").arg(telnet.peerInfo()), true );
		ui->btConnect->setText( tr("Desconectar") );
		break;
	case QAbstractSocket::BoundState:
		setStatusText( tr("Enlazado"), true );
		ui->btConnect->setText( tr("- - - -") );
		break;
	case QAbstractSocket::ListeningState:
		setStatusText( tr("Escuchando"), true );
		ui->btConnect->setText( tr("- - - -") );
		break;
	case QAbstractSocket::ClosingState:
		setStatusText( tr("Cerrando"), true );
		ui->btConnect->setText( tr("Forzar cierre") );
		break;

	}
}

void QTelnetTester::setStatusText(const QString &msg, bool onQTelnetTester)
{
	if( onQTelnetTester )
	{
		addText(msg.toLatin1(), msg.count());
		if( !msg.endsWith("\n") )
			addText("\n", 1);
	}
	ui->statusBar->showMessage(msg);
}

void QTelnetTester::onCommand(const QString &cmd)
{
	if( telnet.isConnected() )
	{
		telnet.sendData(cmd.toLatin1());
		telnet.sendData("\n");
        ui->cbCmd->clear();
	}
}

void QTelnetTester::on_btConnect_clicked()
{
	if( telnet.isConnected() )
		telnet.disconnectFromHost();
	else
		telnet.connectToHost(ui->leAddr->text(), ui->sbPort->value());
}

void QTelnetTester::addText(const char *msg, int count)
{
	ui->teOutput->insertPlainText( QByteArray(msg, count) );
	ui->teOutput->verticalScrollBar()->setValue(0xFFFFFFF);
}

void QTelnetTester::on_deviceListComboBox_currentTextChanged(const QString &arg1)
{
    ui->leAddr->setText(deviceList[arg1]);

    if( telnet.isConnected() ){
        telnet.disconnectFromHost();
    }
    telnet.connectToHost(ui->leAddr->text(), ui->sbPort->value());
}

void QTelnetTester::on_portListComboBox_currentTextChanged(const QString &arg1)
{
    ui->sbPort->setValue(portList[arg1].toInt());

    if( telnet.isConnected() ){
        telnet.disconnectFromHost();
    }
    telnet.connectToHost(ui->leAddr->text(), ui->sbPort->value());
}


void QTelnetTester::on_actionClear_triggered()
{
    ui->teOutput->clear();
}


void QTelnetTester::on_actionSave_as_triggered()
{
    QDateTime dateTime = dateTime.currentDateTime();
    QString nameToSave = dateTime.toString("yyyyMMdd_HHmmss") + "_output.txt";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Text File"), nameToSave, tr("Text Files (*.txt)"));
    if (fileName != "")
    {
        QFile file(QFileInfo(fileName).absoluteFilePath());

        if (file.exists())
        {
            QMessageBox::StandardButton chosenButton
                = QMessageBox::warning(this, tr("File exists"), tr("The file already exists. Do you want to overwrite it?"),
                    QMessageBox::Ok | QMessageBox::Cancel,
                    QMessageBox::Cancel);
            if (chosenButton != QMessageBox::Ok)
            {
                return; //Save was cancelled
            }
        }
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, tr("Error"), tr("Failed to save file"));
            return; //Aborted
        }
        //All ok - save data
        QString text = ui->teOutput->toPlainText();
        QTextStream out(&file);
        out << text;
        file.close();
    }
}


void QTelnetTester::on_actionManual_Address_Port_toggled(bool arg1)
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.setValue("MANUAL_ADDRESS_PORT", arg1);
    ui->actionManual_Address_Port->setChecked(arg1);
    ui->leAddr->setEnabled(arg1);
    ui->sbPort->setEnabled(arg1);
}

