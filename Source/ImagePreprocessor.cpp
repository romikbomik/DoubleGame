#include "ImagePreprocessor.h"
#include "iostream"
#include <opencv2/opencv.hpp>
#include "ImageUtils.h"

#define BACKGROUND_PIXEL 0
#define AREA_OF_INTEREST_LIMIT 10

void ImagePreprocessor::PreprocessImage(std::vector<cv::Mat>& InputImages, std::vector<cv::Mat>& OutputAOI)
{
    OutputAOI.clear();
    cv::Mat GrayMat, TheholdMat ;
    try
    {
        for (auto& InputImage : InputImages)
        {
            cv::Mat gamma_corrected_image, SobelMat, binaryThreshold;
            // Gamma correction parameters
            //double gamma = 1.5; // Adjust this value as needed

            //// Apply gamma correction

            //cv::Mat lookup_table(1, 256, CV_8U);

            //for (int i = 0; i < 256; i++) {
            //    lookup_table.at<uchar>(0, i) = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);
            //}

            ////cv::LUT(InputImage, lookup_table, gamma_corrected_image);
            double minVal ,maxVal;
            cv::minMaxLoc(InputImage, &minVal, &maxVal);

            //            uchar minVal = 255, maxVal = 0;
            //for (int y = 0; y < InputImage.rows; y++) {
            //    for (int x = 0; x < InputImage.cols; x++) {
            //        if (InputImage.at<uchar>(y, x) == BACKGROUND_PIXEL) {
            //            continue;
            //        }
            //        if (InputImage.at<uchar>(y, x) > maxVal) {
            //            maxVal = InputImage.at<uchar>(y, x);
            //        }
            //        if (InputImage.at<uchar>(y, x) < minVal) {
            //            minVal = InputImage.at<uchar>(y, x);
            //        }
            //    }
            //    std::cout << std::endl;
            //}

            cv::Mat contrastStretchedImage;
            InputImage.convertTo(contrastStretchedImage, CV_8UC1, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));

            cv::Mat edges;
            cv::Canny(InputImage, edges, 20, 50);
      
            //cv::adaptiveThreshold(gamma_corrected_image, binaryThreshold, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 9, 7);
            //cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
            //cv::Mat morphed;
            //cv::morphologyEx(edges, morphed, cv::MORPH_CLOSE, kernel);

            OutputAOI.push_back(InputImage);

            //OutputAOI.push_back(contrastStretchedImage);
           // OutputAOI.push_back(edges);


            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;
            cv::findContours(edges, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);


            cv::Mat mask = cv::Mat::zeros(edges.size(), CV_8UC1);
            cv::drawContours(mask, contours, -1, cv::Scalar(255), cv::FILLED);


            cv::Mat result;  // Create a copy of the original image
            InputImage.copyTo(result);
            cv::Scalar red(0, 0, 255);  // Scalar for the color red (BGR format)

            // Iterate through each contour and draw if at the specified hierarchy level
            for (size_t i = 0; i < contours.size(); ++i) {
                if (hierarchy[i][3] == -1) { // Check the parent hierarchy level
                   // Now, find all children of this outer contour
                     
                    int FirstchildIndex = hierarchy[i][2]; // Get the index of the first child

                    int childIndex = - 1;
                    if (FirstchildIndex != -1)
                    {
                        childIndex = hierarchy[FirstchildIndex][2];
                    }
                    while (childIndex != -1) {
                        // Process the child contour here
                        std::vector<cv::Point>& childContour = contours[childIndex];
                        int contourArea = cv::contourArea(childContour);
                        
                        if (contourArea > 15)
                        {
                            cv::Rect boundingRect = cv::boundingRect(childContour);
                            cv::Mat AOI = InputImage(boundingRect).clone();
                            OutputAOI.push_back(AOI);
                            //cv::rectangle(result, boundingRect, red, 2);
                        }
                        // Move to the next child contour (if any)
                        childIndex = hierarchy[childIndex][0];
                    }

                }
            }

           //OutputAOI.push_back(result);

             //Iterate through each contour and draw a red rectangle
            //for (const std::vector<cv::Point>& contour : contours) {
            //    int contourArea = cv::contourArea(contour);

            //        cv::Rect boundingRect = cv::boundingRect(contour);
            //        cv::rectangle(result, boundingRect, red, 2);  // Draw a red rectangle
            //    
            //}
            ////binaryThreshold.convertTo(TheholdMat, CV_8UC1);
            //OutputAOI.push_back(result);
            
           // return MySobel(binaryThreshold);
        }
        for (auto& AOI : OutputAOI)
        {
            cv::resize(AOI, AOI, cv::Size(40, 40));
        }

    }
    catch (cv::Exception e)
    {
        std::cout << e.what();
    }

}



void ImagePreprocessor::RemoveBackground(cv::Mat& img, std::vector<cv::Mat>& OutputAOI)
{
        OutputAOI.clear();
        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

        cv::Mat blured;
        ImageUtils::Blur(gray, blured);

        cv::Mat threshed;
        //cv::threshold(blured, threshed, threshold, 255, cv::THRESH_BINARY_INV);
         // Apply Otsu's thresholding
        float dynamicThreahold = cv::threshold(blured, threshed, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
       // cv::adaptiveThreshold(blured, threshed, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2);
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::Mat morphed;
        cv::morphologyEx(threshed, morphed, cv::MORPH_CLOSE, kernel);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(morphed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

       
        cv::Mat mask = cv::Mat::zeros(threshed.size(), CV_8UC1);
        cv::drawContours(mask, contours,  -1, cv::Scalar(255), cv::FILLED);
        cv::Mat masked_data = cv::Mat(img.rows, img.cols, CV_8UC1, cv::Scalar(BACKGROUND_PIXEL));
        blured.copyTo(masked_data, mask);

        for (auto& contour : contours)
        {
            int contourArea = cv::contourArea(contour);
            if(contourArea > AREA_OF_INTEREST_LIMIT)
            {
                cv::Rect boundingRect = cv::boundingRect(contour);
                OutputAOI.push_back(masked_data(boundingRect).clone());
            }
        }


        /*
        cv::Rect boundingRect = cv::boundingRect(contours[contours.size() - 1]);
        cv::Mat dst = masked_data(boundingRect);

        cv::Mat dst_gray;
        cv::cvtColor(dst, dst_gray, cv::COLOR_BGR2GRAY);
        cv::Mat alpha;
        cv::threshold(dst_gray, alpha, 0, 255, cv::THRESH_BINARY);

        std::vector<cv::Mat> rgba;
        cv::split(dst, rgba);
        rgba.push_back(alpha);

        cv::Mat result;
        cv::merge(rgba, result);/**/
}

cv::Mat ImagePreprocessor::Segmentation(cv::Mat& img, const int kmeans, cv::Mat& centers /*=nullptr*/)
{
    //cv::Mat Pooled;
    //ImageUtils::Pooling(img, Pooled);
    cv::Mat Blured;
    ImageUtils::Blur(img, Blured);
    
    // Reshape the image into a 2D array of pixels (rows x cols, 3 channels)
    cv::Mat reshaped_image = Blured.reshape(1, Blured.rows * Blured.cols);

    // Convert the reshaped image to a 32-bit floating-point type
    cv::Mat reshaped_image_float;
    reshaped_image.convertTo(reshaped_image_float, CV_32F);

    // Define the criteria for K-Means
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 100, 0.2);

    // Perform K-Means clustering
    cv::Mat colloredlabels;
    cv::Mat labels(reshaped_image_float.rows, 1, CV_32S, cv::Scalar(0)); // Initialize labels to 0
    try {
        if (!centers.empty())
        {
            cv::kmeans(reshaped_image_float, kmeans, labels, criteria, 3, cv::KMEANS_USE_INITIAL_LABELS, centers);
        }
        else
        {
            cv::kmeans(reshaped_image_float, kmeans, labels, criteria, 3, cv::KMEANS_RANDOM_CENTERS, centers);
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
    for (int i = 0; i < kmeans; ++i) {
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
    cv::Mat colored_segmented_image(Blured.size(), CV_8UC3);
    for (int row = 0; row < Blured.rows; ++row) {
        for (int col = 0; col < Blured.cols; ++col) {
            int label = labels.at<uint8_t>(row * Blured.cols + col);
            colored_segmented_image.at<cv::Vec3b>(row, col) = colors[label];
        }
    }

    return colored_segmented_image;
}



