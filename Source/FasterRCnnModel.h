#pragma once

#include "IModel.h"
#include <vector>
#include <string>
#include <opencv2/core.hpp>
#include <torch/torch.h>
#include <torch/script.h>

struct TargetInfo 
{
	cv::Rect rect;
	int  label;
	float score;
	TargetInfo() : rect(cv::Rect(0, 0, 0, 0)), label(0), score(0.0f) {}
};


class FasterRCnnModel : public IModel
{
public:
	FasterRCnnModel();
	// Inherited via IModel
	virtual bool Predict(const std::vector<cv::Mat>& input_images, std::vector<std::vector<Annotation>>& predictions) override;
	virtual bool Init() override;
private:
	void NonMaxSuppresion(std::vector<std::vector<TargetInfo>>& targets, const float therhold = 0.3f);
	bool Forward(const std::vector<cv::Mat>& input_images, std::vector<std::vector<TargetInfo>>& targets);
	void LoadLabels();
	bool ExtractFasterRCnnTarget(torch::jit::IValue& model_output, std::vector<std::vector<TargetInfo>>& target_out);
	void ExtractBboxs(const torch::Tensor& target_in, std::vector<TargetInfo>& bboxs_out);
	void ExtractLabels(const torch::Tensor& target_in, std::vector<TargetInfo>& labels_out);
	void ExtractScores(const torch::Tensor& target_in, std::vector<TargetInfo>& scores_out);
	void TargetsToAnnotations(const std::vector<std::vector<TargetInfo>>& targets, std::vector<std::vector<Annotation>>& predictions);

	torch::jit::script::Module model;
	std::vector<std::string> labels;
	torch::Device device;
	bool initialized;
};
