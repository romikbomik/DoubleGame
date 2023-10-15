#pragma once
#include <opencv2/core/mat.hpp>

class IInput
{
public:
	virtual void CaptureImage(cv::Mat& OutputMat) = 0;
};