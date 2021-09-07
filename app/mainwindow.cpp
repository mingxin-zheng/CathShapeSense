#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <vector>

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
	// stop the back end
	if (m_BackEnd != NULL)
	{
		m_BackEnd->Stop();
	}
	
	// kill the timer
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
    // read config file (.ini)
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
	QString src1 = settings.value("DataSources/Aurora1", "").toString();
	QString src2 = settings.value("DataSources/Aurora2", "").toString();

	if (src1.length() == 0 || src2.length() == 0)
	{
		return;
	}
	
	QFile srcFile1(src1), srcFile2(src2);
	srcFile1.open(QIODevice::ReadOnly);
	srcFile2.open(QIODevice::ReadOnly);

	if (!srcFile1.exists() || !srcFile2.exists()) 
	{
		return;
	}
	
    m_Source1.SetSourceFilePath(src1.toUtf8().constData());
	m_Source2.SetSourceFilePath(src2.toUtf8().constData());

	m_FrameRate = settings.value("FrontEnd/FrameRate", 30).toInt();
	m_KeyFrameInterval = settings.value("FrontEnd/KeyFrameInterval", 1).toInt();
	m_CatheterLengthInMM = settings.value("BackEnd/CatheterLengthInMM", 100).toInt();

	int pSim = settings.value("FrontEnd/NumPointSimualation(1:Low 2:Med 3:High)", 1).toInt();
	if (pSim == 1)
	{
		m_NumPointSimualation = SimulationDensity::LOW;
	}
	else if (pSim == 2)
	{
		m_NumPointSimualation = SimulationDensity::MEDIUM;
	}
	else if (pSim == 3)
	{
		m_NumPointSimualation = SimulationDensity::HIGH;
	}
	else // DEFAULT to LOW
	{
		m_NumPointSimualation = SimulationDensity::LOW;
	}

    bool success = Init();
    
	if (success)
    {
        SetState(AppState::AppState_Idle);
    }
}

bool MainWindow::Init()
{
	// stop the previous back end if it exists
	if (m_BackEnd != NULL)
	{
		m_BackEnd->Stop();
	}

	m_CathPts = CatheterPoints::Ptr(new CatheterPoints);
	m_BackEnd = BackEnd::Ptr(new BackEnd((int) m_NumPointSimualation, (double) m_CatheterLengthInMM));

	m_Source1.Init();
	m_Source2.Init();

	// set up the front end
	ui->frontEnd->ClearAll();
	ui->frontEnd->SetCatheterPoints(m_CathPts);
	ui->frontEnd->SetBackEnd(m_BackEnd);
	ui->frontEnd->SetKeyFrameInterval(m_KeyFrameInterval);
	ui->frontEnd->SetNumPointSimualation((int) m_NumPointSimualation);
	ui->frontEnd->Init();

	// back end set up
	m_BackEnd->SetCatheterPoints(m_CathPts);

	// Set up timer for refreshing UI
	if (m_UiRefreshTimer != NULL)
	{
		m_UiRefreshTimer->stop();
		delete m_UiRefreshTimer;
		m_UiRefreshTimer = NULL;
	}

	m_UiRefreshTimer = new QTimer(this);
	connect(m_UiRefreshTimer, SIGNAL(timeout()), this, SLOT(UpdateGUI()));

	// Start Timer at interval of 50ms
	int interval = 1000.0 / ((double) m_FrameRate);
	m_UiRefreshTimer->start(interval);
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
    
	// fetch the sensor data for the next frame
	std::vector<double> tracker1, tracker2;
    m_Source1.GetNextFrame(tracker1);
	m_Source2.GetNextFrame(tracker2);
	// update the catheter point (just the ends) based on the sensor locations
	m_CathPts->SetSensor1(tracker1);
	m_CathPts->SetSensor2(tracker2);

	// update the front end display
    ui->frontEnd->AddFrame(); 

	auto t2 = std::chrono::steady_clock::now();
	auto time_used =
		std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	
	qDebug() << "Instant Frame Rate: " << 1.0/time_used.count() << " Hz";
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
