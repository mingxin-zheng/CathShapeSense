#include "dataSource.h"
#include <fstream>
#include <sstream>

DataSource::DataSource()
	: m_SourceFilePath("")
	, m_NumFrames(0)
	, m_CurrentFrameIndex(0) { }

DataSource::~DataSource()
{
	ClearAll();
}
bool DataSource::Init()
{
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
	const int N = 8;
	
	m_Tracker.clear();
	
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
	m_CurrentFrameIndex = 0;

	file.close();

	return true;
}

void DataSource::GetNextFrame(std::vector<double>& frameData)
{
    frameData = m_Tracker[m_CurrentFrameIndex];

    m_CurrentFrameIndex ++;

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