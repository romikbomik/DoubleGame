#pragma once
#include <opencv2/imgproc/imgproc.hpp>

class ImageUtils
{
public:
	static void Blur(cv::Mat& input_image, cv::Mat& blured);
	static void GetHistogramImage(cv::Mat& input_image, cv::Mat& histogram);
	static void GetHistogramImageGray(cv::Mat& input_image, cv::Mat& histogram);
	static void Pooling(cv::Mat& input_image, cv::Mat& pooled, const int factor = 2);
	static void ContrastStratch(cv::Mat& input_image, cv::Mat& contrast_stretched);
	static void LinearTrasform(cv::Mat& input_image, cv::Mat& contrast_stretched, const float alpha, const float beta);
	static void FilterOutInnerContours(std::vector<std::vector<cv::Point>>& contours, const int max_size = 0);
	static bool IsRectangleInside(const cv::Rect& inner_rect, const cv::Rect& outer_rect);
	static bool IsRectangleInside(const cv::Rect& inner_rect, const cv::Rect& outer_rect, const float threshold);
	static void EnlargeAOI(cv::Mat& input_image, cv::Rect& boundin_box, int padding);
};