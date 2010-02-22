/**
 * @file ControllerQt/ConsoleRoboCupCtrl.cpp
 *
 * This file implements the class ConsoleRoboCupCtrl.
 *
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas R�fer</a>
 */

#define USE_GETMESSAGEIDNAME
#include "ConsoleRoboCupCtrl.h"
#include "Platform/SimRobotQt/LocalRobot.h"
#include "Platform/Directory.h"
#include "Platform/itoa.h"

SystemCall::Mode ConsoleRoboCupCtrl::mode = SystemCall::simulatedRobot;
std::string ConsoleRoboCupCtrl::logFile;

CONNECT_CONTROLLER_TO_SCENE(ConsoleRoboCupCtrl,"RoboCup");

ConsoleRoboCupCtrl::ConsoleRoboCupCtrl() :
teamHandler(teamIn, teamOut),
timeStamp(0),
robotNumber(-1)
{
  settings.load();
  xabslInfo = 0;
  debugRequestTable = 0;
  moduleInfo = 0;
  drawingManager = 0;
  imageViews = 0;
  fieldViews = 0;
  plotViews = 0;
  newLine = true;
  nesting = 0;
  if(!robots.empty())
    selected.push_back((*robots.begin())->getRobotProcess());

  start();
  Global::theStreamHandler = &streamHandler;

  std::string fileName = getSimulationFileName();
  int p = fileName.find_last_of("\\/"),
      p2 = fileName.find_last_of(".");
  if(p2 > p)
    fileName = fileName.substr(0, p2);
  executeFile(fileName.c_str(), false, 0);

  for(std::list<Robot*>::iterator i = robots.begin(); i != robots.end(); ++i)
    (*i)->getRobotProcess()->handleConsole("endOfStartScript");
  for(std::list<RemoteRobot*>::iterator i = remoteRobots.begin(); i != remoteRobots.end(); ++i)
    (*i)->handleConsole("endOfStartScript");
  for(std::list<TeamRobot*>::iterator i = teamRobots.begin(); i != teamRobots.end(); ++i)
    (*i)->handleConsole("endOfStartScript");
  Global::theStreamHandler = &streamHandler;
}

ConsoleRoboCupCtrl::~ConsoleRoboCupCtrl()
{
  for(std::list<RemoteRobot*>::iterator i = remoteRobots.begin(); i != remoteRobots.end(); ++i)
    (*i)->announceStop();
  for(std::list<TeamRobot*>::iterator i = teamRobots.begin(); i != teamRobots.end(); ++i)
    (*i)->announceStop();
  for(std::list<RemoteRobot*>::iterator i = remoteRobots.begin(); i != remoteRobots.end(); ++i)
    delete *i;
  for(std::list<TeamRobot*>::iterator i = teamRobots.begin(); i != teamRobots.end(); ++i)
    delete *i;
  stop();
  mode = SystemCall::simulatedRobot;
}

void ConsoleRoboCupCtrl::execute()
{
  {
    SYNC;
    for(std::list<std::string>::const_iterator i = textMessages.begin(); i != textMessages.end(); ++i)
      if(*i == "_cls")
        clear();
      else if(newLine || &*i != &*textMessages.rend())
        RoboCupCtrl::printLn(*i);
      else
        RoboCupCtrl::print(*i);
    textMessages.clear();
  }
  RoboCupCtrl::execute();

  for(std::list<RemoteRobot*>::iterator i = remoteRobots.begin(); i != remoteRobots.end(); ++i)
    (*i)->update();

  Global::theStreamHandler = &streamHandler;
  ntp.doSynchronization(0, SystemCall::getRealSystemTime(), teamOut.out);
  teamHandler.execute();
  teamIn.handleAllMessages(*this);
  teamIn.clear();
  for(std::list<TeamRobot*>::iterator i = teamRobots.begin(); i != teamRobots.end(); ++i)
    (*i)->update();
  Global::theStreamHandler = &streamHandler;

  std::string robotNumbers = "";
  for(int i = 1; i < 12; ++i)
    if(SystemCall::getRealTimeSince(ntp.receiveTimeStamps[i]) < 500)
    {
      if(robotNumbers != "")
        robotNumbers += ", ";
      char buf[20];
      robotNumbers += itoa_s(i, buf, sizeof(buf), 10);
    }

  if(robotNumbers != "")
    statusText += " [" + robotNumbers + "]";
  setStatusText(statusText);

  if(completion.empty())
    createCompletion();
}

void ConsoleRoboCupCtrl::executeFile(std::string name, bool printError, RobotConsole* console)
{
  if(nesting == 10)
    printLn("Nesting Error");
  else
  {
    ++nesting;
    if((int) name.rfind('.') <= (int) name.find_last_of("\\/"))
      name = name + ".con";
    if(name[0] != '/' && name[0] != '\\' && (name.size() < 2 || name[1] != ':'))
      name = std::string("Scenes\\") + name;
    InBinaryFile stream(name.c_str());
    if(!stream.exists())
    {
      if(printError)
        printLn(name + " not found");
    }
    else
      while(!stream.eof())
      {
        std::string line;
        while(!stream.eof())
        {
          char c[2] = " ";
          stream >> c[0];
          if(c[0] == '\n')
            break;
          else if(c[0] != '\r')
            line = line + c;
        }
        if(line.find_first_not_of(" ") != std::string::npos)
          executeConsoleCommand(line, console);
      }
    --nesting;
  }
}

void ConsoleRoboCupCtrl::onSelected(SimObject* obj)
{
  for(std::list<Robot*>::iterator i = robots.begin(); i != robots.end(); ++i)
    if(obj == (*i)->getSimRobotObject())
    {
      selected.clear();
      selected.push_back((*i)->getRobotProcess());
      printLn(std::string("robot ") + (*i)->getName());
      return;
    }
}

void ConsoleRoboCupCtrl::onKeyEvent(int key, bool pressed)
{
  if(key > 10)
    for(std::list<RobotConsole*>::iterator i = selected.begin(); i != selected.end(); ++i)
      (*i)->handleKeyEvent(key - 11, pressed);
}

void ConsoleRoboCupCtrl::executeConsoleCommand(const std::string& command, RobotConsole* console)
{
  std::string buffer;
  InConfigMemory stream(command.c_str(), command.size());
  stream >> buffer;
  if(buffer == "") // comment
    return;
  else if(buffer == "help" || buffer == "?")
    help(stream);
  else if(buffer == "robot")
  {
    stream >> buffer;
    if(buffer == "?")
    {
      for(std::list<Robot*>::iterator i = robots.begin(); i != robots.end(); ++i)
        print(std::string((*i)->getName()) + " ");
      for(std::list<RemoteRobot*>::iterator i = remoteRobots.begin(); i != remoteRobots.end(); ++i)
        print(std::string((*i)->getName()) + " ");
      for(std::list<TeamRobot*>::iterator i = teamRobots.begin(); i != teamRobots.end(); ++i)
        print(std::string((*i)->getName()) + " ");
      printLn("");
      return;
    }
    else if(buffer == "all")
    {
      selected.clear();
      for(std::list<Robot*>::iterator i = robots.begin(); i != robots.end(); ++i)
        selected.push_back((*i)->getRobotProcess());
      for(std::list<RemoteRobot*>::iterator i = remoteRobots.begin(); i != remoteRobots.end(); ++i)
        selected.push_back(*i);
      for(std::list<TeamRobot*>::iterator i = teamRobots.begin(); i != teamRobots.end(); ++i)
        selected.push_back(*i);
      return;
    }
    else 
    {
      selected.clear();
      for(;;)
      {
        if(!selectRobot(buffer))
          break;
        else if(stream.getEof())
          return;
        stream >> buffer;
      }
    }
    printLn("Syntax Error");
  }
  else if(buffer == "call")
  {
    stream >> buffer;
    executeFile(buffer, true, console);
  }
  else if(buffer == "ro")
  {
    if(!setReleaseOptions(stream))
      printLn("Syntax Error");
  }
  else if(buffer == "st")
  {
    stream >> buffer;
    if(buffer == "on")
    {
      if(!simTime)
      { // simulation time continues at real time
        time = getTime();
        simTime = true;
      }
    }
    else if(buffer == "off")
    {
      if(simTime)
      { // real time contiues at simulation time
        time = getTime() - SystemCall::getRealSystemTime();
        simTime = false;
      }
    }
    else
      printLn("Syntax Error");
  }
  else if(buffer == "dt")
  {
    stream >> buffer;
    if(buffer == "on")
      dragTime = true;
    else if(buffer == "off")
      dragTime = false;
    else
      printLn("Syntax Error");
  }
  else if(buffer == "sc")
  {
    if(!startRemote(stream))
    {
      selected.clear();
      if(!robots.empty())
        selected.push_back((*robots.begin())->getRobotProcess());
    }
  }
  else if(buffer == "sp")
    startPhysical(stream);
  else if(buffer == "sl")
  {
    if(!startLogFile(stream))
      printLn("Logfile not found!");
  }
  else if(buffer == "su")
    startTeamRobot(stream);
  else if(buffer == "tc")
  {
    int port;
    std::string subnet;
    stream >> port >> subnet;
    if(subnet == "")
      subnet = "255.255.255.255";
    if(port)
      teamHandler.start(port, subnet);
  }
  else if(selected.empty())
    if(buffer == "cls")
      printLn("_cls");
    else if(buffer == "echo")
      echo(stream);
    else
      printLn("No robot selected!");
  else if(console)
  {
    console->handleConsole(command);
    Global::theStreamHandler = &streamHandler;
  }
  else
  {
    for(std::list<RobotConsole*>::iterator i = selected.begin(); i != selected.end(); ++i)
      (*i)->handleConsole(command);
    Global::theStreamHandler = &streamHandler;
  }
  if(completion.empty())
    createCompletion();
}

bool ConsoleRoboCupCtrl::selectRobot(const std::string& name)
{
  for(std::list<Robot*>::iterator i = robots.begin(); i != robots.end(); ++i)
    if(name == (*i)->getName())
    {
      selected.push_back((*i)->getRobotProcess());
      return true;
    }
  for(std::list<RemoteRobot*>::iterator i = remoteRobots.begin(); i != remoteRobots.end(); ++i)
    if(name == (*i)->getName())
    {
      selected.push_back(*i);
      return true;
    }
  return false;
}

void ConsoleRoboCupCtrl::help(InConfigMemory& stream)
{
  std::string pattern;
  stream >> pattern;
  list("Initialization commands:",pattern,true);
  list("  sc <name> [<a.b.c.d>] : Starts a TCP connection to a remote robot.",pattern,true);
  list("  sl <name> <file> : Starts a robot reading its inputs from a log file.",pattern,true);
  list("  sp <name> : Start processes for a physical robot.",pattern,true);
  list("  su <name> <number> : Starts a UDP team connection to a remote robot with a certain player number. Requires command tc.",pattern,true);
  list("  sw <name> : Creates a robot reading its inputs from a wireless connection.",pattern,true);
  list("  tc <port> <subnet>: Listen to team communication on given port and broadcast to a subnet.",pattern,true);
  list("Global commands:",pattern,true);
  list("  call <file> : Execute a script file.",pattern,true);
  list("  cls : Clear console window.",pattern,true);
  list("  dt off | on : Switch simulation dragging to realtime on or off.",pattern,true);
  list("  echo <text> : Print text into console window. Useful in console.con.",pattern,true);
  list("  help | ? [<pattern>] : Display this text.",pattern,true);
  list("  robot ? | all | <name> {<name>} : Connect console to a set of active robots. Alternatively, double click one robot.",pattern,true);
  list("  ro stopwatch ( off | <letter> ) | ( sensorData | robotHealth | motionRequest | linePercept ) ( off | on ) : Set release options sent by team communication.",pattern,true);
  list("  st off | on : Switch simulation of time on or off.",pattern,true);
  list("  # <text> : Comment.",pattern,true);
  list("Robot commands:",pattern,true);
  list("  bc <red%> <green%> <blue%> : Set the background color of all 3-D views.",pattern,true);
  list("  ci off | on : Switch the calculation of images on or off.",pattern,true);
  list("  ct off | on | undo | <color> | load <file> | save <file> | send [<ms> | off] | clear [<color>] | imageRadius <number> | shrink [<color>] | grow [<color>] | colorSpaceRadius <number> | sendAndWrite | smart [off] : Modify color table.",pattern,true);
  list("  dr ? [<pattern>] | off | <key> ( off | on | once ) : Send debug request.",pattern,true);
  list("  get ? [<pattern>] | <key> [?]: Show debug data or show its specification.",pattern,true);
  list("  jc hide | show | ( motion | [press | release] <button> ) <command> : Set joystick motion (use $1 .. $6) or button command.",pattern,true);
  list("  js <axis> <speed> <threshold> : Set axis maximum speed and ignore threshold for command \"jc motion\".",pattern,true);
  list("  log start | stop | clear | save <file> | full | jpeg : Record log file and (de)activate image compression.",pattern,true);
  list("  log saveImages (raw) <file> : Save images from log.",pattern,true);
  list("  log ? | load <file> | ( keep | remove ) <message> {<message>} : Load, filter, and display information about log file.",pattern,true);
  list("  log start | pause | stop | forward | backward | repeat | goto <number> | cycle | once : Replay log file.",pattern,true);
  list("  mof : Recompile motion net and send it to the robot. ",pattern,true);
  list("  msg off | on | log <file> | enable | disable : Switch output of text messages on or off. Log text messages to a file. Switch message handling on or off.",pattern,true);
  list("  mr ? [<pattern>] | modules [<pattern>] | save | <representation> ( ? [<pattern>] | <module> | off ) : Send module request.",pattern,true);
  list("  mv <x> <y> <z> [<rotx> <roty> <rotz>] : Move the selected simulated robot to the given position.",pattern,true);
  list("  poll : Poll for all available debug requests and drawings. ",pattern,true);
  list("  qfr queue | replace | reject | collect <seconds> | save [<seconds>] : Send queue fill request.",pattern,true);
  list("  set ? [<pattern>] | <key> ( ? | unchanged | <data> ) : Change debug data or show its specification.",pattern,true);
  list("  v3 ? [<pattern>] | <image> [jpeg] [<name>] : Add a set of 3-D views for a certain image.",pattern,true);
  list("  vf <name> : Add field view.",pattern,true);
  list("  vfd ? [<pattern>] | <name> ( ? [<pattern>] | <drawing> ( on | off ) ) : (De)activate debug drawing in field view.",pattern,true);
  list("  vi ? [<pattern>] | <image> [jpeg] [segmented] [<name>] : Add image view.",pattern,true);
  list("  vid ? [<pattern>] | <name> ( ? [<pattern>] | <drawing> ( on | off ) ) : (De)activate debug drawing in image view.",pattern,true);
  list("  vp <name> <numOfValues> <minValue> <maxValue> [<yUnit> <xUnit> <xScale>]: Add plot view.",pattern,true);
  list("  vpd ? [<pattern>] | <name> ( ? [<pattern>] | <drawing> ( ? [<pattern>] | <color> [<description>] | off ) ) : Plot data in a certain color in plot view.",pattern,true);
  list("  xbb ( ? [<pattern>] | unchanged | <behavior> {<num>} ) : Select Xabsl basic behavior.",pattern,true);
  list("  xis ( ? [<pattern>] | <inputSymbol> ( on | off ) ) : Show Xabsl input symbol.",pattern,true);
  list("  xo ( ? [<pattern>] | unchanged | <option> {<num>} ) : Select Xabsl option.",pattern,true);
  list("  xos ( ? [<pattern>] | <outputSymbol> ( on | off | ? [<pattern>] | unchanged | <value> ) ) : Show or set Xabsl output symbol.",pattern,true);
  list("  xsb : Send current Xabsl behavior to robot.",pattern,true);
}

void ConsoleRoboCupCtrl::echo(InConfigMemory& stream)
{
  bool first = true;
  while(!stream.eof())
  {
    std::string text;
    stream >> text;
    if(first)
      first = false;
    else
      print(" ");
    print(text);
  }
  printLn("");
}

bool ConsoleRoboCupCtrl::startRemote(InConfigMemory& stream)
{
  std::string name,
              ip;
  stream >> name >> ip;
  robotName = std::string(".") + name;
  mode = SystemCall::remoteRobot;
  RemoteRobot* rr = new RemoteRobot(name.c_str(), ip.c_str());
  if(!rr->isClient())
  {
    if(ip != "")
    {
      delete rr;
      printLn(std::string("No connection to ") + ip + " established!");
      return false;
    }
    else
      printLn("Waiting for a connection...");
  }
  selected.clear();
  remoteRobots.push_back(rr);
  selected.push_back(remoteRobots.back());
  rr->addViews();
  rr->start();
  return true;
}

bool ConsoleRoboCupCtrl::startPhysical(InConfigMemory& stream)
{
  std::string name;
  stream >> name;
  robotName = std::string(".") + name;
  mode = SystemCall::physicalRobot;
  robots.push_back(new Robot(name.c_str(), 0));
  selected.clear();
  selected.push_back(robots.back()->getRobotProcess());
  robots.back()->start();
  return true;
}

bool ConsoleRoboCupCtrl::startLogFile(InConfigMemory& stream)
{
  std::string name,
              fileName;
  stream >> name >> fileName;
  if(int(fileName.rfind('.')) <= int(fileName.find_last_of("\\/")))
    fileName = fileName + ".log";
  if(fileName[0] != '\\' && (fileName.size() < 2 || fileName[1] != ':'))
    fileName = std::string("Logs\\") + fileName;
  {
    InBinaryFile test(fileName);
    if(!test.exists())
      return false;
  }
  robotName = std::string(".") + name;
  mode = SystemCall::logfileReplay;
  logFile = fileName;
  robots.push_back(new Robot(name.c_str(), 0));
  logFile = "";
  selected.clear();
  selected.push_back(robots.back()->getRobotProcess());
  robots.back()->start();
  return true;
}

bool ConsoleRoboCupCtrl::startTeamRobot(InConfigMemory& stream)
{
  std::string name;
  int number;
  stream >> name >> number;
  if(!number)
    return false;

  robotName = std::string(".") + name;
  TeamRobot* tr = new TeamRobot(name.c_str(), number);
  teamRobots.push_back(tr);
  selected.clear();
  selected.push_back(teamRobots.back());
  tr->addViews();
  tr->start();
  return true;
}

bool ConsoleRoboCupCtrl::setReleaseOptions(InConfigMemory& stream)
{
  std::string option,
              state;
  stream >> option >> state;
  if(option == "stopwatch")
    if (state == "off")
      releaseOptions.stopwatch = 0;
    else if(state == "")
      return false;
    else
      releaseOptions.stopwatch = state[0];
  else if(option == "sensorData")
    if(state == "on" || state == "")
      releaseOptions.sensorData = true;
    else if (state == "off")
      releaseOptions.sensorData = false;
    else
      return false;
  else if(option == "robotHealth")
    if(state == "on" || state == "")
      releaseOptions.robotHealth = true;
    else if (state == "off")
      releaseOptions.robotHealth = false;
    else
      return false;
  else if(option == "motionRequest")
    if(state == "on" || state == "")
      releaseOptions.motionRequest = true;
    else if (state == "off")
      releaseOptions.motionRequest = false;
    else
      return false;
  else if(option == "linePercept")
    if(state == "on" || state == "")
      releaseOptions.linePercept = true;
    else if (state == "off")
      releaseOptions.linePercept = false;
    else
      return false;
  else
    return false;

  teamOut.out.bin << 0;
  teamOut.out.finishMessage(idRobot);
  teamOut.out.bin << releaseOptions;
  teamOut.out.finishMessage(idReleaseOptions);
  return true;
}

void ConsoleRoboCupCtrl::print(const std::string& text) 
{
  SYNC;
  if(newLine)
    textMessages.push_back(text);
  else
    textMessages.back() += text;
  newLine = false;
}

void ConsoleRoboCupCtrl::printLn(const std::string& text) 
{
  SYNC;
  if(newLine)
    textMessages.push_back(text);
  else
    textMessages.back() += text;
  newLine = true;
}
 
void ConsoleRoboCupCtrl::printStatusText(const std::string& text)
{
  SYNC;
  if(statusText != "")
    statusText += " | ";
  statusText += text;
}

void ConsoleRoboCupCtrl::createCompletion()
{
  const char* commands[] =
  {
    "sc",
    "sl",
    "sp",
    "su",
    "tc",
    "call",
    "cls",
    "ct off",
    "ct on",
    "ct undo",
    "ct send off",
    "ct sendAndWrite",
    "ct imageRadius",
    "ct colorSpaceRadius",
    "ct smart off",
    "dr off",
    "echo",
    "help",
    "jc motion",
    "jc hide",
    "jc show",
    "jc press",
    "jc release",
    "js",
    "st off",
    "st on",
    "dt off",
    "dt on",
    "ci off",
    "ci on",
    "log start",
    "log stop",
    "log save",
    "log saveImages",
    "log saveImages raw",
    "log clear",
    "log full",
    "log jpeg",
    "log ?",
    "log load",
    "log cycle",
    "log once",
    "log pause",
    "log forward",
    "log backward",
    "log repeat",
    "log goto",
    "mof",
    "mr modules",
    "mr save",
    "msg off",
    "msg on",
    "msg log",
    "msg enable",
    "msg disable",
    "mv",
    "poll",
    "qfr queue",
    "qfr replace",
    "qfr reject",
    "qfr collect",
    "qfr save",
    "ro sensorData off",
    "ro sensorData on",
    "ro stopwatch off",
    "ro robotHealth off",
    "ro robotHealth on",
    "ro motionRequest off",
    "ro motionRequest on",
    "ro linePercept off",
    "ro linePercept on",
    "robot all",
    "v3 image jpeg",
    "vf",
    "vi none",
    "vi image jpeg segmented",
    "vi image segmented",
    "xsb",
    "save representation:JointCalibration",
    "save representation:WalkingParameters",
    "save representation:ExpWalkingParameters3",
    "save representation:CameraCalibration",
    "save representation:SensorCalibration",
    "save representation:sideParameters",
    "save representation:rotationParameters",
    "save representation:backParameters",
    "save representation:expParameters",
  };

  SYNC;
  completion.clear();
  const int num = sizeof(commands) / sizeof(commands[0]);
  for(int i = 0; i < num; ++i)
    completion.insert(std::pair<std::string,int>(commands[i], 0));

  for(std::list<Robot*>::iterator i = robots.begin(); i != robots.end(); ++i)
    completion.insert(std::pair<std::string,int>(std::string("robot ") + (*i)->getName(), 0));
  for(std::list<RemoteRobot*>::iterator i = remoteRobots.begin(); i != remoteRobots.end(); ++i)
    completion.insert(std::pair<std::string,int>(std::string("robot ") + (*i)->getName(), 0));
  for(std::list<TeamRobot*>::iterator i = teamRobots.begin(); i != teamRobots.end(); ++i)
    completion.insert(std::pair<std::string,int>(std::string("robot ") + (*i)->getName(), 0));

  for(int i = 1; i < numOfMessageIDs; ++i)
    if(std::string(getMessageIDName(MessageID(i))) != "unknown")
    {
      completion.insert(std::pair<std::string,int>(std::string("log keep ") + getMessageIDName(MessageID(i)), 0));
      completion.insert(std::pair<std::string,int>(std::string("log remove ") + getMessageIDName(MessageID(i)), 0));
    }

  for(int color = ColorClasses::none; color < ColorClasses::numOfColors; ++color)
  {
    completion.insert(std::pair<std::string,int>(std::string("ct ") + translate(ColorClasses::getColorName((ColorClasses::Color) color)) + " add", 0));
    completion.insert(std::pair<std::string,int>(std::string("ct ") + translate(ColorClasses::getColorName((ColorClasses::Color) color)) + " replace", 0));
    completion.insert(std::pair<std::string,int>(std::string("ct clear ") + translate(ColorClasses::getColorName((ColorClasses::Color) color)), 0));
    completion.insert(std::pair<std::string,int>(std::string("ct mask ") + translate(ColorClasses::getColorName((ColorClasses::Color) color)), 0));
    completion.insert(std::pair<std::string,int>(std::string("ct grow ") + translate(ColorClasses::getColorName((ColorClasses::Color) color)), 0));
    completion.insert(std::pair<std::string,int>(std::string("ct shrink ") + translate(ColorClasses::getColorName((ColorClasses::Color) color)), 0));
  }

  addCompletionFiles("log load ", std::string(File::getGTDir()) + "\\Config\\Logs\\*.log");
  addCompletionFiles("log save ", std::string(File::getGTDir()) + "\\Config\\Logs\\*.log");
  addCompletionFiles("call ", "*.con");
  addCompletionFiles("ct load ", std::string(File::getGTDir()) + "\\Config\\" + settings.expandLocationFilename("*.c64"));
  addCompletionFiles("ct save ", std::string(File::getGTDir()) + "\\Config\\" + settings.expandLocationFilename("*.c64"));

  if(moduleInfo)
  {
    for(std::list<ModuleInfo::Provider>::const_iterator i = moduleInfo->providers.begin(); i != moduleInfo->providers.end(); ++i)
    {
      completion.insert(std::pair<std::string,int>(std::string("mr ") + i->representation + " off", 0));
      for(std::vector<std::string>::const_iterator j = i->modules.begin(); j != i->modules.end(); ++j)
        completion.insert(std::pair<std::string,int>(std::string("mr ") + i->representation + " " + *j, 0));
    }
  }

  if(debugRequestTable)
  {
    for(int i = 0; i < debugRequestTable->currentNumberOfDebugRequests; ++i)
    {
      completion.insert(std::pair<std::string,int>(std::string("dr ") + translate(debugRequestTable->debugRequests[i].description) + " on", 0));
      completion.insert(std::pair<std::string,int>(std::string("dr ") + translate(debugRequestTable->debugRequests[i].description) + " off", 0));
      completion.insert(std::pair<std::string,int>(std::string("dr ") + translate(debugRequestTable->debugRequests[i].description) + " once", 0));
      if(debugRequestTable->debugRequests[i].description.substr(0, 13) == "debug images:")
      {
        completion.insert(std::pair<std::string,int>(std::string("v3 ") + translate(debugRequestTable->debugRequests[i].description.substr(13)) + " jpeg", 0));
        completion.insert(std::pair<std::string,int>(std::string("vi ") + translate(debugRequestTable->debugRequests[i].description.substr(13)) + " jpeg segmented", 0));
        completion.insert(std::pair<std::string,int>(std::string("vi ") + translate(debugRequestTable->debugRequests[i].description.substr(13)) + " segmented", 0));
      }
      else if(debugRequestTable->debugRequests[i].description.substr(0, 11) == "debug data:")
      {
        completion.insert(std::pair<std::string,int>(std::string("get ") + translate(debugRequestTable->debugRequests[i].description.substr(11)) + " ?", 0));
        completion.insert(std::pair<std::string,int>(std::string("set ") + translate(debugRequestTable->debugRequests[i].description.substr(11)) + " ?", 0));
        completion.insert(std::pair<std::string,int>(std::string("set ") + translate(debugRequestTable->debugRequests[i].description.substr(11)) + " unchanged", 0));
      }
    }
  }

  if(drawingManager)
  {
    for(DrawingManager::DrawingNameIdTable::const_iterator i = drawingManager->drawingNameIdTable.begin();
        i != drawingManager->drawingNameIdTable.end(); ++i)
    {
      if(!strcmp(drawingManager->getDrawingType(i->first), "drawingOnImage") && imageViews)
        for(RobotConsole::Views::const_iterator j = imageViews->begin(); j != imageViews->end(); ++j)
          completion.insert(std::pair<std::string,int>(std::string("vid ") + j->first + " " + translate(i->first), 0));
      else if(!strcmp(drawingManager->getDrawingType(i->first), "drawingOnField") && fieldViews)
        for(RobotConsole::Views::const_iterator j = fieldViews->begin(); j != fieldViews->end(); ++j)
          completion.insert(std::pair<std::string,int>(std::string("vfd ") + j->first + " " + translate(i->first), 0));
    }
  }

  if(plotViews)
    for(RobotConsole::PlotViews::const_iterator i = plotViews->begin(); i != plotViews->end(); ++i)
      for(int j = 0; j < debugRequestTable->currentNumberOfDebugRequests; ++j)
        if(translate(debugRequestTable->debugRequests[j].description).substr(0, 5) == "plot:")
        {
          for(int color = 1; color < ColorClasses::numOfColors; ++color)
            completion.insert(std::pair<std::string,int>(std::string("vpd ") + i->first + " " + 
                              translate(debugRequestTable->debugRequests[j].description).substr(5) + " " +
                              translate(ColorClasses::getColorName((ColorClasses::Color) color)), 0));
          completion.insert(std::pair<std::string,int>(std::string("vpd ") + i->first + " " + 
                            translate(debugRequestTable->debugRequests[j].description).substr(5) + " off", 0));
        }

  if(xabslInfo)
  {
    completion.insert(std::pair<std::string,int>(std::string("xbb ") + "unchanged", 0));
    for(std::list<XabslInfo::BasicBehavior>::const_iterator i = xabslInfo->basicBehaviors.begin(); i !=xabslInfo->basicBehaviors.end(); ++i)
      completion.insert(std::pair<std::string,int>(std::string("xbb ") + i->name, 0));
    for(std::list<XabslInfo::InputSymbol>::const_iterator i = xabslInfo->inputSymbols.begin(); i != xabslInfo->inputSymbols.end(); ++i)
      if (i->decimalParameters.size() == 0 &&
          i->booleanParameters.size() == 0 &&
          i->enumeratedParameters.size() == 0)
      {
        completion.insert(std::pair<std::string,int>(std::string("xis ") + i->name + " off", 0));
        completion.insert(std::pair<std::string,int>(std::string("xis ") + i->name + " on", 0));
      }
    for(std::list<XabslInfo::OutputSymbol>::const_iterator i = xabslInfo->outputSymbols.begin(); i != xabslInfo->outputSymbols.end(); ++i)
    {
      completion.insert(std::pair<std::string,int>(std::string("xos ") + i->name + " off", 0));
      completion.insert(std::pair<std::string,int>(std::string("xos ") + i->name + " on", 0));
      completion.insert(std::pair<std::string,int>(std::string("xos ") + i->name + " unchanged", 0));
      if (i->type == XabslInfo::boolean)
      {
        completion.insert(std::pair<std::string,int>(std::string("xos ") + i->name + " true", 0));
        completion.insert(std::pair<std::string,int>(std::string("xos ") + i->name + " false", 0));
      } 
      else if (i->type == XabslInfo::enumerated)
      {
        for(std::list<std::string>::const_iterator j = i->enumeration->elements.begin(); j != i->enumeration->elements.end(); ++j)
          if(!strncmp((i->name + ".").c_str(), j->c_str(), i->name.length() + 1))
            completion.insert(std::pair<std::string,int>(std::string("xos ") + i->name + " " + j->substr(i->name.length() + 1), 0));
          else
            completion.insert(std::pair<std::string,int>(std::string("xos ") + i->name + " " + *j, 0));
      }
    }
    completion.insert(std::pair<std::string,int>(std::string("xo ") + "unchanged", 0));
    for(std::list<XabslInfo::Option>::const_iterator i = xabslInfo->options.begin(); i != xabslInfo->options.end(); ++i)
      completion.insert(std::pair<std::string,int>(std::string("xo ") + i->name, 0));
  }
}

void ConsoleRoboCupCtrl::addCompletionFiles(const std::string& command, const std::string& pattern)
{
  Directory dir;
  if(!dir.open(pattern))
    return;

  std::string fileName;
  bool isDir;
  while(dir.read(fileName, isDir))
  {
    fileName = fileName.substr(fileName.rfind('\\') + 1);
    fileName = fileName.substr(0, fileName.rfind('.'));
    completion.insert(std::pair<std::string,int>(command + fileName, 0));
  }
}

void ConsoleRoboCupCtrl::onConsoleCompletion(std::string& command, bool forward, bool nextSection)
{
  SYNC;
  std::string separators = " :";
  char endSeparator = separators[0];
  if(!nextSection && command.size() && separators.find(command[command.size() - 1]) != std::string::npos)
  {
    endSeparator = command[command.size() - 1];
    command = command.substr(0, command.size() - 1);
  }
  std::string constantPart;
  int n = command.find_last_of(separators);
  char separator = ' ';
  if(n != -1)
  {
    separator = command[n];
    constantPart = command.substr(0,n);
  }

  if(forward)
  {
    std::map<std::string, int>::const_iterator i = completion.lower_bound(command + endSeparator + 'z');
    if(i == completion.end() || constantPart != i->first.substr(0, constantPart.length()))
    {
      i = completion.lower_bound(constantPart + separator);
      if(i == completion.end() || constantPart != i->first.substr(0, constantPart.length()))
        return;
    }
    command = i->first.substr(constantPart.length());
  }
  else
  {
    std::map<std::string, int>::const_iterator i = completion.lower_bound(command);
    if(i != completion.begin())
      --i;
    if(i == completion.begin() || constantPart != i->first.substr(0, constantPart.length()))
    {
      i = completion.lower_bound(constantPart + "zzzzzzzzzzz");
      --i;
      if(i == completion.begin() || constantPart != i->first.substr(0, constantPart.length()))
        return;
    }
    command = i->first.substr(constantPart.length());
  }
  while(command.length() > 0 && separators.find(command[0]) != std::string::npos)
    command = command.substr(1);
  n = command.find_first_of(separators);
  char separator2 = ' ';
  if(n == -1)
    n = command.size();
  else
    separator2 = command[n];
  command = (constantPart == "" ? "" : constantPart + separator) + command.substr(0, n) + separator2;
}

void ConsoleRoboCupCtrl::list(const std::string& text, const std::string& required, bool newLine)
{
  std::string s1(text), s2(required);
  for(std::string::iterator i = s1.begin(); i != s1.end(); ++i)
    *i = toupper(*i);
  for(std::string::iterator i = s2.begin(); i != s2.end(); ++i)
    *i = toupper(*i);
  if(s1.find(s2) != std::string::npos)
  {
    if(newLine)
    {
      printLn(text);
    }
    else
    {
      print(text + " ");
    }
  }
}

std::string ConsoleRoboCupCtrl::translate(const std::string& text) const
{
  std::string s(text);
  for(unsigned i = 0; i < s.size(); ++i)
    if(s[i] == ' ' || s[i] == '-')
    {
      s = s.substr(0, i) + s.substr(i + 1);
      if(i < s.size())
      {
        if(s[i] >= 'a' && s[i] <= 'z')
          s[i] = s[i] - 32;
        --i;
      }
    }
    else if(i < s.size() - 1 && s[i] == ':' && s[i + 1] == ':')
      s = s.substr(0, i) + s.substr(i + 1);
  return s;
}

bool ConsoleRoboCupCtrl::handleMessage(InMessage& message)
{
  switch(message.getMessageID())
  {
  case idReceiveTimeStamp:
    message.bin >> timeStamp;
    return true;

  case idRobot:
    message.bin >> robotNumber;
    ntp.setReceiveTimeStamp(robotNumber, timeStamp);
    return true;

  case idSendTimeStamp:
    {
      unsigned timeStamp;
      message.bin >> timeStamp;
      ntp.setSendTimeStamp(robotNumber, timeStamp);
      return true;
    }

  default:
    if(!ntp.handleMessage(message))
      for(std::list<TeamRobot*>::iterator i = teamRobots.begin(); i != teamRobots.end(); ++i)
        if((*i)->getNumber() == robotNumber)
        {
          (*i)->setGlobals();
          (*i)->handleMessage(message);
          message.resetReadPosition();
        }

    return true;
  }
}
