#include "scenewidget.h"

#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTubeFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkTubeFilter.h>
#include <vtkTupleInterpolator.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

SceneWidget::SceneWidget(QWidget* parent)
    : QVTKOpenGLNativeWidget(parent)
{
    vtkNew<vtkGenericOpenGLRenderWindow> window;
    setRenderWindow(window.Get());

    // Camera
    vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
    camera->SetViewUp(0, 1, 0);
    camera->SetPosition(0, 0, 10);
    camera->SetFocalPoint(0, 0, 0);

    // Renderer
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->SetActiveCamera(camera);
    m_renderer->SetBackground(0.5, 0.5, 0.5);
    renderWindow()->AddRenderer(m_renderer);

	counter = 0;
	functionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
}

void SceneWidget::addDataSet(vtkSmartPointer<vtkDataSet> dataSet)
{
    // Actor
    /*vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

    // Mapper
    vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputData(dataSet);
    actor->SetMapper(mapper);

    m_renderer->AddActor(actor);
    m_renderer->ResetCamera(dataSet->GetBounds());

    renderWindow()->Render();
	*/
	vtkNew<vtkPoints> points;


	points->InsertPoint(0, 1, counter, 0);
	points->InsertPoint(1, 2, 0, 0);
	points->InsertPoint(2, 3, 1, 0);
	points->InsertPoint(3, 4, 1, 0);
	points->InsertPoint(4, 5, 0, 0);
	points->InsertPoint(5, 6, 0, 0);

	// Fit a spline to the points
	vtkNew<vtkParametricSpline> spline;
	spline->SetPoints(points);
	// vtkNew<vtkParametricFunctionSource> functionSource;
	functionSource->SetParametricFunction(spline);
	functionSource->SetUResolution(10 * points->GetNumberOfPoints());
	functionSource->Update();
		
	// Create the tubes

	if (counter < 1)
	{
		// Add the scalars to the polydata
		auto tubePolyData = functionSource->GetOutput();

		vtkNew<vtkTubeFilter> tuber;
		tuber->SetInputData(tubePolyData);
		tuber->SetNumberOfSides(20);
		tuber->SetRadius(0.2);

		//--------------
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

	renderWindow()->Render();
	counter++;
	
}

void SceneWidget::removeDataSet()
{
    //vtkActor* actor = m_renderer->GetActors()->GetLastActor();
    //if (actor != nullptr) {
    //    m_renderer->RemoveActor(actor);
    //}

    renderWindow()->Render();
}

void SceneWidget::zoomToExtent()
{
    // Zoom to extent of last added actor
    vtkSmartPointer<vtkActor> actor = m_renderer->GetActors()->GetLastActor();
    if (actor != nullptr) {
        m_renderer->ResetCamera(actor->GetBounds());
    }

    renderWindow()->Render();
}
