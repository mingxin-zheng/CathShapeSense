#ifndef BACKEND_H
#define BACKEND_H

#include "catheterPoints.h"
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

enum class SimulationDensity // because ceres/eigen template function can only take constant in compile, so we use three sets of values
{
	LOW = 6,
	MEDIUM = 10,
	HIGH = 14
};

class BackEnd{
    public:
        typedef std::shared_ptr<BackEnd> Ptr;
        /*!
        * \param[in] numPointSimulation: number of points to solve in the optimization problem. cathLength: length of the catheter, which is used as the contraint of the the problem
        */
        BackEnd(int numPointSimualation, double cathLength);

        //! use the data point in the latest frame and fit the catheter shape by the solving an optimaztion problem
        void UpdateAllPoints();

        //! stop the back end
        void Stop();

		//! Setter for Catheter points
        void SetCatheterPoints(CatheterPoints::Ptr cath) { m_CathPts = cath; };

    private:
        // multi-threading loop
        void BackEndLoop();

        /*!
		* \param[in] tracker1, tracker2: two vector (x, y, z, mx, my, mz) of the EM sensors 1 and 2.
		*/
        std::vector<double> Optimize(std::vector<double> tracker1, std::vector<double> tracker2);


		const int		m_NumPointSimualation;	// number of points used for simulation, not including the ends measured by sensor For example, if 6 points are used, in total there are 6+2 points
		const double	m_CatheterLengthInMM;	// length of the catheter (Unit: mm)
		double*			lastOptimalPoints;	// initial values for each iteration of the optimzation problem, which is the solution of last iteration, required by ceres-solver

        std::shared_ptr<CatheterPoints> m_CathPts;	// pointer to the catheter points object 

		// multi-threads
        std::thread				m_BackEndThread;	
        std::mutex				m_DataMutex;
        std::condition_variable m_PointUpdate;
        std::atomic<bool>		m_BackEndRunning;
};

#endif // BACKEND_H