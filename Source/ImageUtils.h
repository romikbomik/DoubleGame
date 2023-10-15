#pragma once
#include <opencv2/imgproc/imgproc.hpp>


class ImageUtils
{
public:
	static void Blur(cv::Mat& input_image, cv::Mat& blured);
	static void GetHistogramImage(cv::Mat& input_image, cv::Mat& histogram);
	static void GetHistogramImageGray(cv::Mat& input_image, cv::Mat& histogram);
	static void Pooling(cv::Mat& input_image, cv::Mat& pooled, const int factor = 2);
};