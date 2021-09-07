//#define _DISABLE_EXTENDED_ALIGNED_STORAGE
//#define NOGDI

#include "frontEnd.h"

//#include <ceres/ceres.h>

#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTubeFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkTupleInterpolator.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include "backEnd.h"

#include <chrono>
//todo(mingxin): remove the debug
#include <QDebug>

FrontEnd::FrontEnd(QWidget* parent)
    : QVTKOpenGLNativeWidget(parent)
{
    vtkNew<vtkGenericOpenGLRenderWindow> window;
    setRenderWindow(window.Get());

    // Camera
    vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
    camera->SetViewUp(1, 0, 0);
    camera->SetPosition(0, 0, 1);
    camera->SetFocalPoint(0, 0, 0);
    // Renderer
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->SetActiveCamera(camera);
    m_renderer->SetBackground(0.5, 0.5, 0.5);
    renderWindow()->AddRenderer(m_renderer);

	functionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
}

void FrontEnd::Init()
{
	auto tubePolyData = functionSource->GetOutput();

	vtkNew<vtkTubeFilter> tuber;
	tuber->SetInputData(tubePolyData);
	tuber->SetNumberOfSides(20);
	tuber->SetRadius(1);

	//--------------
	// Create the tubes
	// Setup actors and mappers
	vtkNew<vtkPolyDataMapper> tubeMapper;
	tubeMapper->SetInputConnection(tuber->GetOutputPort());
	tubeMapper->SetScalarRange(tubePolyData->GetScalarRange());

	vtkNew<vtkActor> tubeActor;
	tubeActor->SetMapper(tubeMapper);
	tubeActor->GetProperty()->SetOpacity(0.6);

	// Setup render window, renderer, and interactor
	vtkNew<vtkNamedColors> colors;
	// vtkNew<vtkRenderer> renderer;
	m_renderer->UseHiddenLineRemovalOn();
	m_renderer->AddActor(tubeActor);
	m_renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

}


void FrontEnd::ClearAll()
{
	// note(mingxin): Only ONE actor in the scene
    vtkActor* actor = m_renderer->GetActors()->GetLastActor();
    if (actor != nullptr) {
       m_renderer->RemoveActor(actor);
    }
	m_FirstTimeDisplay = true;
    renderWindow()->Render();
}

void FrontEnd::ZoomToExtent()
{
    // Zoom to extent of last added actor
    vtkSmartPointer<vtkActor> actor = m_renderer->GetActors()->GetLastActor();
    if (actor != nullptr) {
        m_renderer->ResetCamera(actor->GetBounds());
		m_renderer->GetActiveCamera()->SetClippingRange(0.01, 2000);
    }

    renderWindow()->Render();
}

void FrontEnd::AddFrame()
{
	auto t1 = std::chrono::steady_clock::now();
	if (IsKeyFrame())
	{
		m_BackEnd->UpdateAllPoints();
	}

	std::vector<double> optimalFit = m_CathPts->GetAllPoints();

	if (optimalFit.size() > 0)
	{
		std::vector<double> tracker1 = m_CathPts->GetTracker1();
		std::vector<double> tracker2 = m_CathPts->GetTracker2();

		vtkNew<vtkPoints> points;
		points->InsertPoint(0, tracker1[0], tracker1[1], tracker1[2]);
		for (int i = 0; i < m_NumPointSimualation; i++)
		{
			int x = i * 3, y = 3 * i + 1, z = 3 * i + 2;
			points->InsertPoint(i + 1, optimalFit[x],
				optimalFit[y],
				optimalFit[z]);
		}
		points->InsertPoint(m_NumPointSimualation + 1, tracker2[0], tracker2[1], tracker2[2]);

		// Fit a spline to the points
		vtkNew<vtkParametricSpline> spline;
		spline->SetPoints(points);
		functionSource->SetParametricFunction(spline);
		functionSource->SetUResolution(10 * points->GetNumberOfPoints());
		functionSource->Update();

		renderWindow()->Render();

		if (m_FirstTimeDisplay)
		{
			ZoomToExtent();
			m_FirstTimeDisplay = false;
		}

	}

	auto t2 = std::chrono::steady_clock::now();
	auto time_used =
		std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

	qDebug() << "Time Elasped for BackEnd " << time_used.count() << " secs";
}

bool FrontEnd::IsKeyFrame()
{
	bool success = (m_KeyFrameCounter == 0);
	m_KeyFrameCounter--;
	if (m_KeyFrameCounter < 0)
	{
		m_KeyFrameCounter = m_KeyFrameInterval - 1;
	}
	return success;
}