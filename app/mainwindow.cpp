#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <vector>
#include <chrono>

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QSettings>

//todo(mingxin): remove the debug
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, m_UiRefreshTimer(NULL)
{
    ui->setupUi(this);
    SetState(AppState::AppState_Unitialized);
}

MainWindow::~MainWindow() 
{ 
	if (m_UiRefreshTimer != NULL)
	{
		m_UiRefreshTimer->stop();
		delete m_UiRefreshTimer;
		m_UiRefreshTimer = NULL;
	}

	delete ui; 
}

void MainWindow::ShowLicenseDialog()
{
    QMessageBox::information(
        this, "About",
        "By Mingxin Zheng.\nSource code is available only for evaluation and non-commerical purposes");
}

void MainWindow::ShowOpenConfigDialog()
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

	OpenConfigFile(fileName);
}

void MainWindow::OpenConfigFile(const QString& fileName)
{
    // Read a config file before initialization
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
	
    m_Source1.SetSourceFilePath(resultCSV.toUtf8().constData());
    bool success = Init();
    if (success)
    {
        SetState(AppState::AppState_Idle);
    }
}

bool MainWindow::Init()
{
    m_Source1.Init();
    ui->sceneWidget->clearAll();
    ui->sceneWidget->init();

	// Set up timer for refreshing UI
	if (m_UiRefreshTimer != NULL)
	{
		m_UiRefreshTimer->stop();
		delete m_UiRefreshTimer;
		m_UiRefreshTimer = NULL;
	}

	m_UiRefreshTimer = new QTimer(this);
	connect(m_UiRefreshTimer, SIGNAL(timeout()), this, SLOT(UpdateGUI()));

	// Start Timer at inteval of 50ms
	m_UiRefreshTimer->start(20);
    return true;
}

void MainWindow::Run()
{
	SetState(AppState::AppState_Running);
	UpdateGUI();
}

void MainWindow::Pause()
{
	SetState(AppState::AppState_Idle);
	UpdateGUI();
}

void MainWindow::UpdateGUI()
{
	if (m_State == AppState::AppState_Running)
	{
		Step();
	}
}
bool MainWindow::Step()
{
	auto t1 = std::chrono::steady_clock::now();
    std::vector<double> frame;
    m_Source1.GetNextFrame(frame);
    ui->sceneWidget->addFrame(frame); // AddFrame: convert the numbers as correct inputs
	auto t2 = std::chrono::steady_clock::now();
	auto time_used =
		std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	
	qDebug() << "Time elapsed: " << time_used.count() << " seconds.";

    return false;
}

void MainWindow::SetState(AppState state)
{
    m_State = state;
    if (m_State == AppState::AppState_Unitialized)
    {
		ui->actionPlay->setEnabled(false);
		ui->actionZoomToExtent->setEnabled(false);
    }
	else if (m_State == AppState::AppState_Idle)
	{
		ui->actionZoomToExtent->setEnabled(true);
		ui->actionPlay->setEnabled(true);
		ui->actionPlay->setIcon(QIcon(":/icons/play.png"));
		disconnect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(Pause()));
		connect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(Run()));
	}
	else if (m_State == AppState::AppState_Running)
	{
		ui->actionPlay->setEnabled(true);
		ui->actionZoomToExtent->setEnabled(true);
		ui->actionPlay->setIcon(QIcon(":/icons/pause.png"));
		disconnect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(Run()));
		connect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(Pause()));
	}
}
