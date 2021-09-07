#define _DISABLE_EXTENDED_ALIGNED_STORAGE
#define NOGDI

#include "backEnd.h"
#include <ceres/ceres.h>
#include <chrono>

struct CATH_FITTING_COST
{
	CATH_FITTING_COST(	double x0, double y0, double z0, double mx0, double my0, double mz0, 
						double x7, double y7, double z7, double mx7, double my7, double mz7, 
						int N, int funcID) :
						_x0(x0), _y0(y0), _z0(z0), _mx0(mx0), _my0(my0), _mz0(mz0), 
						_x7(x7), _y7(y7), _z7(z7), _mx7(mx7), _my7(my7), _mz7(mz7), 
						_N(N), _funcID(funcID) {}
	// residual/cost compuation 
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

BackEnd::BackEnd(int numPointSimualation, double cathLength)
	: m_NumPointSimualation(numPointSimualation)
	, m_CatheterLengthInMM(cathLength)
{
	lastOptimalPoints = new double[m_NumPointSimualation * 3];

	// custom way to give the initial value to Ceres. Make sure non-zero gradient between all the points
	for (int i = 0; i < m_NumPointSimualation * 3; i++)
	{
		lastOptimalPoints[i] = 0.1 + 0.04*i;
	}

    m_BackEndRunning.store(true);
    m_BackEndThread = std::thread(std::bind(&BackEnd::BackEndLoop, this));
}

void BackEnd::UpdateAllPoints()
{
    std::unique_lock<std::mutex> lock(m_DataMutex);
	m_PointUpdate.notify_one();
}

void BackEnd::Stop()
{
    m_BackEndRunning.store(false);
    m_PointUpdate.notify_one();
    m_BackEndThread.join();
}

void BackEnd::BackEndLoop()
{
    while (m_BackEndRunning.load())
    {
        std::unique_lock<std::mutex> lock(m_DataMutex);
        m_PointUpdate.wait(lock);

        // Optimize in the back end
		std::vector<double> tracker1 = m_CathPts->GetTracker1();
		std::vector<double> tracker2 = m_CathPts->GetTracker2();
		std::vector<double> optimalResult;
		optimalResult = Optimize(tracker1, tracker2);
		m_CathPts->SetAllPoints(optimalResult);
    }
}

std::vector<double> BackEnd::Optimize(std::vector<double> tracker1, std::vector<double> tracker2)
{

	double xMean = 0.5*(tracker1[0] + tracker2[0]);
	double yMean = 0.5*(tracker1[1] + tracker2[1]);
	double zMean = 0.5*(tracker1[2] + tracker2[2]);

	// location(x,y,z) of tracker 1 and 2
	double x1, y1, z1, x2, y2, z2;
	x1 = (tracker1[0] - xMean) / m_CatheterLengthInMM;
	y1 = (tracker1[1] - yMean) / m_CatheterLengthInMM;
	z1 = (tracker1[2] - zMean) / m_CatheterLengthInMM;
	x2 = (tracker2[0] - xMean) / m_CatheterLengthInMM;
	y2 = (tracker2[1] - yMean) / m_CatheterLengthInMM;
	z2 = (tracker2[2] - zMean) / m_CatheterLengthInMM;

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

	for (int i = 0; i <= 2 * m_NumPointSimualation + 6; i++)
	{
		if (m_NumPointSimualation == (int) SimulationDensity::LOW)
		{
			problem.AddResidualBlock(
				new ceres::AutoDiffCostFunction<CATH_FITTING_COST, 1, ((int) SimulationDensity::LOW) * 3>
					(new CATH_FITTING_COST(	x1, y1, z1, mx1, my1, mz1, 
												x2, y2, z2, mx2, my2, mz2, 
												m_NumPointSimualation, i)
					), nullptr, lastOptimalPoints);
		}
		else if (m_NumPointSimualation == (int)SimulationDensity::MEDIUM)
		{
			problem.AddResidualBlock(
				new ceres::AutoDiffCostFunction<CATH_FITTING_COST, 1, ((int)SimulationDensity::MEDIUM) * 3>
					(new CATH_FITTING_COST(	x1, y1, z1, mx1, my1, mz1, 
												x2, y2, z2, mx2, my2, mz2, 
												m_NumPointSimualation, i)
					), nullptr, lastOptimalPoints);
		}
		else if (m_NumPointSimualation == (int)SimulationDensity::HIGH)
		{
			problem.AddResidualBlock(
				new ceres::AutoDiffCostFunction<CATH_FITTING_COST, 1, ((int)SimulationDensity::HIGH) * 3>
					(new CATH_FITTING_COST(	x1, y1, z1, mx1, my1, mz1, 
												x2, y2, z2, mx2, my2, mz2, 
												m_NumPointSimualation, i)
					), nullptr, lastOptimalPoints);
		}
	}

	ceres::Solver::Options options;
	options.linear_solver_type = ceres::DENSE_QR;  // Use Dense_QR to solve
	options.minimizer_progress_to_stdout = false;  // no std outputs

	ceres::Solver::Summary summary;                // summary of info
	ceres::Solve(options, &problem, &summary);

	//cout << counter << endl;
	std::vector<double> optimalFit;

	for (int i = 0; i < m_NumPointSimualation; i++)
	{
		optimalFit.push_back(lastOptimalPoints[i * 3 + 0] * m_CatheterLengthInMM + xMean);
		optimalFit.push_back(lastOptimalPoints[i * 3 + 1] * m_CatheterLengthInMM + yMean);
		optimalFit.push_back(lastOptimalPoints[i * 3 + 2] * m_CatheterLengthInMM + zMean);
	}

	return optimalFit;
}