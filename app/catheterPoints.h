# ifndef CATHETERPOINTS_H
# define CATHETERPOINTS_H

#include <vector>
#include <thread>
#include <mutex>

class CatheterPoints
{
    public:
    typedef std::shared_ptr<CatheterPoints> Ptr;

	// Setter for tracker1 (sensor1)
    void SetSensor1(std::vector<double> sensor1) {m_Sensor1 = sensor1;};

	// Setter for tracker2 (sensor2)
    void SetSensor2(std::vector<double> sensor2) {m_Sensor2 = sensor2;};

	// Setter for All points (intepolated points derived by optimization)
	void SetAllPoints(std::vector<double> allPoints) { m_AllPoints = allPoints; };

	// Thread-safe Getter for tracker 1 data
	std::vector<double> GetTracker1()
	{
		std::unique_lock<std::mutex> lck(m_DataMutex);
		return m_Sensor1;
	};
	
	// Thread-safe Getter for tracker 2 data
	std::vector<double> GetTracker2()
	{
		std::unique_lock<std::mutex> lck(m_DataMutex);
		return m_Sensor2;
	};

	// Thread-safe Getter for all (intepolated) point data
    std::vector<double> GetAllPoints()
    {
        std::unique_lock<std::mutex> lck(m_DataMutex);
        return m_AllPoints;
    };

    private:
    std::mutex			m_DataMutex;// mutex for data operation
    std::vector<double> m_Sensor1;	// a 6-element vector (x,y,z, mx, my, mz) for sensor 1
    std::vector<double> m_Sensor2;	// a 6-element vector (x,y,z, mx, my, mz) for sensor 2
    std::vector<double> m_AllPoints;// a N*3 vector (x1,y1,z1, ..., xN, yN, zN) for intepolated points on the catheter
};

# endif // CATHETERPOINTS_H