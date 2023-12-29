#pragma once

#include "Annotation.h"
#include "vector"
#include <opencv2/core/mat.hpp>

class IModel
{
public:
	virtual bool Init() = 0;
	virtual bool Predict(const std::vector<cv::Mat>& input_image, std::vector<std::vector<Annotation>>& predictions) = 0;
};