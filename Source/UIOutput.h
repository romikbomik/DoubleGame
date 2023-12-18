#pragma once
#include "IOutput.h"
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Annotation.h"


class UIOutput : public IOutput
{
public:

	~UIOutput();
	// Inherited via IOutput
	virtual void ReciveResult(const std::vector<cv::Mat>& output_aoi, const std::vector<std::vector<Annotation>>& annotations, const std::string image_source) override;
private:
	void Draw(std::string name, std::vector<cv::Mat>& input);
	bool AccumulateImageInfo(std::vector<cv::Mat>& input, int& height, int& weight, int& type);
	void DrawAnnotations(cv::Mat& img, const std::vector<Annotation>& annotations);
	void FindMatchingLabel(const std::vector<std::vector<Annotation>>& annotations);
	std::string matching_label;
	bool bActiveWindows;
};
