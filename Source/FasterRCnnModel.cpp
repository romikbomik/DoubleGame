#include "FasterRCnnModel.h"
#include <vision.h>
#include <algorithm> 
#include <opencv2/opencv.hpp>
#include <chrono>

FasterRCnnModel::FasterRCnnModel() : initialized(false), device(torch::kCPU), labels({"Background"})
{
}

bool FasterRCnnModel::Init()
{
    bool result = false;
    try {
        //get availible device type
        torch::DeviceType device_type;
        if (torch::cuda::is_available()) {
            std::cout << "FasterRCnnModel::init using CUDA" << std::endl;
            device_type = torch::kCUDA;
        }
        else {
            std::cout << "FasterRCnnModel::init using CPU" << std::endl;
            device_type = torch::kCPU;
        }
        device = torch::Device(device_type);
        //Load model TODO:: try async
        model = torch::jit::load(".\\models\\faster_rcnn_model_scripted_cpu.pt");
        model.eval();
        model.to(device);
        LoadLabels();

        initialized = true;
        result = true;
    }
    catch (const c10::Error& e) {
        std::cerr << "FasterRCnnModel::init " << e.msg() << std::endl;
        result = false;
    }
    catch (std::exception& e) {
        std::cerr << "FasterRCnnModel::init " << e.what() << std::endl;
        result = false;
    }

    return result;
}

void FasterRCnnModel::LoadLabels()
{
    std::ifstream file("E:\\Projects\\DoubleGameCV\\DoubleGameCV\\models\\config.json");
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // Parse the JSON array
    std::vector<std::string> loaded_lables;
    std::string item;
    while (std::getline(file, item, ',')) {
        // Remove leading and trailing whitespaces from each item
        item.erase(std::remove_if(item.begin(), item.end(), [](char c) { return std::isspace(c) || c == '[' || c == ']'; }), item.end());
        loaded_lables.push_back(item);
    }
    // Sort the items
    std::sort(loaded_lables.begin(), loaded_lables.end());

    //0 index is alwasy reserved for background. Concactionate sorted loaded lables with {"Background"} vector
    labels.insert(labels.end(), std::make_move_iterator(loaded_lables.begin()), std::make_move_iterator(loaded_lables.end()));
}

void FasterRCnnModel::NonMaxSuppresion(std::vector<std::vector<TargetInfo>>& targets, const float therhold)
{
    for (std::vector<TargetInfo>& img_target : targets)
    {
        std::vector<TargetInfo> finalPrediction;

        // Convert TargetInfo to std::vector<cv::Rect> and std::vector<float> for scores
        std::vector<cv::Rect> boxes;
        std::vector<float> scores;
        for (const auto& info : img_target) {
            boxes.push_back(info.rect);
            scores.push_back(info.score);
        }

        // OpenCV's NMS
        std::vector<int> keep;
        cv::dnn::NMSBoxes(boxes, scores, 0.0, therhold, keep);

        // Populate final prediction using the indices to keep
        for (int idx : keep) {
            finalPrediction.push_back(img_target[idx]);
        }
        img_target = finalPrediction;
    }
}

bool FasterRCnnModel::Forward(const std::vector<cv::Mat>& input_images, std::vector<std::vector<TargetInfo>>& targets)
{
    bool result = false;
    try {
        std::vector<torch::IValue> inputs;
        std::vector<torch::Tensor> images;
        images.reserve(input_images.size());;
        for (const cv::Mat& input_image : input_images)
        {
            cv::Mat transformed_image;
            input_image.convertTo(transformed_image, CV_32FC3);
            // Normalize by dividing by 255.0
            transformed_image /= 255.0;
            torch::Tensor img_tensor = torch::from_blob(transformed_image.data, { transformed_image.rows, transformed_image.cols, 3 }, torch::kFloat32);
            img_tensor = img_tensor.permute({ 2, 0, 1 });  // Change layout from HWC to CHW
            img_tensor = img_tensor.to(device);
            images.push_back(img_tensor);
        }
        // Create a vector of IValue to hold the input tensors
        inputs.push_back(images);
        // Make a prediction
        auto start_time = std::chrono::high_resolution_clock::now();
        torch::jit::IValue output = model.forward(inputs);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        std::cout << "Prediction time: " << duration.count() << " milliseconds" << std::endl;
        result = ExtractFasterRCnnTarget(output, targets);
        NonMaxSuppresion(targets);
        if(!result)
        {
            std::cerr << "FasterRCnnModel::Forward failed to extract target" << std::endl;
            return result;
        }
        result = true;
    }
    catch (const c10::Error& e) {
        std::cerr << "FasterRCnnModel::Forward " << e.msg() << std::endl;
        result = false;
    }
    catch (std::exception& e) {
        std::cerr << "FasterRCnnModel::Forward " << e.what() << std::endl;
        result = false;
    }
    return result;
}

bool FasterRCnnModel::ExtractFasterRCnnTarget(torch::jit::IValue& model_output, std::vector<std::vector<TargetInfo>>& target_out)
{
    bool result = false;
    std::vector<TargetInfo> targets;
    if (!model_output.isTuple())
    {
        std::cerr << "ExtractFasterRCnnTarget::model_output expected to be tuple" << std::endl;
        return result;
    }
    auto elements = model_output.toTuple()->elements();
    auto target_contatiner = elements[1];
    if (!target_contatiner.isList())
    {
        std::cerr << "ExtractFasterRCnnTarget::target contatiner expected to be list" << std::endl;
        return result;
    }

    auto targets_wrapper = target_contatiner.toList();
    target_out.clear();
    target_out.reserve(targets_wrapper.size());
    for (auto it = targets_wrapper.begin(); it != targets_wrapper.end(); it++)
    {
        auto current_element = *it;
        auto current_value = current_element.get();
        if (!current_value.isGenericDict())
        {
            std::cerr << "ExtractFasterRCnnTarget::target contatiner expected to be list" << std::endl;
            return result;
        }
        auto traget = current_value.toGenericDict();
        //std::cout << traget << std::endl;
        if (!traget.contains("boxes"))
        {
            std::cerr << "Key 'boxes' not found in the dictionary." << std::endl;
            return result;
        }
        // Extract the 'boxes' tensor from the dictionary
        torch::Tensor boxes_tensor = traget.at("boxes").toTensor();
        ExtractBboxs(boxes_tensor, targets);

        if (!traget.contains("scores"))
        {
            std::cerr << "Key 'scores' not found in the dictionary." << std::endl;
            return result;
        }
        // Extract the 'boxes' tensor from the dictionary
        torch::Tensor scores_tensor = traget.at("scores").toTensor();
        ExtractScores(scores_tensor, targets);

        if (!traget.contains("labels"))
        {
            std::cerr << "Key 'labels' not found in the dictionary." << std::endl;
            return result;
        }
        // Extract the 'boxes' tensor from the dictionary
        torch::Tensor labels_tensor = traget.at("labels").toTensor();
        ExtractLabels(labels_tensor, targets);

        target_out.push_back(std::move(targets));
        targets = {};
    }

    result = true;
    return result;
}

void FasterRCnnModel::ExtractBboxs(const torch::Tensor& boxes_tensor, std::vector<TargetInfo>& bboxs_out)
{
    // Get the number of bounding boxes
    int size = boxes_tensor.size(0);
    if (size && bboxs_out.empty())
    {
        // init vector
        bboxs_out = std::vector<TargetInfo>(size);
    }
    if (size != bboxs_out.size())
    {
        std::cerr << "boxes size missmatch with lables/scores" << std::endl;
        return;
    }

    // Iterate over the bounding boxes
    for (int i = 0; i < size; ++i) {
        // Extract the coordinates from the tensor
        float x1 = boxes_tensor[i][0].item<float>();
        float y1 = boxes_tensor[i][1].item<float>();
        float x2 = boxes_tensor[i][2].item<float>();
        float y2 = boxes_tensor[i][3].item<float>();

        // Convert the coordinates to integers and create a cv::Rect
        cv::Rect box(static_cast<int>(x1), static_cast<int>(y1), static_cast<int>(x2 - x1), static_cast<int>(y2 - y1));

        // Add the bounding box to the vector
        bboxs_out[i].rect = box;
    }
}

void FasterRCnnModel::ExtractLabels(const torch::Tensor& labels_tensor, std::vector<TargetInfo>& labels_out)
{
    // Get the number of scores
    int size = labels_tensor.size(0);
    if (size && labels_out.empty())
    {
        // init vector
        labels_out = std::vector<TargetInfo>(size);
    }
    if (size != labels_out.size())
    {
        std::cerr << "labels size missmatch with size/boxes" << std::endl;
        return;
    }

    // Iterate over the tensor
    for (int i = 0; i < size; ++i) {
        int value = labels_tensor[i].item<int>();
        //std::cout << "Found: " + labels.at(value) << std::endl;
        labels_out[i].label = value;
    }
}

void FasterRCnnModel::ExtractScores(const torch::Tensor& scores_tensor, std::vector<TargetInfo>& scores_out)
{
    // Get the number of scores
    int size = scores_tensor.size(0);
    if (size && scores_out.empty())
    {
        // init vector
        scores_out = std::vector<TargetInfo>(size);
    }
    if (size != scores_out.size())
    {
        std::cerr << "scores size missmatch with lables/boxes" << std::endl;
        return;
    }

    // Iterate over the tensor
    for (int i = 0; i < size; ++i) {
        float value = scores_tensor[i].item<float>();
        scores_out[i].score = value;
    }
}

void FasterRCnnModel::TargetsToAnnotations(const std::vector<std::vector<TargetInfo>>& targets, std::vector<std::vector<Annotation>>& predictions)
{
    predictions.clear();
    if (targets.empty())
    {
        return;
    }
    predictions.reserve(targets.size());
    for (const std::vector<TargetInfo>& img_targets : targets)
    {
        std::vector<Annotation> annotations;
        for (const TargetInfo & target : img_targets)
        {
            Annotation annotation;
            annotation.bbox = target.rect;
            annotation.name = labels.at(target.label);
            annotations.push_back(annotation);
        }
        predictions.push_back(std::move(annotations));
        annotations = {};
    }
}

bool FasterRCnnModel::Predict(const std::vector<cv::Mat>& input_images, std::vector<std::vector<Annotation>>& predictions)
{
    bool result = false;
    if (!initialized)
    {
        std::cerr << "Model not initialized" << std::endl;
        return result;
    }
    std::vector<std::vector<TargetInfo>> targets;
    if (!Forward(input_images, targets))
    {
        std::cerr << "Model fail to predict" << std::endl;
        return result;
    }
    TargetsToAnnotations(targets, predictions);
    return result;
}
