#pragma once
#include <Windows.h>
#include <NuiApi.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

class KinectV1Sensor

{
    public:
    HANDLE colorStream = (INVALID_HANDLE_VALUE);
    HANDLE m_hNextColorFrameEvent = (INVALID_HANDLE_VALUE);
    
    HANDLE depthStream = (INVALID_HANDLE_VALUE);
    HANDLE m_hNextDepthFrameEvent = (INVALID_HANDLE_VALUE);
    
    INuiSensor* sensor;
    
    static const int eventCount = 1;
    HANDLE hEvents[eventCount];
    
    
    	KinectV1Sensor(){};
    
    HRESULT CreateFirstConnected()
    
    {
        
        INuiSensor * pNuiSensor;
        HRESULT hr;
        
        int iSensorCount = 0;
        //    Gets the number of kinect devices connected.
        hr = NuiGetSensorCount(&iSensorCount);
        if (FAILED(hr))
        
        {
            
            return hr;
            
        }
        
        
        // Look at each Kinect sensor
        for (int i = 0; i < iSensorCount; ++i)
        
        {
            
            // Create the sensor so we can check status, if we can't create it, move on to the next
            hr = NuiCreateSensorByIndex(i, &pNuiSensor);
            if (FAILED(hr))
            
            {
                
                continue;
   
            }
            
            
            // Get the status of the sensor, and if connected, then we can initialize it
            hr = pNuiSensor->NuiStatus();
            if (S_OK == hr)
            
            {
                
                sensor = pNuiSensor;
                break;
                
            }
            
            
            // This sensor wasn't OK, so release it since we're not using it
            pNuiSensor->Release();
            
        }
        
        
        if (NULL != sensor)
        
        {
            
            // Initialize the Kinect and specify that we'll be using color and depth.
            hr = sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR);
            if (SUCCEEDED(hr))
            
            {
                
                // Create an event that will be signaled when color data is available.
                m_hNextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                
                // Open a color image stream to receive color frames.
                hr = sensor->NuiImageStreamOpen(
                NUI_IMAGE_TYPE_COLOR,
                NUI_IMAGE_RESOLUTION_640x480,
                0,
                2,    //    frame limit should be set to 2 as states my Microsoft docs.
                m_hNextColorFrameEvent,
                &colorStream);
                
                // Create an event that will be signaled when depth data is available.
                m_hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                
                //    Open a depth stream to receive depth frames.
                hr = sensor->NuiImageStreamOpen(
                NUI_IMAGE_TYPE_DEPTH,
                NUI_IMAGE_RESOLUTION_640x480,
                0,
                2,    //    frame limit should be set to 2 as states my Microsoft docs.
                m_hNextDepthFrameEvent,
                &depthStream);
                
            }
            
            
        }
        
        return hr;
        
    }
    
    
    /// <summary>
    /// Obtains RGAB frame data from kinect and transfer it to opencv mat then display opencv mat.
    /// </summary>
    /// <returns>indicates success or failure</returns>
    void getRGBFrame(cv::Mat* test)
    {
		
        
        //    Frame to hold RGAB data returened from kinect sensor.
        NUI_IMAGE_FRAME imageFrame;
        NUI_LOCKED_RECT lockedRect;
        
        if (sensor->NuiImageStreamGetNextFrame(colorStream, 0, &imageFrame) < 0)
        {
            return;
        }
        
        
        INuiFrameTexture* texture = imageFrame.pFrameTexture;
        //    must lock texture to prevent updaing of image before copy is complete.
        texture->LockRect(0, &lockedRect, NULL, 0);
        
        //    If pitch  == 0 then its an empty frame.
        if (lockedRect.Pitch != 0)
        {
            BYTE* upperLeftCorner = (BYTE*)lockedRect.pBits;
            BYTE* pointerToTheByteBeingRead = upperLeftCorner;
            
            // copy bytes from locked texture to opencv mat channels.
            for (int i = 0; i < 480; i++)
            
            {
                

                Vec3b *pointerToRow = &(*test->ptr<Vec3b>(i));
                for (int j = 0; j < 640; j++)
                
                {
                    
                    unsigned char r = *pointerToTheByteBeingRead;
                    pointerToTheByteBeingRead += 1;
                    unsigned char g = *pointerToTheByteBeingRead;
                    pointerToTheByteBeingRead += 1;
                    unsigned char b = *pointerToTheByteBeingRead;
                    pointerToTheByteBeingRead += 2; //So to skip the alpha channel
                    pointerToRow[j] = Vec3b(r, g, b);
                    
                }
                
                
            }
            
            //imshow("ColorStream", test);
           // waitKey(1);
            
        }
        
        texture->UnlockRect(0);
        // Release stream so it can be updated.
        sensor->NuiImageStreamReleaseFrame(colorStream, &imageFrame);
    }
    
    
    
    /// <summary>
    /// Obtains depth frame data from kinect and transfer it to opencv mat then display opencv mat.
    /// </summary>
    /// <returns>void</returns>
    void getDepthFrame(cv::Mat* test)
    
    {
        
        NUI_IMAGE_FRAME imageFrame;
        NUI_LOCKED_RECT lockedRect;
        if (sensor->NuiImageStreamGetNextFrame(depthStream, 0, &imageFrame) < 0)
        {
            return;
        }
        
        INuiFrameTexture* texture = imageFrame.pFrameTexture;
        texture->LockRect(0, &lockedRect, NULL, 0);
        
        // Copy image information into Mat. Each pixel in depth frame obtained by kinect is
        //    represented by 1 USHORT, 16 bits wide grayscale.
        if (lockedRect.Pitch != 0)
        
        {
            
            BYTE* upperLeftCorner = (BYTE*)lockedRect.pBits;
            BYTE* pointerToTheByteBeingRead = upperLeftCorner;
            BYTE* pBuffer = lockedRect.pBits;
            BYTE* m_pDepthBuffer = new BYTE[lockedRect.size];
            USHORT* pBufferRun = reinterpret_cast<USHORT*>(m_pDepthBuffer);
            memcpy_s(m_pDepthBuffer, lockedRect.size, pBuffer, lockedRect.size);
            for (UINT y = 0; y < 480; ++y)
            
            {
                
                // Get row pointer for depth Mat
                USHORT* pDepthRow = &(*test->ptr<USHORT>(y));
                for (UINT x = 0; x < 640; ++x)
                
                {
                    
                    pDepthRow[x] = pBufferRun[y * 640 + x];
                    
                }
                
                
            }      
        }
        
        texture->UnlockRect(0);
        // Release stream so it can be updated.
        sensor->NuiImageStreamReleaseFrame(depthStream, &imageFrame);
        
    }
	

};