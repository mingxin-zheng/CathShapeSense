#define _DISABLE_EXTENDED_ALIGNED_STORAGE
#define NOGDI

#include "frontEnd.h"

#include <ceres/ceres.h>

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

// CURVE_FITTING_COST_1_COMMENT
struct CURVE_FITTING_COST_1
{
	CURVE_FITTING_COST_1(double x0, double y0, double z0, double mx0, double my0, double mz0, double x7, double y7, double z7, double mx7, double my7, double mz7, int N, int funcID) :
		_x0(x0), _y0(y0), _z0(z0), _mx0(mx0), _my0(my0), _mz0(mz0), _x7(x7), _y7(y7), _z7(z7), _mx7(mx7), _my7(my7), _mz7(mz7), _N(N), _funcID(funcID) {}
	// 残差的计算
	template <typename T> bool operator() (const T* const a, T* residual) const
	{
		double L = 1.0 / (_N + 1);
		if (_funcID == 0) { residual[0] = a[0] - T(_x0) - L * T(_mx0); }
		else if (_funcID == 1) { residual[0] = a[1] - T(_y0) - L * T(_my0); }
		else if (_funcID == 2) { residual[0] = a[2] - T(_z0) - L * T(_mz0); }
		else if (_funcID == 3) { residual[0] = T(_x7) - a[_N * 3 - 3] - L * T(_mx7); }
		else if (_funcID == 4) { residual[0] = T(_y7) - a[_N * 3 - 2] - L * T(_my7); }
		else if (_funcID == 5) { residual[0] = T(_z7) - a[_N * 3 - 1] - L * T(_mz7); }
		else if (_funcID == 6)
		{
			residual[0] = ceres::pow(a[0] - T(_x0), 2) +
				ceres::pow(a[1] - T(_y0), 2) +
				ceres::pow(a[2] - T(_z0), 2) - L * L;
		}
		else if (_funcID == 7)
		{
			residual[0] = ceres::pow(a[_N * 3 - 3] - T(_x7), 2) +
				ceres::pow(a[_N * 3 - 2] - T(_y7), 2) +
				ceres::pow(a[_N * 3 - 1] - T(_z7), 2) - L * L;
		}
		else if ((_funcID >= 8) && (_funcID < _N + 7))
		{
			int i = _funcID - 8;
			residual[0] = ceres::pow(a[(i + 1) * 3 + 0] - a[i * 3 + 0], 2) +
				ceres::pow(a[(i + 1) * 3 + 1] - a[i * 3 + 1], 2) +
				ceres::pow(a[(i + 1) * 3 + 2] - a[i * 3 + 2], 2) - L * L;
		}
		else if (_funcID == _N + 7)
		{
			residual[0] = ceres::pow(a[3] + T(_x0) - 2.0*a[0], 2) +
				ceres::pow(a[4] + T(_y0) - 2.0*a[1], 2) +
				ceres::pow(a[5] + T(_z0) - 2.0*a[2], 2);
		}
		else if (_funcID == _N + 8)
		{
			residual[0] = ceres::pow(a[_N * 3 - 6] + T(_x7) - 2.0*a[_N * 3 - 3], 2) +
				ceres::pow(a[_N * 3 - 5] + T(_y7) - 2.0*a[_N * 3 - 2], 2) +
				ceres::pow(a[_N * 3 - 4] + T(_z7) - 2.0*a[_N * 3 - 1], 2);
		}
		else if ((_funcID > _N + 8) && (_funcID <= 2 * _N + 6))
		{
			int i = _funcID - (_N + 8); // start from 1
			residual[0] = ceres::pow(a[(i + 1) * 3 + 0] + a[(i - 1) * 3 + 0] - 2.0*a[i * 3 + 0], 2) +
				ceres::pow(a[(i + 1) * 3 + 1] + a[(i - 1) * 3 + 1] - 2.0*a[i * 3 + 1], 2) +
				ceres::pow(a[(i + 1) * 3 + 2] + a[(i - 1) * 3 + 2] - 2.0*a[i * 3 + 2], 2);
		}
		return true;
	}
	const double    _x0, _y0, _z0, _mx0, _my0, _mz0, _x7, _y7, _z7, _mx7, _my7, _mz7;
	const int _N, _funcID;
};

FrontEnd::FrontEnd(QWidget* parent)
    : QVTKOpenGLNativeWidget(parent)
	, m_NumPointSimualation(6)
	, lastOptimalPoints(NULL)
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

	if (lastOptimalPoints != NULL)
	{
		delete[] lastOptimalPoints;
		lastOptimalPoints = NULL;
	}
	
	lastOptimalPoints = new double[m_NumPointSimualation*3];

	for (int i = 0; i< m_NumPointSimualation *3; i++)
    {
        lastOptimalPoints[i] = 0.1 + 0.04*i;
    }
}


void FrontEnd::ClearAll()
{
	// note(mingxin): Only ONE actor in the scene
    vtkActor* actor = m_renderer->GetActors()->GetLastActor();
    if (actor != nullptr) {
       m_renderer->RemoveActor(actor);
    }

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

void FrontEnd::AddFrame(std::vector<double> tracker1, std::vector<double> tracker2)
{
	Solve(tracker1, tracker2);
	
	vtkNew<vtkPoints> points;
	
	for (int i = 0; i < m_NumPointSimualation + 2; i++)
	{
		int x = i * 3, y = 3 * i + 1, z = 3 * i + 2;
		points->InsertPoint(i,	m_OptimalFit[x] ,
								m_OptimalFit[y],
								m_OptimalFit[z]);
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

void FrontEnd::Solve(std::vector<double> tracker1, std::vector<double> tracker2)
{

	double xMean = 0.5*(tracker1[0] + tracker2[0]);
	double yMean = 0.5*(tracker1[1] + tracker2[1]);
	double zMean = 0.5*(tracker1[2] + tracker2[2]);
	
	// location(x,y,z) of tracker 1 and 2
	double x1, y1, z1, x2, y2, z2; 
	x1 = (tracker1[0] - xMean) / 100.0;
	y1 = (tracker1[1] - yMean) / 100.0;
	z1 = (tracker1[2] - zMean) / 100.0;
	x2 = (tracker2[0] - xMean) / 100.0;
	y2 = (tracker2[1] - yMean) / 100.0;
	z2 = (tracker2[2] - zMean) / 100.0;

	// tagent vector (mx, my, mz) of tracker 1 and 2
	double mx1, my1, mz1, mx2, my2, mz2;
	mx1 = tracker1[3];
	mx2 = tracker2[3];

	my1 = tracker1[4];
	my2 = tracker2[4];

	mz1 = tracker1[5];
	mz2 = tracker2[5];

	int functionID = 0; // each functionID relates to a dedicated cost function
	ceres::Problem problem;

	for (int i = 0; i <= 2*m_NumPointSimualation + 6; i ++)
	{
		problem.AddResidualBlock (new ceres::AutoDiffCostFunction<CURVE_FITTING_COST_1, 1, 6 *3> (
			new CURVE_FITTING_COST_1(x1, y1, z1, mx1, my1, mz1, x2, y2, z2, mx2, my2, mz2, 6, i)), nullptr, lastOptimalPoints);
	}

	ceres::Solver::Options options;
	options.linear_solver_type = ceres::DENSE_QR;  // Use Dense_QR to solve
	options.minimizer_progress_to_stdout = false;  // no std outputs

	ceres::Solver::Summary summary;                // summary of info
	ceres::Solve ( options, &problem, &summary );

	//cout << counter << endl;
	m_OptimalFit.clear();

	m_OptimalFit.push_back(x1 * 100 + xMean);
	m_OptimalFit.push_back(y1 * 100 + yMean);
	m_OptimalFit.push_back(z1 * 100 + zMean);

	for (int i = 0; i < m_NumPointSimualation; i++)
	{
		m_OptimalFit.push_back(lastOptimalPoints[i * 3 + 0] * 100 + xMean);
		m_OptimalFit.push_back(lastOptimalPoints[i * 3 + 1] * 100 + yMean);
		m_OptimalFit.push_back(lastOptimalPoints[i * 3 + 2] * 100 + zMean);
	}
	
	m_OptimalFit.push_back(x2 * 100 + xMean);
	m_OptimalFit.push_back(y2 * 100 + yMean);
	m_OptimalFit.push_back(z2 * 100 + zMean);
}