#include "KinectV1Sensor.cpp"

int main()
{
	KinectV1Sensor kinectSensor = KinectV1Sensor();
	if (SUCCEEDED(kinectSensor.CreateFirstConnected()))
	{
		Mat color = Mat(480, 640, CV_8UC3);
		Mat depth = Mat(480, 640, CV_16U);

		while (1)
		{
			MsgWaitForMultipleObjects(kinectSensor.eventCount, kinectSensor.hEvents, FALSE, INFINITE, QS_ALLINPUT);
			if (WAIT_OBJECT_0 == WaitForSingleObject(kinectSensor.m_hNextColorFrameEvent, 0))
			{
				kinectSensor.getRGBFrame(color);

			}
			if (WAIT_OBJECT_0 == WaitForSingleObject(kinectSensor.m_hNextDepthFrameEvent, 0))
			{
				kinectSensor.getDepthFrame(depth);
			}
			imshow("ColorStream", color);
			imshow("DepthStream", depth);
			waitKey(1);
		}

	}
}