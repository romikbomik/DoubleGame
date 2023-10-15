// DoubleGameCV.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include "ImagePreprocessor.h"

#include "TestInput.h";
#include "TestInput.h";
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <algorithm>
void Draw(std::string Name, std::vector<cv::Mat>& Input);

int main()
{
    std::shared_ptr<IInput> Input = std::make_shared<TestInput>();
    if (!Input)
    {
        return -1;
    }

    cv::Mat frame;
    std::vector<cv::Mat> AreasOfInterest, PotentialObjects;
    float threshold = 110.0f;
    while (true) {

        Input->CaptureImage(frame);
        // Check for user input to change the threshold
        int key = cv::waitKey(1);
        if (key == 27) // Exit when the 'Esc' key is pressed
            break;
        else if (key == 'w') // Increase threshold when 'w' key is pressed
        {
            threshold += 1.0f;
            threshold = std::max(0.0f, std::min(255.0f, threshold));
        }
        else if (key == 's') // Decrease threshold when 's' key is pressed
        {
            threshold -= 1.0f;
            threshold = std::max(0.0f, std::min(255.0f, threshold));
        }
        else if (key == 'p') // Decrease threshold when 's' key is pressed
            cv::imwrite("TestInput2.jpg", frame);

        // Here, you can process the 'frame' using OpenCV functions
        ImagePreprocessor::RemoveBackground(frame, AreasOfInterest);
        ImagePreprocessor::PreprocessImage(AreasOfInterest, PotentialObjects);

        imshow("Source image", frame);
        Draw("AreasOfInterest", AreasOfInterest);
        Draw("PotentialObjects", PotentialObjects);
        // Display the histogram

    }
    cv::destroyAllWindows(); // Close the window

    return 0;
}

void Draw(std::string Name, std::vector<cv::Mat>& Input)
{
    int canvasWidth = 0;
    int canvasHeight = 0;

    for (const cv::Mat& roi : Input) {
        canvasWidth = std::max(canvasWidth, roi.cols);
        canvasHeight += roi.rows;
    }

    // Create a canvas (large cv::Mat) to combine all the regions of interest.
    cv::Mat canvas(canvasHeight, canvasWidth, CV_8UC1, cv::Scalar(255, 255, 255)); // Initialize with white color (optional).

    // Copy each region of interest to the canvas.
    int y_offset = 0;
    for (const cv::Mat& roi : Input) {
        roi.copyTo(canvas(cv::Rect(0, y_offset, roi.cols, roi.rows)));
        y_offset += roi.rows;
    }

    // Display the combined canvas in an OpenCV window.
    cv::imshow("Name", canvas);
}