// DoubleGameCV.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "UIOutput.h"
#include "FasterRCnnModel.h"
#include "ImagePreprocessor.h"
#include "TestInput.h"
#include "IOutput.h"
#include "IModel.h"
#include "CocoOutput.h"
#include "CameraInput.h"
#include "Game.h"
#include "OutputComposite.h"


int main()
{
    std::shared_ptr<IInput> input = std::make_shared<TestInput>();
    std::shared_ptr<IOutput> output = std::make_shared<UIOutput>();
    std::shared_ptr<IModel> model = std::make_shared<FasterRCnnModel>();
    std::shared_ptr<OutputComposite> output_composite = std::make_shared<OutputComposite>(output);
    //output_composite->Add(std::make_shared<UIOutput>());
    if (!input)
    {
        return -1;
    }

    Game game = Game(input, output_composite, model);
    game.MainLoop();
    return 0;
}
