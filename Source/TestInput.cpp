#pragma once
#include "TestInput.h";
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>

void TestInput::CaptureImage(cv::Mat& OutputMat)
{
	try {
		OutputMat = cv::imread("TestInput2.jpg");
	}
	catch (cv::Exception e)
	{
		OutputMat = cv::Mat::zeros(480, 640, CV_8UC3);
	}

}