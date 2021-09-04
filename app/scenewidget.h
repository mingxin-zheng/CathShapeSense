#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QVTKOpenGLNativeWidget.h>
#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <vtkParametricFunctionSource.h>

class SceneWidget : public QVTKOpenGLNativeWidget {
    Q_OBJECT
public:
    explicit SceneWidget(QWidget* parent = nullptr);

    //! Add a data set to the scene
    /*!
    \param[in] dataSet The data set to add
    */
    void addDataSet(vtkSmartPointer<vtkDataSet> dataSet);

    //! Remove the data set from the scene
    void removeDataSet();

    // todo(mingxin): 
    // void AddCurrentFrame()
public slots:
    //! Zoom to the extent of the data set in the scene
    void zoomToExtent();

private:
    // todo(mingxin):
    // RenderImage()?

    vtkSmartPointer<vtkRenderer> m_renderer;
	int counter = 0;
	vtkSmartPointer< vtkParametricFunctionSource> functionSource;
};

#endif // SCENEWIDGET_H
