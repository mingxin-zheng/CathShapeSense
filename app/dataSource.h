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
        //! Read a tracker/sensor 5-DOF data from a text file with the format of (x y z qw qx qy qz)
		bool Init();

        /*!
		* \param[out] frameData: the sensor location + orientation in the next frame
        */
		void GetNextFrame(std::vector<double>& frameData);
        
        //! Setter of the SourceFilePath
		void SetSourceFilePath(std::string filePath);

        //! reset the data source loop (make CurrentFrameIndex 0)
        void Reset();

        //! Clear the memory and reset
        void ClearAll(); 

    public:
		/*! Convert the 5-DOF data to vectors from postion + orientation
		* param[in]: XYZQuat, a 7-element vector (x y z qw qx qy qz)
		* param[out]: posVec, a 6-element vecotr (x, y, z, mx, my, mz). 
        *             x, y, and z are the location of sensor in 3D. 
        *             mx, my, mz is a vector representing the orientation of sensor
		*/
		void	XYZQuatToTransformation(std::vector<double> XYZQuat, std::vector<double> &posVec);

        std::string     m_SourceFilePath;			// file path to the data source (.txt)
		int             m_NumFrames = 0;			// total number of frames
        int             m_CurrentFrameIndex = 0;	// current index of the frame in the sequence

        std::vector<std::vector<double>>    m_Tracker; // 2D array of sensor location + orientation vector
};

#endif