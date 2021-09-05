#include "scenewidget.h"

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

SceneWidget::SceneWidget(QWidget* parent)
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

void SceneWidget::init()
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


void SceneWidget::clearAll()
{
	// note(mingxin): Only ONE actor in the scene
    vtkActor* actor = m_renderer->GetActors()->GetLastActor();
    if (actor != nullptr) {
       m_renderer->RemoveActor(actor);
    }

    renderWindow()->Render();
}

void SceneWidget::zoomToExtent()
{
    // Zoom to extent of last added actor
    vtkSmartPointer<vtkActor> actor = m_renderer->GetActors()->GetLastActor();
    if (actor != nullptr) {
        m_renderer->ResetCamera(actor->GetBounds());
		m_renderer->GetActiveCamera()->SetClippingRange(0.01, 2000);
    }

    renderWindow()->Render();
}

void SceneWidget::addFrame(std::vector<double> frame)
{
	vtkNew<vtkPoints> points;

	for (int i = 0; i < 8; i++)
	{
		int x = i * 3, y = 3 * i + 1, z = 3 * i + 2;
		points->InsertPoint(i, frame[x]*0.5, frame[y] * 0.5, frame[z] * 0.5);
	}
	
	// Fit a spline to the points
	vtkNew<vtkParametricSpline> spline;
	spline->SetPoints(points);
	// vtkNew<vtkParametricFunctionSource> functionSource;
	functionSource->SetParametricFunction(spline);
	functionSource->SetUResolution(10 * points->GetNumberOfPoints());
	functionSource->Update();
		
	renderWindow()->Render();
}