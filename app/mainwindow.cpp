#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <vtkDataSetReader.h>
#include <QSettings>
#include <QDebug>
#include <vector>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::showAboutDialog()
{
    QMessageBox::information(
        this, "About",
        "By Martijn Koopman.\nSource code available under Apache License 2.0.");
}

void MainWindow::showOpenFileDialog()
{
    // todo(mingxin): read txt for config
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), "",
        "Configuration Files (*.ini)");

    // Open file
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    // Return on Cancel
	if (!file.exists())
	{
		return;
	}

    openFile(fileName);
}

void MainWindow::openFile(const QString& fileName)
{
    // todo(mingxin): read a text file line by line
	QSettings settings(QString(fileName), QSettings::IniFormat);
	QString resultCSV = settings.value("inputs/resultCSV", "").toString();
	if (resultCSV.length() == 0)
	{
		return;
	}
	
	QFile file(resultCSV);
	file.open(QIODevice::ReadOnly);
	if (!file.exists())
	{
		return;
	}
	
    m_dataset.setDatasetPath(resultCSV.toUtf8().constData());
    init();
}

bool MainWindow::init()
{
    m_dataset.clearAll();
    m_dataset.init();
    ui->sceneWidget->clearAll();
    ui->sceneWidget->init();
    return true;
}

void MainWindow::run()
{
	while (1)
	{
		if (step() == false)
		{
			break;
		}
	}
}

bool MainWindow::step()
{
    std::vector<double> frame;
    m_dataset.getNextFrame(frame);
    ui->sceneWidget->addFrame(frame); // AddFrame: convert the numbers as correct inputs
    return false;
}

