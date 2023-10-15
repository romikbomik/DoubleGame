
#include "ImageUtils.h"

void ImageUtils::Blur(cv::Mat& input_image, cv::Mat& blured)
{
    cv::GaussianBlur(input_image, blured, cv::Size(3, 3), 0);
}

void ImageUtils::GetHistogramImage(cv::Mat& input_image, cv::Mat& histogram)
{

    std::vector<cv::Mat> bgr_planes;
    cv::split(input_image, bgr_planes);
    int hist_size = 256;
    float range[] = { 0, 256 }; //the upper boundary is exclusive
    const float* hist_range[] = { range };
    bool uniform = true, accumulate = false;
    cv::Mat b_hist, g_hist, r_hist;
    cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &hist_size, hist_range, uniform, accumulate);
    cv::calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &hist_size, hist_range, uniform, accumulate);
    cv::calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &hist_size, hist_range, uniform, accumulate);
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double)hist_w / hist_size);
    histogram = cv::Mat(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::normalize(b_hist, b_hist, 0, histogram.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(g_hist, g_hist, 0, histogram.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(r_hist, r_hist, 0, histogram.rows, cv::NORM_MINMAX, -1, cv::Mat());
    for (int i = 1; i < hist_size; i++)
    {
        cv::line(histogram, cv::Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
            cv::Scalar(255, 0, 0), 2, 8, 0);
        cv::line(histogram, cv::Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
            cv::Scalar(0, 255, 0), 2, 8, 0);
        cv::line(histogram, cv::Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
            cv::Scalar(0, 0, 255), 2, 8, 0);
    }
}

void ImageUtils::GetHistogramImageGray(cv::Mat& input_image, cv::Mat& histogram)
{
    cv::Mat grayscale_image;
    cv::cvtColor(input_image, grayscale_image, cv::COLOR_BGR2GRAY);

    // Compute the histogram
    int hist_size = 256; // Number of bins
    float range[] = { 0, 256 }; // Range of pixel values
    const float* hist_range = { range };
    bool uniform = true, accumulate = false;
    cv::calcHist(&grayscale_image, 1, nullptr, cv::Mat(), histogram, 1, &hist_size, &hist_range, uniform, accumulate);

    // Normalize the histogram for display
    cv::normalize(histogram, histogram, 0, grayscale_image.rows, cv::NORM_MINMAX, -1, cv::Mat());

    // Create an image to display the histogram
    const int hist_width = 512, hist_height = 400;
    cv::Mat hist_image(hist_height, hist_width, CV_8UC3, cv::Scalar(255, 255, 255));

    // Draw the histogram
    for (int i = 1; i < hist_size; ++i) {
        cv::line(hist_image, cv::Point(i - 1, hist_width - cvRound(histogram.at<float>(i - 1))),
            cv::Point(i, hist_height - cvRound(histogram.at<float>(i))),
            cv::Scalar(0, 0, 0), 2, 8, 0);
    }
}

void ImageUtils::Pooling(cv::Mat& input_image, cv::Mat& pooled_image, const int factor)
{
    // Define the pooling factor (downscale by 2x)
    int pooling_factor = factor;

    // Create a new image for the pooled result
    pooled_image = cv::Mat(input_image.rows / pooling_factor, input_image.cols / pooling_factor, input_image.type());

    // Perform max pooling
    for (int i = 0; i < pooled_image.rows; ++i) {
        for (int j = 0; j < pooled_image.cols; ++j) {
            cv::Rect roi(j * pooling_factor, i * pooling_factor, pooling_factor, pooling_factor);
            cv::Mat roi_image = input_image(roi);
            cv::MatIterator_<cv::Vec3b> it, end;
            cv::Vec3b max_value = { 0, 0, 0 };

            for (it = roi_image.begin<cv::Vec3b>(), end = roi_image.end<cv::Vec3b>(); it != end; ++it) {
                max_value[0] = std::max(max_value[0], (*it)[0]);
                max_value[1] = std::max(max_value[1], (*it)[1]);
                max_value[2] = std::max(max_value[2], (*it)[2]);
            }

            pooled_image.at<cv::Vec3b>(i, j) = max_value;
        }
    }
}