#pragma once
#include "CameraInput.h"
#include <opencv2/core.hpp>

CameraInput::CameraInput()
{
	cap = cv::VideoCapture(0);
}



void CameraInput::CaptureImage(cv::Mat& OutputMat)
{
	if (cap.isOpened())
	{
		cap >> OutputMat;
	}
	else 
	{
		OutputMat = cv::Mat::zeros(480, 640, CV_8UC3);
	}
}

CameraInput::~CameraInput()
{
	cap.release();
}

