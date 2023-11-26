// DoubleGameCV.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include "ImagePreprocessor.h"

#include <filesystem>
#include "TestInput.h";
#include "CameraInput.h";
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <algorithm>
void Draw(std::string Name, std::vector<cv::Mat>& Input);
bool AccumulateImageInfo(std::vector<cv::Mat>& Input, int& Height, int& Weight, int& type);

int main()
{
    std::shared_ptr<IInput> Input = std::make_shared<TestInput>();
    if (!Input)
    {
        return -1;
    }

    cv::Mat frame;
    std::vector<cv::Mat> AreasOfInterest;
    std::vector<cv::Mat> ProcessedAreasOfInterest;
    std::vector<cv::Rect> Annotations;
    while (true) {

        Input->CaptureImage(frame);
        // Check for user input to change the threshold
        int key = cv::waitKey(1);
        if (key == 27) // Exit when the 'Esc' key is pressed
            break;
        else if (key == 'n') // Decrease threshold when 's' key is pressed
        {
            std::shared_ptr<TestInput> testInput = std::dynamic_pointer_cast<TestInput>(Input);
            if (testInput)
            {
                testInput->Next();
            }
        }
        else if (key == 'p') // Decrease threshold when 's' key is pressed
        {
            int frameNumber = 1;
            std::string filename;
            do {
                std::ostringstream filenameStream;
                filenameStream << "TestInput"  << frameNumber << ".jpg";
                filename = filenameStream.str();
                frameNumber++;
            } while (std::filesystem::exists(filename));
            cv::imwrite(filename, frame);
        }


        // Here, you can process the 'frame' using OpenCV functions
       ImagePreprocessor::FindAreasOfInterest(frame, AreasOfInterest);
       ImagePreprocessor::ProcessAreasOfInterest(AreasOfInterest, ProcessedAreasOfInterest, Annotations);

       imshow("Source image", frame);
       Draw("AreasOfInterest", ProcessedAreasOfInterest);

       if (key == 's') // Decrease threshold when 's' key is pressed
       {
           std::string prefix = Input->GetName();
           for (size_t i = 0; i < AreasOfInterest.size(); i++) {
               std::string filename = ".\\TrainingData\\" + prefix + "_image" + std::to_string(i) + ".jpg";
               cv::imwrite(filename, AreasOfInterest[i]);
           }
       }
        


    }
    cv::destroyAllWindows(); // Close the window

    return 0;
}

void Draw(std::string Name, std::vector<cv::Mat>& Input)
{
    if (Input.empty())
    {
        std::cerr << "DoubleGameCV main Draw: empty inout";
        return;
    }
    int canvasHeight, canvasWidth, type;
    if (!AccumulateImageInfo(Input, canvasHeight, canvasWidth, type))
    {
        std::cerr << "DoubleGameCV main Draw: depth missmatch of input images";
    }
    // Create a canvas (large cv::Mat) to combine all the regions of interest.
    cv::Mat canvas(canvasHeight, canvasWidth, type, cv::Scalar(255, 255, 255)); // Initialize with white color (optional).

    // Copy each region of interest to the canvas.
    int y_offset = 0;
    for (const cv::Mat& roi : Input) {
        roi.copyTo(canvas(cv::Rect(0, y_offset, roi.cols, roi.rows)));
        y_offset += roi.rows;
    }

    // Display the combined canvas in an OpenCV window.
    cv::imshow("Name", canvas);

}

bool AccumulateImageInfo(std::vector<cv::Mat>& Input, int& Height, int& Weight, int& type)
{
    bool result = false;
    const int invalid_depth = -1;
    Height = 0;
    Weight = 0;
    type = invalid_depth;
    for (const cv::Mat& roi : Input) {
        Weight = std::max(Weight, roi.cols);
        Height += roi.rows;
        if (type == invalid_depth)
        {
            type = roi.type();
            result = true;
        }
        else if (type != roi.type())
        {
            result = false;
            break;
        }
    }
    return result;
}