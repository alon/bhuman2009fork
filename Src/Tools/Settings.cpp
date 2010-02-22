/**
* @file Tools/Settings.cpp
* Implementation of a class that provides access to settings-specific configuration directories.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "Tools/Streams/InStreams.h"
#include "Settings.h"
#include "Representations/Infrastructure/RoboCupGameControlData.h"
#ifdef TARGET_SIM
#include "Platform/RoboCupCtrl.h"
#endif
#include "Platform/GTAssert.h"
#include "Platform/SystemCall.h"

Settings::Settings() :
teamNumber(0),
teamColor(TEAM_BLUE),
playerNumber(0),
teamPort(0),
model(nao),
recover(false)
{
}

void Settings::load()
{
  InConfigFile file("settings.cfg");
  ASSERT(file.exists());
  std::string entryName;
  while(!file.eof())
  {
    file >> entryName;
    if(entryName == "model")
    {
      file >> entryName;
      if(entryName == "nao")
        model = nao;
      else
        ASSERT(false);
    }
	else if(entryName == "teamNumber")
      file >> teamNumber;
	else if(entryName == "teamPort")
      file >> teamPort;
    else if(entryName == "teamColor")
    {
      file >> entryName;
      if(entryName == "blue")
        teamColor = TEAM_BLUE;
      else if(entryName == "red")
        teamColor = TEAM_RED;
      else
        ASSERT(false);
    }
    else if(entryName == "playerNumber")
      file >> playerNumber;
    else if(entryName == "location")
      file >> location;
    else
      ASSERT(false);
  }

#ifdef TARGET_SIM
  if(SystemCall::getMode() == SystemCall::simulatedRobot)
  {
    int index = RoboCupCtrl::getController()->getRobotName()[5] - '1';
    teamNumber = index < 4 ? 1 : 2;
    teamPort = 10000 + teamNumber;
    teamColor = index < 4 ? TEAM_BLUE : TEAM_RED;
    playerNumber = (index & 3) + 1;
    model = nao;
  }
  else if(SystemCall::getMode() == SystemCall::physicalRobot)
  {
    const char* name = RoboCupCtrl::getController()->getRobotFullName().c_str();
    int index = name[strlen(name) - 1] - '1' + 1;
    if(index >= 1 && index <= 4)
      playerNumber = index;
  }
#endif

  robot = "Nao";
}

std::string Settings::expandHostFilename(const std::string& file) const
{
  return std::string("Hosts/") + SystemCall::getHostName() + "/" + file;
}

std::string Settings::expandLocationFilename(const std::string& file) const
{
  return std::string("Locations/") + location + "/" + file;
}

std::string Settings::expandRobotFilename(const std::string& file) const
{
  return std::string("Robots/") + robot + "/" + file;
}

std::string Settings::expandHostLocationFilename(const std::string& file) const
{
  return std::string("Locations/") + location + "/Hosts/" + SystemCall::getHostName() + "/" + file;
}

std::string Settings::expandRobotLocationFilename(const std::string& file) const
{
  return std::string("Locations/") + location + "/Robots/" + robot + "/" + file;
}

std::string Settings::expandModelFilename(const std::string& file) const
{
  return std::string("Robots/") + "Nao" + "/" + file;
}
