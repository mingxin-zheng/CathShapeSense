#include "dataset.h"
#include <fstream>
#include <sstream>

void Dataset::setDatasetPath(std::string fileName)
{
    m_DatasetPath = fileName;
}

bool Dataset::init()
{
	std::ifstream file(m_DatasetPath);
	std::string line;
	int numFrames;
	const int N = 8;

	// read the table-format number into a 2D vector
	for (numFrames = 0; std::getline(file, line); numFrames++)
	{
		std::stringstream ss(line);
		std::vector<double> lineData;
		for (double pos; ss >> pos; )
		{
			lineData.push_back(pos);
			if (ss.peek() == ',')
			{
				ss.ignore();
			}
		}
		if (lineData.size() != N * 3)
		{
			return false;
		}
		m_Tracker.push_back(lineData);
	}
	
	m_NumFrames = numFrames;

	file.close();

	return true;
}

void Dataset::getNextFrame(std::vector<double>& frameData)
{
    frameData = m_Tracker[m_CurrentFrameIndex];

    m_CurrentFrameIndex ++;

    if (m_CurrentFrameIndex >= m_NumFrames)
    {
        m_CurrentFrameIndex = 0;
    }
}

void Dataset::clearAll()
{
	m_Tracker.clear();
	return;
}