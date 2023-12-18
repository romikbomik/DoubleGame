#pragma once
#include "IModel.h"
#include "IInput.h"
#include "IOutput.h"
#include "memory"

class Game
{
public:
	Game(std::shared_ptr<IInput> input, std::shared_ptr<IOutput> output, std::shared_ptr<IModel> model);
	void MainLoop();
private:
	std::shared_ptr<IInput> input;
	std::shared_ptr<IOutput> output;
	std::shared_ptr<IModel> model;
};

