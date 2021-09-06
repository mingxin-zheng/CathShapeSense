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
        \param[in] tracker1, tracker2 
    */
    void addFrame(std::vector<double> tracker1, std::vector<double> tracker2);

public slots:
    //! Zoom to the extent of the data set in the scene
    void zoomToExtent();

private:
	/*!
		\param[in] tracker1, tracker2
	*/
	void Solve(std::vector<double> tracker1, std::vector<double> tracker2);
    vtkSmartPointer<vtkRenderer> m_renderer;
	vtkSmartPointer< vtkParametricFunctionSource> functionSource;
	int m_NumPointSimualation;
	std::vector<double> m_OptimalFit;
    double* lastOptimalPoints;
};

#endif // SCENEWIDGET_H
