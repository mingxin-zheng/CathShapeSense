#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dataset.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

	//! Initialize the scene
	bool init();
	bool step();

public slots:
    //! Show the 'About this application' dialog
    void showAboutDialog();

    //! Show the 'Open file...' dialog
    void showOpenFileDialog();

    /**
     * Make a step forward in dataset
     */
    // todo(mingxin)
    
    void run();

protected:
    //! Open a file
    /*!
    \param[in] fileName The name of the file including the path
  */
    void openFile(const QString& fileName);

private:
    Ui::MainWindow* ui;
    // todo(mingxin)
    // std::string config_file_path_;
    // Frontend::Ptr frontend = nullptr;
    
    //! Dataset
    Dataset m_dataset; 
};

#endif // MAINWINDOW_H
