
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
	IColorFrameSource* pColorSource;					// Color Source
	IColorFrameReader* pColorReader;					// Color Reader
	IFrameDescription* pColorFrameDescription;			// Color Frame Description
	IColorFrame* pColorFrame = nullptr;					// Color Frame

	IDepthFrameSource* pDepthSource;					// Depth Source
	IDepthFrameReader* pDepthReader;					// Depth Reader
	IFrameDescription* pDepthFrameDescription;			// Depth Frame Description
	IDepthFrame* pDepthFrame = nullptr;					// Depth Frame


	unsigned int bufferSize = 1920 * 1080 * 4 * sizeof(unsigned char);
	unsigned int depthFrameSize = 512 * 424 * 2 * sizeof(USHORT);
	Mat bufferMat = Mat(1080, 1920, CV_8UC4);
	Mat colorMat = Mat(480, 640, CV_8UC4);
	Mat currentFrame;

	UINT capacity = 0;
	USHORT* buffer;

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
				if (SUCCEEDED(pColorSource->get_FrameDescription(&pColorFrameDescription))) {
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
				if (SUCCEEDED(pColorSource->get_FrameDescription(&pColorFrameDescription))) {
					pColorFrame = nullptr;
					hResult = pColorReader->AcquireLatestFrame(&pColorFrame);
					if (SUCCEEDED(hResult)) {
						unsigned int bytes = 0;
						int hei = 0;
						pColorFrameDescription->get_BytesPerPixel(&bytes);
						pColorFrameDescription->get_Height(&hei);

						//std::cout << bytes << std::endl;
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

	void getDepthFrame(cv::Mat& DepthMat)
	{
		if (SUCCEEDED(pSensor->get_DepthFrameSource(&pDepthSource))) {
			if (SUCCEEDED(pDepthSource->OpenReader(&pDepthReader))) {
				if (SUCCEEDED(pDepthSource->get_FrameDescription(&pDepthFrameDescription))) {
					pDepthFrame = nullptr;
					hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame);
					if (SUCCEEDED(hResult)) {
						pDepthFrame->AccessUnderlyingBuffer(&capacity, &buffer);
						const UINT16* pBufferEnd = buffer + (512 * 424);
						uchar* dat = DepthMat.data;
						while (buffer < pBufferEnd)
						{
							USHORT depth = *buffer;

							// To convert to a byte, we're discarding the most-significant
							// rather than least-significant bits.
							// We're preserving detail, although the intensity will "wrap."
							// Values outside the reliable depth range are mapped to 0 (black).

							// Note: Using conditionals in this loop could degrade performance.
							// Consider using a lookup table instead when writing production code.
							BYTE intensity = static_cast<BYTE>((depth >= 500) && (depth <= 65535) ? (depth % 256) : 0);
							*dat = intensity;
							dat++;
							*dat = intensity;
							dat++;
							*dat = intensity;
							++buffer;
							++dat;
						}
					}
					SafeRelease(pDepthFrame);
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
		hResult = pColorSource->get_FrameDescription(&pColorFrameDescription);
		if (FAILED(hResult)) {
			std::cerr << "Error : IColorFrameSource::get_FrameDescription()" << std::endl;
		}
		int width = 0;
		int height = 0;

		int widt = 640;
		int heigh = 480;

		pColorFrameDescription->get_Width(&width); // 1920
		pColorFrameDescription->get_Height(&height); // 1080
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
		SafeRelease(pColorFrameDescription);
		if (pSensor) {
			pSensor->Close();
		}
		SafeRelease(pSensor);
	}
};