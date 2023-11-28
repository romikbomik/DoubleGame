#pragma once
#include <opencv2/core/mat.hpp>
#include <vector>
#include <string>
#include "Annotation.h"


class IOutput
{
public:
	virtual void ReciveResult(const std::vector<cv::Mat>& output_aoi, const std::vector<std::vector<Annotation>>& annotations, const std::string image_source) = 0;
};