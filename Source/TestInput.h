#pragma once

#include "IInput.h";

class TestInput : public IInput
{
public:
	virtual void CaptureImage(cv::Mat& OutputMat) override;
};