# ifndef CATHETERPOINTS_H
# define CATHETERPOINTS_H

#include <vector>
#include <thread>
#include <mutex>

class CatheterPoints
{
    public:
    typedef std::shared_ptr<CatheterPoints> Ptr;

    void SetSensor1(std::vector<double> sensor1) {m_Sensor1 = sensor1;};
    void SetSensor2(std::vector<double> sensor2) {m_Sensor1 = sensor2;};

    std::vector<double> GetAllPoints()
    {
        std::unique_lock<std::mutex> lck(m_DataMutex);
        return m_AllPoints;
    };

    void ClearAll() {m_AllPoints.clear();};
    private:
    std::mutex  m_DataMutex;
    std::vector<double> m_Sensor1;
    std::vector<double> m_Sensor2;
    std::vector<double> m_AllPoints;
};

# endif // CATHETERPOINTS_H