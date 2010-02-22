#include "SelfLocatorParameter.h"
#include "Tools/Streams/InStreams.h"
#include "Platform/GTAssert.h"

SelfLocatorParameter::SelfLocatorParameter()
{
  load("selfloc.cfg");
}

void SelfLocatorParameter::load(const std::string& fileName)
{
  InConfigFile file(Global::getSettings().expandLocationFilename(fileName), "default");
  ASSERT(file.exists());
  file >> *this;
}
