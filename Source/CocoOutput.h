#pragma once
#include "IOutput.h"
#include <string>
#include <vector>
#include "Annotation.h"

class CocoOutput : public IOutput
{
public:

	// Inherited via IOutput
	virtual void ReciveResult(const std::vector<cv::Mat>& output_aoi, const std::vector<std::vector<Annotation>>& annotations, const std::string image_source) override;
private:
	void SaveAnnotation(std::string image_filename, const cv::Mat& image, const std::vector<Annotation>& annotations);
};