/**
* @file Tools/Settings.h
* Definition of a class that provides access to settings-specific configuration directories.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/
#ifndef __Settings_h__
#define __Settings_h__

#include <string>

class ConsoleRoboCupCtrl;
class Process;
class TeamDataProvider;
class GameDataProvider;
class CMainFrame;

/**
* @class Settings
* The class provides access to settings-specific configuration directories.
*/
class Settings
{
private:
  /** 
  * Default constructor.
  */
  Settings();

  /**
  * The function loads the settings from disk.
  */
  void load();

  int teamNumber; /**< The number of our team in the game controller. Private, use theOwnTeamInfo.teamNumber instead. */
  unsigned int teamColor; /**< The color of our goal. Private, use theOwnTeamInfo.teamColour instead. */
  int playerNumber; /**< The number of the robot in the team. Private, use theRobotInfo.playerNumber instead. */
  std::string location; /**< The name of the location. */

public:
  enum Model
  {
    nao
  };

  int teamPort; /**< The UDP port our team uses for team communication. */
  Model model; /**< The model of this robot. */
  std::string robot; /**< The name of this robot. */
  bool recover; /**< Start directly without the pre-initial state. */

  /** 
  * The function prefixes the given filename by a path based on the host's name.
  * @param file The name of a configuration file that is located in the Config/Hosts
  *             directory tree.
  * @return A path relative to the Config directory.
  */
  std::string expandHostFilename(const std::string& file) const;

  /** 
  * The function prefixes the given filename by a path based on the selected location.
  * @param file The name of a configuration file that is located in the Config/Location
  *             directory tree.
  * @return A path relative to the Config directory.
  */
  std::string expandLocationFilename(const std::string& file) const;

  /** 
  * The function prefixes the given filename by a path based on the robot's name.
  * @param file The name of a configuration file that is located in the Config/Robot
  *             directory tree.
  * @return A path relative to the Config directory.
  */
  std::string expandRobotFilename(const std::string& file) const;

  /** 
  * The function prefixes the given filename by a path based on the host name and
  * the selected location.
  * @param file The name of a configuration file that is located in the Config/Location
  *             directory tree.
  * @return A path relative to the Config directory.
  */
  std::string expandHostLocationFilename(const std::string& file) const;

  /** 
  * The function prefixes the given filename by a path based on the robot's name and
  * the selected location.
  * @param file The name of a configuration file that is located in the Config/Location
  *             directory tree.
  * @return A path relative to the Config directory.
  */
  std::string expandRobotLocationFilename(const std::string& file) const;

  /** 
  * The function prefixes the given filename by a path based on the robot's model.
  * @param file The name of a configuration file that is located in the Config/Robots
  *             directory tree in one of the directories dedicated to robot models.
  * @return A path relative to the Config directory.
  */
  std::string expandModelFilename(const std::string& file) const;

  friend class ConsoleRoboCupCtrl; /**< This class requires a local copy. */
  friend class Process; /**< A process is allowed to create the instance. */
  friend class TeamDataProvider; /**< To access teamNumber, teamColor and playerNumber. */
  friend class GameDataProvider; /**< To access teamNumber, teamColor and playerNumber. */
  friend class CMainFrame; /**< This class requires a local copy. */
};

#endif // __Settings_h_
