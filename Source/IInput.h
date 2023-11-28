#pragma once
#include <opencv2/core/mat.hpp>

class IInput
{
public:
	virtual void CaptureImage(cv::Mat& output_mat) = 0;
	virtual std::string GetName() = 0;
};