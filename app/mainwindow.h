#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

public slots:
    //! Show the 'About this application' dialog
    void showAboutDialog();

    //! Show the 'Open file...' dialog
    void showOpenFileDialog();

    /**
     * Make a step forward in dataset
     */
    // todo(mingxin)
    // bool Init();
    // void Run();
    // bool Step();

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

};

#endif // MAINWINDOW_H
