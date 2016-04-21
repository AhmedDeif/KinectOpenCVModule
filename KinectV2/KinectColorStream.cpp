
#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <Kinect.h>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

class KinectColorStream {

	IKinectSensor* pSensor;
	HRESULT hResult = S_OK;

	//	RGB Feed Parameters
	IColorFrameSource* pColorSource;			// Source
	IColorFrameReader* pColorReader;			// Reader
	IFrameDescription* pDescription;			// Description
	IColorFrame* pColorFrame = nullptr;			// Frame

	unsigned int bufferSize = 1920 * 1080 * 4 * sizeof(unsigned char);
	Mat bufferMat = Mat(1080,1920, CV_8UC4);
	Mat colorMat = Mat(480,640, CV_8UC4);
	Mat currentFrame;

public:
	template<class Interface>
	inline void SafeRelease(Interface *& pInterfaceToRelease)
	{
		if (pInterfaceToRelease != NULL) {
			pInterfaceToRelease->Release();
			pInterfaceToRelease = NULL;
		}
	}

	HRESULT kinectSensorIntialise() {

		HRESULT hResult = GetDefaultKinectSensor(&pSensor);
		if (FAILED(hResult)) {
			std::cerr << "Error : GetDefaultKinectSensor" << std::endl;
		}
		hResult = pSensor->Open();
		if (FAILED(hResult)) {
			std::cerr << "Error : IKinectSensor::Open()" << std::endl;
			return hResult;
		}
		return hResult;
	}

	Mat getRGBFeedOptimised() {
		Mat colorMat = Mat(480, 640, CV_8UC4);
		if (SUCCEEDED(pSensor->get_ColorFrameSource(&pColorSource))) {
			if (SUCCEEDED(pColorSource->OpenReader(&pColorReader))) {
				if (SUCCEEDED(pColorSource->get_FrameDescription(&pDescription))) {
					pColorFrame = nullptr;
					hResult = pColorReader->AcquireLatestFrame(&pColorFrame);
					if (SUCCEEDED(hResult)) {
						hResult = pColorFrame->CopyConvertedFrameDataToArray(bufferSize, reinterpret_cast<BYTE*>(bufferMat.data), ColorImageFormat::ColorImageFormat_Bgra);
						if (SUCCEEDED(hResult)) {
							resize(bufferMat, colorMat, colorMat.size(), 0, 0);
						}
					}
					SafeRelease(pColorFrame);
				}
			}
		}
		return colorMat;
	}

	void getRGBFeedOptimised(const cv::Mat& colorMat) {
		if (SUCCEEDED(pSensor->get_ColorFrameSource(&pColorSource))) {
			if (SUCCEEDED(pColorSource->OpenReader(&pColorReader))) {
				if (SUCCEEDED(pColorSource->get_FrameDescription(&pDescription))) {
					pColorFrame = nullptr;
					hResult = pColorReader->AcquireLatestFrame(&pColorFrame);
					if (SUCCEEDED(hResult)) {
						hResult = pColorFrame->CopyConvertedFrameDataToArray(bufferSize, reinterpret_cast<BYTE*>(bufferMat.data), ColorImageFormat::ColorImageFormat_Bgra);
						if (SUCCEEDED(hResult)) {
							resize(bufferMat, colorMat, colorMat.size(), 0, 0);
						}
					}
					SafeRelease(pColorFrame);
				}
			}
		}
	}

	Mat getRGBFeed() {
		// Source
		hResult = pSensor->get_ColorFrameSource(&pColorSource);
		if (FAILED(hResult)) {
			std::cerr << "Error : IKinectSensor::get_ColorFrameSource()" << std::endl;
		}

		// Reader
		hResult = pColorSource->OpenReader(&pColorReader);
		if (FAILED(hResult)) {
			std::cerr << "Error : IColorFrameSource::OpenReader()" << std::endl;
		}

		// Description
		hResult = pColorSource->get_FrameDescription(&pDescription);
		if (FAILED(hResult)) {
			std::cerr << "Error : IColorFrameSource::get_FrameDescription()" << std::endl;
		}
		int width = 0;
		int height = 0;

		int widt = 640;
		int heigh = 480;

		pDescription->get_Width(&width); // 1920
		pDescription->get_Height(&height); // 1080
		unsigned int bufferSize = width * height * 4 * sizeof(unsigned char);

		// Frame
		IColorFrame* pColorFrame = nullptr;
		hResult = pColorReader->AcquireLatestFrame(&pColorFrame);
		if (SUCCEEDED(hResult)) {
			hResult = pColorFrame->CopyConvertedFrameDataToArray(bufferSize, reinterpret_cast<BYTE*>(bufferMat.data), ColorImageFormat::ColorImageFormat_Bgra);
			if (SUCCEEDED(hResult)) {
				cv::resize(bufferMat, colorMat, cv::Size(), 0.5, 0.5);
				//	save image here
				// need to figure out the frame per second to take proper screen shots
			}
		}

		SafeRelease(pColorFrame);
		return colorMat;
	}

	void close() {
		SafeRelease(pColorSource);
		SafeRelease(pColorReader);
		SafeRelease(pDescription);
		if (pSensor) {
			pSensor->Close();
		}
		SafeRelease(pSensor);
	}
};