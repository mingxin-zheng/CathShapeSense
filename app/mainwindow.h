#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "dataset.h"

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
    
    //! Dataset
    Dataset m_dataset;
    AppState m_State;
};

#endif // MAINWINDOW_H
