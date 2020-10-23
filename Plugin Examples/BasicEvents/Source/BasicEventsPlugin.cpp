#include "BasicEventsPlugin.h"

#include "Events.h"

BasicEventsPlugin::BasicEventsPlugin()
{
}

BasicEventsPlugin::~BasicEventsPlugin()
{
}

extern "C" BOOST_SYMBOL_EXPORT BasicEventsPlugin plugin;
BasicEventsPlugin plugin;