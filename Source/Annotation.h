#pragma once
#include <opencv2/core/mat.hpp>
#include <string>

struct Annotation
{
	std::string name;
	cv::Rect bbox;
};