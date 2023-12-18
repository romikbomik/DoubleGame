// DoubleGameCV.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "UIOutput.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include "ImagePreprocessor.h"
#include <filesystem>
#include "TestInput.h"
#include "IOutput.h"
#include "IModel.h"
#include "CocoOutput.h"
#include "FasterRCnnModel.h"
#include "Annotation.h"
#include "CameraInput.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <algorithm>
#include <Game.h>
#include "OutputComposite.h"

void Draw(std::string name, std::vector<cv::Mat>& input);
bool AccumulateImageInfo(std::vector<cv::Mat>& input, int& height, int& weight, int& type);
void drawAnnotations(cv::Mat& img, const std::vector<Annotation>& annotations);


int main()
{
    std::shared_ptr<IInput> input = std::make_shared<TestInput>();
   // std::shared_ptr<IOutput> output = std::make_shared<CocoOutput>();
    std::shared_ptr<IModel> model = std::make_shared<FasterRCnnModel>();
    std::shared_ptr<OutputComposite> output_composite = std::make_shared<OutputComposite>(std::make_shared<UIOutput>());
   // output_composite->Add(std::make_shared<UIOutput>());
    if (!input)
    {
        return -1;
    }

    Game game = Game(input, output_composite, model);
    game.MainLoop();

    //cv::Mat frame;
    //std::vector<cv::Mat> areas_of_interest;
    //std::vector<cv::Mat> pocessed_areas_of_interest;
    //std::vector<std::vector<Annotation>> annotations;

    ////model->Init();
    //while (true) {

    //    input->CaptureImage(frame);
    //    // Check for user input to change the threshold
    //    int key = cv::waitKey(1);
    //    if (key == 27) // Exit when the 'Esc' key is pressed
    //        break;
    //    else if (key == 'n')
    //    {
    //        std::shared_ptr<TestInput> testInput = std::dynamic_pointer_cast<TestInput>(input);
    //        if (testInput)
    //        {
    //            testInput->Next();
    //        }
    //    }
    //    else if (key == 'p')
    //    {
    //        int frameNumber = 1;
    //        std::string filename;
    //        do {
    //            std::ostringstream filenameStream;
    //            filenameStream << "TestInput"  << frameNumber << ".jpg";
    //            filename = filenameStream.str();
    //            frameNumber++;
    //        } while (std::filesystem::exists(filename));
    //        cv::imwrite(filename, frame);
    //    }


    //    // Here, you can process the 'frame' using OpenCV functions
    //   ImagePreprocessor::FindAreasOfInterest(frame, areas_of_interest);
    //   ImagePreprocessor::ProcessAreasOfInterest(areas_of_interest, pocessed_areas_of_interest, annotations);

    //   imshow("Source image", frame);
    //   Draw("AreasOfInterest", pocessed_areas_of_interest);
    //   std::vector<Annotation> annotations2;
    //   //model->Predict(areas_of_interest[0], annotations2);
    //   //drawAnnotations(areas_of_interest[0], annotations2);
    //   //imshow("Result", areas_of_interest[0]);
    //   if (key == 's') // Decrease threshold when 's' key is pressed
    //   {
    //        std::string prefix = input->GetName();
    //        output->ReciveResult(areas_of_interest, annotations, prefix);
    //   }
    //    


    //}
    //cv::destroyAllWindows(); // Close the window

    return 0;
}

void drawAnnotations(cv::Mat& img, const std::vector<Annotation>& annotations) {
    for (const auto& annotation : annotations) {
        // Draw rectangle
        cv::rectangle(img, annotation.bbox, cv::Scalar(0, 255, 0), 2);  // Green color, thickness 2

        // Display the name of the annotation
        cv::putText(img, annotation.name, cv::Point(annotation.bbox.x, annotation.bbox.y - 5),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);  // Green color, font scale 0.5, thickness 2
    }
}

void Draw(std::string name, std::vector<cv::Mat>& input)
{
    if (input.empty())
    {
        std::cerr << "DoubleGameCV main Draw: empty inout";
        return;
    }
    int canvasHeight, canvasWidth, type;
    if (!AccumulateImageInfo(input, canvasHeight, canvasWidth, type))
    {
        std::cerr << "DoubleGameCV main Draw: depth missmatch of input images";
    }
    // Create a canvas (large cv::Mat) to combine all the regions of interest.
    cv::Mat canvas(canvasHeight, canvasWidth, type, cv::Scalar(255, 255, 255)); // Initialize with white color (optional).

    // Copy each region of interest to the canvas.
    int y_offset = 0;
    for (const cv::Mat& roi : input) {
        roi.copyTo(canvas(cv::Rect(0, y_offset, roi.cols, roi.rows)));
        y_offset += roi.rows;
    }

    // Display the combined canvas in an OpenCV window.
    cv::imshow("Name", canvas);

}

bool AccumulateImageInfo(std::vector<cv::Mat>& input, int& height, int& weight, int& type)
{
    bool result = false;
    const int invalid_depth = -1;
    height = 0;
    weight = 0;
    type = invalid_depth;
    for (const cv::Mat& roi : input) {
        weight = std::max(weight, roi.cols);
        height += roi.rows;
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