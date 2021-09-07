#include "dataSource.h"
#include <fstream>
#include <sstream>

#include <Eigen/Core>
#include <Eigen/Geometry>

DataSource::DataSource()
	: m_SourceFilePath("")
{
}

DataSource::~DataSource()
{
	ClearAll();
}

bool DataSource::Init()
{
	// check file existence
	if (m_SourceFilePath.length() == 0)
	{
		return false;
	}

	std::ifstream file(m_SourceFilePath);

	if (file.fail())
	{
		return false;
	}

	std::string line;
	int numFrames;
	const int N = 7;
	
	m_Tracker.clear();

	// read the table-format number into a 2D vector
	for (numFrames = 0; std::getline(file, line); numFrames++)
	{
		std::stringstream ss(line);
		std::vector<double> lineData;
		for (double pos; ss >> pos; )
		{
			lineData.push_back(pos);
			if (ss.peek() == ' ')
			{
				ss.ignore();
			}
		}
		if (lineData.size() != N)
		{
			return false;
		}

		std::vector<double> posVec;
		XYZQuatToTransformation(lineData, posVec);
		m_Tracker.push_back(posVec);
	}
	
	m_NumFrames = numFrames;
	m_CurrentFrameIndex = 0;

	file.close();

	return true;
}

void DataSource::XYZQuatToTransformation(std::vector<double> XYZQuat, std::vector<double> &posVec)
{
	// both ends + mid point of the EM sensor, Length = 1;
	Eigen::Vector4d v1(0, 0, -0.5, 1), v2(0, 0, 0, 1), v3(0, 0, 0.5, 1); 
	Eigen::Vector3d T(XYZQuat[0], XYZQuat[1], XYZQuat[2]);
	Eigen::Quaterniond q(XYZQuat[3], XYZQuat[4], XYZQuat[5], XYZQuat[6]);
	Eigen::Matrix3d R = q.toRotationMatrix();

	Eigen::Matrix4d Trans; // Transformation Matrix
	Trans.setIdentity(); 
	Trans.block<3, 3>(0, 0) = R;
	Trans.block<3, 1>(0, 3) = T;
	
	Eigen::Vector4d pV1 = Trans * v1;
	Eigen::Vector4d pV2 = Trans * v2;
	Eigen::Vector4d pV3 = Trans * v3;

	posVec.clear();
	posVec.push_back(pV2(0));				// x
	posVec.push_back(pV2(1));				// y
	posVec.push_back(pV2(2));				// z
	posVec.push_back(pV1(0) - pV3(0));		// tagent vector (mx)
	posVec.push_back(pV1(1) - pV3(1));		// tagent vector (my)
	posVec.push_back(pV1(2) - pV3(2));		// tagent vector (mz)

	return;
}

void DataSource::GetNextFrame(std::vector<double>& frameData)
{
    frameData = m_Tracker[m_CurrentFrameIndex];

    m_CurrentFrameIndex++;

    if (m_CurrentFrameIndex >= m_NumFrames)
    {
        m_CurrentFrameIndex = 0;
    }
}

void DataSource::SetSourceFilePath(std::string filePath)
{
	m_SourceFilePath = filePath;
	return;
}

void DataSource::Reset()
{
	m_CurrentFrameIndex = 0;
	return;
}

void DataSource::ClearAll()
{
	m_SourceFilePath = "";
	m_NumFrames = 0;
	m_CurrentFrameIndex = 0;
	m_Tracker.clear();
	return;
}