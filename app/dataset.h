#ifndef DATASET_H
#define DATASET_H

/*!
    \class dataset
    \brief stores a list of tracker postions (pose information)
*/
#include <string>
#include <vector>

class Dataset {
	public:
		bool init();

        void setDatasetPath(std::string fileName);

		/*!
			
		*/
		void getNextFrame(std::vector<double>& frameData);

    public:
        std::string m_DatasetPath;
		int m_NumFrames = 0;
        int m_CurrentFrameIndex = 0;
        int m_FrameRate = 0;
        std::vector<std::vector<double>> m_Tracker;
	
};

#endif