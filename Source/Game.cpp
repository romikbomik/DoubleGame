#include "Game.h"
#include "ImagePreprocessor.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include "TestInput.h"
#include <filesystem>
Game::Game(std::shared_ptr<IInput> input, std::shared_ptr<IOutput> output, std::shared_ptr<IModel> model)
	: input(input), output(output), model(model)
{
    model->Init();
}

void Game::MainLoop()
{
    cv::Mat frame;
    std::vector<cv::Mat> areas_of_interest;
    std::vector<cv::Mat> pocessed_areas_of_interest;
    std::vector<std::vector<Annotation>> annotations;
    while (true) {

        input->CaptureImage(frame);
        // Check for user input to change the threshold
        int key = cv::waitKey(1);
        if (key == 27) // Exit when the 'Esc' key is pressed
            break;
        else if (key == 'n')
        {
            std::shared_ptr<TestInput> testInput = std::dynamic_pointer_cast<TestInput>(input);
            if (testInput)
            {
                testInput->Next();
            }
        }
        else if (key == 'p')
        {
            int frameNumber = 1;
            std::string filename;
            do {
                std::ostringstream filenameStream;
                filenameStream << "TestInput" << frameNumber << ".jpg";
                filename = filenameStream.str();
                frameNumber++;
            } while (std::filesystem::exists(filename));
            cv::imwrite(filename, frame);
        }


        // Here, you can process the 'frame' using OpenCV functions
        ImagePreprocessor::FindAreasOfInterest(frame, areas_of_interest);
        ImagePreprocessor::ProcessAreasOfInterest(areas_of_interest, pocessed_areas_of_interest, annotations);
        annotations.clear();
        annotations.resize(areas_of_interest.size());
        for (int i = 0; i < areas_of_interest.size(); i++)
        {
            model->Predict(areas_of_interest[i], annotations[i]);
        }


        //if (key == 's') // Decrease threshold when 's' key is pressed
        //{
        std::string prefix = input->GetName();
        output->ReciveResult(areas_of_interest, annotations, prefix);
        //}
    }
}
