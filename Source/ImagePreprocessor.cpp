#include "ImagePreprocessor.h"
#include "iostream"
#include <opencv2/opencv.hpp>
#include "ImageUtils.h"

#define BACKGROUND_PIXEL 0
#define AREA_OF_INTEREST_LOWER_LIMIT 0.01
#define AREA_OF_INTEREST_UPPER_LIMIT 0.9

void ImagePreprocessor::ProcessAreasOfInterest(std::vector<cv::Mat>& target_areas, std::vector<cv::Mat>& output_aoi, std::vector<cv::Rect>& output_annotation)
{
    try
    {
        output_aoi.clear();
        for (auto& image : target_areas)
        {
            const int lower_limit = static_cast<int>(image.cols * image.rows * AREA_OF_INTEREST_LOWER_LIMIT);
            const int upper_limit = static_cast<int>(image.cols * image.rows * AREA_OF_INTEREST_UPPER_LIMIT);

            //edge detection
            cv::Mat edges;
            cv::Canny(image, edges, 20, 50);

            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
            cv::Mat morphed;
            cv::morphologyEx(edges, morphed, cv::MORPH_CLOSE, kernel);
            //output_aoi.push_back(morphed);

            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(morphed, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
            ImageUtils::FilterOutInnerContours(contours, 10000);

            cv::Mat mask = cv::Mat::zeros(edges.size(), CV_8UC1);
            cv::drawContours(mask, contours, -1, cv::Scalar(255), cv::FILLED);


            cv::Mat result;  // Create a copy of the original image
            image.copyTo(result);
            cv::Scalar red(0, 0, 255);  // Scalar for the color red (BGR format)

            for (auto& contour : contours)
            {
                cv::Rect bounding_rect = cv::boundingRect(contour);
                int contour_area = bounding_rect.area();
                //if (true)
                if (contour_area >= lower_limit && contour_area <= upper_limit)
                {
                    cv::Mat AOI = image(bounding_rect).clone();
                    output_annotation.push_back(bounding_rect);
                    cv::rectangle(result, bounding_rect, red, 1);
                }
            }
             output_aoi.push_back(result);
        }
    }
    catch (cv::Exception& e)
    {
        std::cout << e.what() << std::endl;
    }

}

void ImagePreprocessor::FindAreasOfInterest(cv::Mat& input_image, std::vector<cv::Mat>& output_aoi)
{
    output_aoi.clear();
    ImagePreprocessor::RemoveBackground(input_image, output_aoi);
    for (auto& aoi : output_aoi)
    {
        cv::resize(aoi, aoi, cv::Size(250, 250));
    }
}

void ImagePreprocessor::RemoveBackground(cv::Mat& input_image, std::vector<cv::Mat>& output_aoi)
{
        const int lower_limit = static_cast<int>(input_image.cols * input_image.rows * AREA_OF_INTEREST_LOWER_LIMIT);

        output_aoi.clear();

        cv::Mat contrast_stretched;
        ImageUtils::ContrastStratch(input_image, contrast_stretched);

        cv::Mat blured;
        ImageUtils::Blur(contrast_stretched, blured);

        cv::Mat gray;
        cv::cvtColor(blured, gray, cv::COLOR_BGR2GRAY);
     

        cv::Mat threshed;
        // Apply Otsu's thresholding
        const float dynamic_threahold = cv::threshold(gray, threshed, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::Mat morphed;
        cv::morphologyEx(threshed, morphed, cv::MORPH_CLOSE, kernel);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(morphed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

       
        cv::Mat mask = cv::Mat::zeros(threshed.size(), CV_8UC1);
        cv::drawContours(mask, contours,  -1, cv::Scalar(255), cv::FILLED);
        cv::Mat masked_data = cv::Mat(input_image.rows, input_image.cols, input_image.depth(), cv::Scalar(BACKGROUND_PIXEL));
        blured.copyTo(masked_data, mask);

        for (auto& contour : contours)
        {
            int padding = 10;
            int contour_area = cv::contourArea(contour);
            cv::Rect bounding_rect = cv::boundingRect(contour);
            ImageUtils::EnlargeAOI(input_image, bounding_rect, 10);
            if(bounding_rect.area() >= lower_limit)
            {
                output_aoi.push_back(masked_data(bounding_rect).clone());
            }
        }
}

void ImagePreprocessor::Segmentation(cv::Mat& input_image, cv::Mat& output_image, const int k_means, cv::Mat& centers)
{
    cv::Mat pooled;
    ImageUtils::Pooling(input_image, pooled);
    cv::Mat blured;
    ImageUtils::Blur(pooled, blured);
    
    // Reshape the image into a 2D array of pixels (rows x cols, 3 channels)
    cv::Mat reshaped_image = blured.reshape(1, blured.rows * blured.cols);

    // Convert the reshaped image to a 32-bit floating-point type
    cv::Mat reshaped_image_float;
    reshaped_image.convertTo(reshaped_image_float, CV_32F);

    // Define the criteria for K-Means
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 100, 0.2);

    // Perform K-Means clustering
    cv::Mat labels(reshaped_image_float.rows, 1, CV_32S, cv::Scalar(0)); // Initialize labels to 0
    try {
        if (!centers.empty())
        {
            cv::kmeans(reshaped_image_float, k_means, labels, criteria, 3, cv::KMEANS_USE_INITIAL_LABELS, centers);
        }
        else
        {
            cv::kmeans(reshaped_image_float, k_means, labels, criteria, 3, cv::KMEANS_RANDOM_CENTERS, centers);
        }
    }
    catch (cv::Exception e)
    {
        std::cout << e.what()<< std::endl;
    }
    // Convert the labels back to 8-bit for visualization
    labels.convertTo(labels, CV_8U);
    // Create a color map for the clusters (you can customize these colors)
    std::vector<cv::Vec3b> colors;
    for (int i = 0; i < k_means; ++i) {
        // Define colors here (BGR format)
        cv::Vec3b color;
        if (i == 0)       color = cv::Vec3b(0, 0, 255);  // Red
        else if (i == 1)  color = cv::Vec3b(0, 255, 0);  // Green
        else if (i == 2)  color = cv::Vec3b(255, 0, 0);  // Blue
        // Add more colors as needed
        else               color = cv::Vec3b(255, 255, 255); // Default to white
        colors.push_back(color);
    }

    // Create a colored segmented image
    output_image = cv::Mat(blured.size(), CV_8UC3);
    for (int row = 0; row < blured.rows; ++row) {
        for (int col = 0; col < blured.cols; ++col) {
            int label = labels.at<uint8_t>(row * blured.cols + col);
            output_image.at<cv::Vec3b>(row, col) = colors[label];
        }
    }
}



