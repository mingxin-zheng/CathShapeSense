#include "backEnd.h"

BackEnd::BackEnd()
{
    m_BackEndRunning.store(true);
    m_BackEndThread = std::thread(std::bind(&BackEnd::BackEndLoop, this));
}

void BackEnd::UpdateAllPoints()
{
    std::unique_lock<std::mutex> lock(m_DataMutex);
    m_BackEndThread = std::thread(std::bind(&BackEnd::BackEndLoop, this));
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

        /// 后端仅优化激活的Frames和Landmarks
        Optimize();
    }
}

void BackEnd::Optimize()
{
    
}