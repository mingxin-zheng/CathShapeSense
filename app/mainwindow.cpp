#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include <vtkDataSetReader.h>

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
        "VTK Files (*.vtk)");

    // Open file
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    // Return on Cancel
    if (!file.exists())
        return;

    openFile(fileName);
}

void MainWindow::openFile(const QString& fileName)
{
    // todo(mingxin): read a text file
    ui->sceneWidget->removeDataSet();

    // Create reader
    vtkSmartPointer<vtkDataSetReader> reader = vtkSmartPointer<vtkDataSetReader>::New();
    reader->SetFileName(fileName.toStdString().c_str());

    // Read the file
    reader->Update();

    // Add data set to 3D view
    vtkSmartPointer<vtkDataSet> dataSet = reader->GetOutput();
    if (dataSet != nullptr) {
        ui->sceneWidget->addDataSet(reader->GetOutput());
    }

    // todo(mingxin): if success, run Init()
}

// todo(mingxin)
/*
bool MainWindow::Step(){
    dataset_->NextFrame();
    frontend_->AddFrame(); // AddFrame: convert the numbers as correct inputs
    return true;
}

*/
