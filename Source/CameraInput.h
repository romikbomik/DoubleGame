#pragma once
#include "IInput.h";
#include <opencv2/videoio.hpp>

class CameraInput : public IInput
{
public:
	CameraInput();
	~CameraInput();
	virtual void CaptureImage(cv::Mat& OutputMat) override;
private:
	cv::VideoCapture cap;
};