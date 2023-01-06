#pragma once

#include "RasterSurface.h"
#include "XTime.h"

class Application
{
private:
	XTime mTimer;

public:
	Application() = default;
	~Application();

public:
	void Init();

private:
	void Update();
};

