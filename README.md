# KinectOpenCVModule

Kinect OpenCV Modules are used to obtain color and depth frame data from Kinect sensors and display them in cv::Mat.
Kinect V1 module supports both RGB and depth streams. Kinect V2 module supports only RGB streams. 

##Prerequisites:
  * Cmake version > 2.8 https://cmake.org/
  * OpenCV  http://opencv.org/
  * Kinect SDK v1.8 for Kinect for xbox 360  https://www.microsoft.com/en-us/download/details.aspx?id=40278
  * Kinect SDK v2.0 for Kinect for xbox one https://www.microsoft.com/en-us/download/details.aspx?id=44561
  
###Setting enviroment variables:
  The Following enviroment variables must be set correctly for cmake to build the project. Note that the two projects
  are seprate, one for each kinect device version.
  * KINECTSDK20_DIR
  * KINECTSDK10_DIR
  * OpenCV_DIR
  * OpenCV_INCLUDE_DIRS
  * OpenCV_LIBS
  
###To build either project:
  1. Start Cmake GUI.
  2. Choose source folder.
  3. Create seperate folder to contain build files.
  4. Configure then generate. Set the start up project to KinectV1Module OR KinectV2Module then run.
  
  
