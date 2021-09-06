#ifndef FRONTEND_H
#define FRONTEND_H

#include <vector>
#include <thread>
#include <mutex>

#include <QVTKOpenGLNativeWidget.h>

#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkParametricFunctionSource.h>

#include "catheterPoints.h"
#include "backEnd.h"

class FrontEnd : public QVTKOpenGLNativeWidget {
    Q_OBJECT
public:
    explicit FrontEnd(QWidget* parent = nullptr);

    void Init();
    
    //! Remove the data set from the scene
    void ClearAll();

    //! Add a data set to the scene
    /*!
        \param[in] tracker1, tracker2 
    */
    void AddFrame(std::vector<double> tracker1, std::vector<double> tracker2);

    /*!
	
	*/
	void SetCatheterPoints(CatheterPoints::Ptr cath) { m_CathPts = cath; };

	/*!
	
	*/
	void SetBackEnd(BackEnd::Ptr backEnd) { m_BackEnd = backEnd; };

public slots:
    //! Zoom to the extent of the data set in the scene
    void ZoomToExtent();

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

    CatheterPoints::Ptr m_CathPts = nullptr;
	std::shared_ptr<BackEnd> m_BackEnd = nullptr;
};

#endif // FRONTEND_H
