#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "dataSource.h"
#include "frontEnd.h"
#include "backEnd.h"
#include <chrono>

enum class AppState
{
    AppState_Unitialized = 0,
    AppState_Idle, 
    AppState_Running
};

enum class SimulationDensity;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

public slots:
    //! Show the licensing dialog
    void ShowLicenseDialog();

    //! Show the 'Open config file...' dialog
    void ShowOpenConfigDialog();

    //! Play/Fake-streaming the dataset 
    void Run();

	//! Pause playing dataset
	void Pause();

	//!Updates every part of the GUI (called by ui refresh timer)
	void UpdateGUI();

private:
    /*! Open a config file
    * \param[in] fileName The name of the file including the path
    */
    void OpenConfigFile(const QString& fileName);

	//! Initialize the scene and objects
	bool Init();

	//! Step to the next frame and update the scene
	bool Step();

	/*! Sets display mode (visibility of actors) according to the current state
	* \param[in] AppState_Unitialized, AppState_Idle, or AppState_Running
	*/
	void SetState(AppState state);

	Ui::MainWindow*		ui;						//Main UI
	QTimer*				m_UiRefreshTimer;		// Timer that refreshes the UI
    
	CatheterPoints::Ptr m_CathPts = nullptr;	// Catheter point class
    BackEnd::Ptr		m_BackEnd = nullptr;	// Backend optimizer    
    
	std::string			m_Source1FileName;		// Dataset 1 filename
    std::string			m_Source2FileName;		// Dataset 2 filename
	DataSource			m_Source1;				// Fake data source 1
    DataSource			m_Source2;				// Fake data source 2
	
	int					m_FrameRate = 30;			// frame rate of the system, default 30 Hz
	int					m_KeyFrameInterval = 1;		// front end parameter
	int					m_CatheterLengthInMM = 100; // length of the catheter (Unit: mm)

	AppState			m_State = AppState::AppState_Unitialized;		// Internal state of the system
	SimulationDensity	m_NumPointSimualation = SimulationDensity::LOW;	// front end and back end parameter
};

#endif // MAINWINDOW_H
