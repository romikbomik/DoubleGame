#pragma once
#include "CameraInput.h"
#include <opencv2/core.hpp>

CameraInput::CameraInput()
{
	cap = cv::VideoCapture(0);
}



void CameraInput::CaptureImage(cv::Mat& output_mat)
{
	if (cap.isOpened())
	{
		cap >> output_mat;
	}
	else 
	{
		output_mat = cv::Mat::zeros(480, 640, CV_8UC3);
	}
}

std::string CameraInput::GetName()
{
	return std::string("CameraInput");
}

CameraInput::~CameraInput()
{
	cap.release();
}

