#ifndef BACKEND_H
#define BACKEND_H

#include "catheterPoints.h"
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

class BackEnd{
    public:
        typedef std::shared_ptr<BackEnd> Ptr;
        /*!
        * start the optimization thread
        */
        BackEnd();

        /*!
        *
        */
        void UpdateAllPoints();

        /*!
        *
        */
        void Stop();

        /*!
        *
        */
        void SetCatheterPoints(CatheterPoints::Ptr cath) { m_CathPts = cath; };


    
    private:
        // 
        void BackEndLoop();

        //
        std::vector<double> Optimize(std::vector<double> tracker1, std::vector<double> tracker2);

        std::shared_ptr<CatheterPoints> m_CathPts;
        std::thread m_BackEndThread;
        std::mutex m_DataMutex;

        std::condition_variable m_PointUpdate;
        std::atomic<bool>m_BackEndRunning ;

		double* lastOptimalPoints; // required by ceres-solver

		const int m_NumPointSimualation = 6;
};

#endif // BACKEND_H