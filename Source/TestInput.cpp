#pragma once
#include "TestInput.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>

TestInput::TestInput() : 
	test_name("TestInput"), 
	number(1)
{

}


void TestInput::CaptureImage(cv::Mat& output_mat)
{
	try {
		output_mat = cv::imread(test_name + std::to_string(number) + ".jpg");
	}
	catch (cv::Exception e)
	{
		output_mat = cv::Mat::zeros(480, 640, CV_8UC3);
	}

}

std::string TestInput::GetName()
{
	return test_name + std::to_string(number);
}

void TestInput::Next()
{
	number++;
}
