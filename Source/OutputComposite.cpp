#include "OutputComposite.h"

OutputComposite::OutputComposite()
{
}

OutputComposite::OutputComposite(std::shared_ptr<IOutput> output) :wrapped_output({ output })
{
}

void OutputComposite::Add(std::shared_ptr<IOutput> output)
{
	wrapped_output.push_back(output);
}

void OutputComposite::ReciveResult(const std::vector<cv::Mat>& output_aoi, const std::vector<std::vector<Annotation>>& annotations, const std::string image_source)
{
	for (std::shared_ptr<IOutput> output: wrapped_output)
	{
		output->ReciveResult(output_aoi, annotations, image_source);
	}

}
