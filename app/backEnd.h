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
        void Optimize();

        std::shared_ptr<CatheterPoints> m_CathPts;
        std::thread m_BackEndThread;
        std::mutex m_DataMutex;

        std::condition_variable m_PointUpdate;
        std::atomic<bool>m_BackEndRunning ;

};

#endif // BACKEND_H