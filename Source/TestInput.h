#pragma once

#include "IInput.h"

class TestInput : public IInput
{
public:
	TestInput();
	~TestInput();
	virtual void CaptureImage(cv::Mat& output_mat) override;
	virtual std::string GetName() override;
	void Next();
private:
	std::string test_name;
	int number;
};