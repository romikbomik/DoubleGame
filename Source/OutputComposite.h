#pragma once

#include "IOutput.h"
#include <string>
#include <vector>
#include <memory>
#include "Annotation.h"


class OutputComposite : public IOutput
{
public:
	OutputComposite();
	OutputComposite(std::shared_ptr<IOutput> output);
	void Add(std::shared_ptr<IOutput> output);
	// Inherited via IOutput
	virtual void ReciveResult(const std::vector<cv::Mat>& output_aoi, const std::vector<std::vector<Annotation>>& annotations, const std::string image_source) override;
private:
	std::vector<std::shared_ptr<IOutput>> wrapped_output;
};