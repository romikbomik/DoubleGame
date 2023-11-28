#include "CocoOutput.h"
#include <opencv2/imgcodecs.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>

void CocoOutput::ReciveResult(const std::vector<cv::Mat>& output_aoi, const std::vector<std::vector<Annotation>>& annotations, const std::string image_source)
{
	std::string folder_path = ".\\CocoTrainingData\\";

	// Create the folder if it doesn't exist
	std::filesystem::create_directories(folder_path);
	for (int i = 0; i < output_aoi.size(); i++)
	{
		std::string filename = folder_path + image_source + "_" + std::to_string(i) + ".jpg";
		bool result = cv::imwrite(filename, output_aoi[i]);

		if (result && i < annotations.size())
		{
			SaveAnnotation(filename, output_aoi[i], annotations[i]);
		}
	}
}

void CocoOutput::SaveAnnotation(std::string image_filename, const cv::Mat& image, const std::vector<Annotation>& annotations)
{
	std::filesystem::path image_path(image_filename);
	std::string filename = image_path.parent_path().string() + "\\" +
		image_path.stem().string() + ".xml";
	std::ofstream file(filename, std::ios::trunc);
	if (!file.is_open())
	{
		std::cerr << "CocoOutput::SaveAnnotation: failed to create file: " << filename << std::endl;
		return;
	}

	//write header
	file << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	file << "<annotation>\n";
	file << "\t<folder>CocoTrainingData</folder>\n";
	file << "\t<filename>" << image_path.stem().string() + image_path.extension().string() << "</filename>\n";
	file << "\t<source>\n";
	file << "\t\t<database>Unknown</database>\n";
	file << "\t</source>\n";
	file << "\t<size>\n";
	file << "\t\t<width>" << image.cols << "</width>\n";
	file << "\t\t<height>" << image.rows << "</height>\n";
	file << "\t\t<depth>" << image.depth() << "</depth>\n";
	file << "\t\t<channels>" << image.channels() << "</channels>\n";
	file << "\t</size>\n";
	file << "\t<segmented>0</segmented>\n";
	for (const Annotation& annotation : annotations)
	{
		// Write object information
		file << "\t<object>\n";
		file << "\t\t<name>Unknown</name>\n";
		file << "\t\t<pose>Unspecified</pose>\n";
		file << "\t\t<truncated>0</truncated>\n";
		file << "\t\t<difficult>0</difficult>\n";
		file << "\t\t<bndbox>\n";
		file << "\t\t\t<xmin>" << annotation.bbox.x << "</xmin>\n";
		file << "\t\t\t<ymin>" << annotation.bbox.y << "</ymin>\n";
		file << "\t\t\t<xmax>" << annotation.bbox.x + annotation.bbox.width << "</xmax>\n";
		file << "\t\t\t<ymax>" << annotation.bbox.y + annotation.bbox.height << "</ymax>\n";
		file << "\t\t</bndbox>\n";
		file << "\t</object>\n";
	}
	file << "</annotation>\n";
	file.close();
}
