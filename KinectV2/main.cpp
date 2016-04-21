#include "KinectColorStream.cpp"
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat image = Mat(360, 640, CV_8UC4);

KinectColorStream ks;

int main(int argc, char** argv){
	
	if (FAILED(ks.kinectSensorIntialise())) {
		std::cout << "No device found" << std::endl;
	}

	while (1) {
		ks.getRGBFeedOptimised(image);
		imshow("VideoStream", image);
		waitKey(1);
	}
}