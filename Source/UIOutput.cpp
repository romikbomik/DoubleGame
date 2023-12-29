#include "UIOutput.h"
#include <unordered_set>

void UIOutput::Draw(std::string name, std::vector<cv::Mat>& input)
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
    cv::imshow(name, canvas);
    bActiveWindows = true;
}

bool UIOutput::AccumulateImageInfo(std::vector<cv::Mat>& input, int& height, int& weight, int& type)
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

void UIOutput::DrawAnnotations(cv::Mat& img, const std::vector<Annotation>& annotations)
{
    for (const auto& annotation : annotations) {
        // Draw rectangle
        if (annotation.name.empty()) { continue; }
        cv::Scalar color = annotation.name == matching_label ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
        cv::rectangle(img, annotation.bbox, color, 2);  // Green color, thickness 2

        // Display the name of the annotation
        //cv::putText(img, annotation.name, cv::Point(annotation.bbox.x, annotation.bbox.y - 5),
         //   cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2); // Green color, font scale 0.5, thickness 2
    }
}

void UIOutput::FindMatchingLabel(const std::vector<std::vector<Annotation>>& annotations)
{
    if (annotations.size() <  2)
    {
        return;
    }

    std::unordered_set<std::string> set;
    //fill labels from first object
    for (const Annotation& annotation : annotations[0])
    {
        set.insert(annotation.name);
    }

    for (const Annotation& annotation : annotations[1])
    {
        if (set.find(annotation.name)!= set.end())
        {
            matching_label = annotation.name;
            return;
        }
    }

}

UIOutput::~UIOutput()
{
    if (bActiveWindows)
    {
        cv::destroyWindow("UIOutput");
    }
}

void UIOutput::ReciveResult(const std::vector<cv::Mat>& output_aoi, const std::vector<std::vector<Annotation>>& annotations, const std::string image_source)
{
    matching_label = "";
    std::vector<cv::Mat> output_aoi_copy(output_aoi.begin(), output_aoi.end());
    for (int i = 0; i < output_aoi_copy.size(); i++)
    {
        output_aoi[i].copyTo(output_aoi_copy[i]);
    }
    FindMatchingLabel(annotations);

    for (int i = 0; i < annotations.size() && i < output_aoi.size(); i++)
    {
        DrawAnnotations(output_aoi_copy[i], annotations[i]);
    }
    Draw("UIOutput", output_aoi_copy);
}
