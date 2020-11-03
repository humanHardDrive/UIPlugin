#pragma once

#include "UIPluginBase.hpp"

class BasicEventsPlugin : public UIPluginBase
{
public:
	BasicEventsPlugin();
	~BasicEventsPlugin();

protected:
	std::string resourcePath() { return "layout.xml"; };

private:
};