#pragma once
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

class ImagePreprocessor
{
public:
	static void PreprocessImage(std::vector<cv::Mat>& InputImage, std::vector<cv::Mat>& OutputAOI);
	static void RemoveBackground(cv::Mat& img, std::vector<cv::Mat>& OutputAOI);
	static cv::Mat Segmentation(cv::Mat& img, const int kmeans, cv::Mat& centers);
};