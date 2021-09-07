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

	//! Initialize the display and vtk pipelines
    void Init();
    
    //! Remove the data set from the scene
    void ClearAll();

    //! Add the current frame to front end for display
    void AddFrame();

    //! Setter for Catheter points
	void SetCatheterPoints(CatheterPoints::Ptr cath) { m_CathPts = cath; }

	//! Setter for BackEnd pointer
	void SetBackEnd(BackEnd::Ptr backEnd) { m_BackEnd = backEnd; }

	//! Setter for Key Frame interval 
	void SetKeyFrameInterval(int keyInt) { m_KeyFrameInterval = keyInt; }

	//! Setter for NumPointSimualation
	void SetNumPointSimualation(int numPts) { m_NumPointSimualation = numPts; }
	
public slots:
    //! Zoom to the extent of the data set in the scene
    void ZoomToExtent();

private:
	//! determine if the current frame is key frame, which will be used for optimization
	bool IsKeyFrame();

	int		m_NumPointSimualation = 6;	// number of points used for simulation, not including the ends measured by sensor For example, if 6 points are used, in total there are 6+2 points
	int		m_KeyFrameInterval = 1;		// if the ptimization is taking a long time, only points in the key frame will be used for the optimization
	int		m_KeyFrameCounter = 1;		// counter to support IsKeyFrame()
	bool	m_FirstTimeDisplay = true;	// if the object is displayed for the first time, apply auto zoom to focus on the object. Default true (do it for the next run)
	CatheterPoints::Ptr			m_CathPts = nullptr;
	std::shared_ptr<BackEnd>	m_BackEnd = nullptr;
    vtkSmartPointer<vtkRenderer>					m_renderer;
	vtkSmartPointer< vtkParametricFunctionSource>	functionSource;
};

#endif // FRONTEND_H
