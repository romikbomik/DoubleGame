#pragma once
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include "Annotation.h"

class ImagePreprocessor
{
public:
	static void ProcessAreasOfInterest(std::vector<cv::Mat>& target_areas, std::vector<cv::Mat>& output_aoi, std::vector <std::vector<Annotation>>& output_annotation);
	static void FindAreasOfInterest(cv::Mat& input_image, std::vector<cv::Mat>& output_aoi);
	static void RemoveBackground(cv::Mat& input_image, std::vector<cv::Mat>& output_aoi);
	static void Segmentation(cv::Mat& input_image, cv::Mat& output_image, const int k_means, cv::Mat& centers);
};