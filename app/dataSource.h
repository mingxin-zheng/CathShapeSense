#ifndef DATASOURCE_H
#define DATASOURCE_H

/*!
    \class dataset
    \brief stores a list of tracker postions (pose information)
*/
#include <string>
#include <vector>

class DataSource {
	public:
        explicit DataSource();
        ~DataSource();
        /*!
			
		*/
		bool Init();

        /*!

        */
		void GetNextFrame(std::vector<double>& frameData);
        
        /*!

        */
		void SetSourceFilePath(std::string filePath);

        /*! reset the data source loop (make CurrentFrameIndex 0)*/
        void Reset();

        /*! Clear the memory and reset */
        void ClearAll(); 

    public:
		/*!

		*/
		void	XYZQuatToTransformation(std::vector<double> XYZQuat, std::vector<double> &posVec);

        std::string                         m_SourceFilePath;
		int                                 m_NumFrames = 0;
        int                                 m_CurrentFrameIndex = 0;
        std::vector<std::vector<double>>    m_Tracker;
};

#endif