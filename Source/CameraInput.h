#pragma once
#include "IInput.h";
#include <opencv2/videoio.hpp>

class CameraInput : public IInput
{
public:
	CameraInput();
	~CameraInput();
	virtual void CaptureImage(cv::Mat& output_mat) override;
	virtual std::string GetName() override;
private:
	cv::VideoCapture cap;
};