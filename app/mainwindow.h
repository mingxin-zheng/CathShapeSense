#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "dataSource.h"
#include "frontEnd.h"
#include "backEnd.h"

enum class AppState
{
    AppState_Unitialized = 0,
    AppState_Idle, 
    AppState_Running
};

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

    /**
     * Play the dataset
     */
    void Run();

	/**
	 * Pause playing dataset
	 */
	void Pause();

	/*!
	* Updates every part of the GUI (called by ui refresh timer)
	*/
	void UpdateGUI();

protected:
    /*! Open a config file
    * \param[in] fileName The name of the file including the path
    */
    void OpenConfigFile(const QString& fileName);

	/*! Timer that refreshes the UI */
	QTimer* m_UiRefreshTimer;

private:
	//! Initialize the scene
	bool Init();
	bool Step();

    /*! Sets display mode (visibility of actors) according to the current state */
    void SetState(AppState);
    Ui::MainWindow* ui;
    
    CatheterPoints::Ptr m_CathPts = nullptr;    
    BackEnd::Ptr m_BackEnd = nullptr;

    //! Dataset
    std::string m_Source1FileName;
    std::string m_Source2FileName;
    DataSource m_Source1;
    DataSource m_Source2;
    AppState m_State;
};

#endif // MAINWINDOW_H
