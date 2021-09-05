#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QVTKOpenGLNativeWidget.h>
#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vector>
#include <vtkParametricFunctionSource.h>

class SceneWidget : public QVTKOpenGLNativeWidget {
    Q_OBJECT
public:
    explicit SceneWidget(QWidget* parent = nullptr);

    void init();
    
    //! Remove the data set from the scene
    void clearAll();

    //! Add a data set to the scene
    /*!
        \param[in] dataSet The data set to add
    */
    void addFrame(std::vector<double>);

public slots:
    //! Zoom to the extent of the data set in the scene
    void zoomToExtent();

private:
    vtkSmartPointer<vtkRenderer> m_renderer;
	vtkSmartPointer< vtkParametricFunctionSource> functionSource;
};

#endif // SCENEWIDGET_H
