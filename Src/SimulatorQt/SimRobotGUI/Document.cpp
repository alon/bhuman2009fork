/** 
* @file SimRobotGUI/Document.cpp
* Implementation of class Document.
* @author Colin Graf
*/

#include <QMainWindow>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>
#include <QSettings>
#include <QApplication>
#include <QTimer>

#include <Simulation/Simulation.h>

#include "Main.h"
#include "Document.h"

Connection* Connection::start = 0;
Simulation* Connection::simulation;
SimulatorGui* Connection::simulatorGui;

Connection::Connection(const char* sceneName)
: sceneName(sceneName)
{
  next = start;
  start = this;
}

void SimulatorGui::print(const std::string& text)
{
  Document::document->print(text.c_str());
}

void SimulatorGui::clear()
{
  Document::document->clearConsole();
}

void SimulatorGui::setUpdateDelay(unsigned ms)
{
  Document::document->setUpdateViewsDelay(ms);
}

void SimulatorGui::setStatusText(const std::string& text)
{
  Document::document->setStatusText(text.c_str());
}

Document* Document::document = 0;

Document::Document(QMainWindow* parent) : QObject(parent), 
  opened(false), restoredLayout(false), contentChanged(false),
  simulation(0), controller(0), running(false), performStep(false), updateViews(false),
  lastUpdateViewsTime(0), updateViewsDelay(100), layoutSettings(0)
{
  Q_ASSERT(document == 0);
  document = this;
}

Document::~Document()
{
  Q_ASSERT(!opened);
  Q_ASSERT(!simulation);
  Q_ASSERT(!controller);
  Q_ASSERT(!running);
  Q_ASSERT(!layoutSettings);

  Q_ASSERT(document == this);
  document = 0;
}

void Document::newFile()
{
  close();
  file.clear();
  fileContent.clear();

  statusMessage = tr("File created");
  emit updateStatusBarSignal();

  initDocument();
}

bool Document::openFile(const QString& fileName, QString& error)
{
  close();

  QFile xmlFile(fileName);
  if(!xmlFile.open(QFile::ReadOnly | QFile::Text))
  {
     error = tr("Cannot read file %1:\n%2.").arg(fileName).arg(xmlFile.errorString());
     return false;
  }

  QFileInfo fileInfo(fileName);
  QTextStream in(&xmlFile);
  file = fileInfo.absoluteDir().canonicalPath() + '/' + fileInfo.fileName();
  fileContent = in.readAll(); 

  statusMessage = tr("File loaded");
  emit updateStatusBarSignal();

  initDocument();
  return true;
}

bool Document::saveFile(const QString& fileName, QString& error)
{
  emit aboutToSave(); // this may changed contentChanged

  bool fileNameChanged = fileName != file;

  if(!fileNameChanged && !contentChanged)
    return true;
  
  QFile xmlFile(fileName);
  if(!xmlFile.open(QFile::WriteOnly | QFile::Text))
  {
     error = tr("Cannot write file %1:\n%2.").arg(fileName).arg(xmlFile.errorString());
     return false;
  }

  QFileInfo fileInfo(fileName);
  QTextStream out(&xmlFile);
  file = fileInfo.absoluteDir().canonicalPath() + '/' + fileInfo.fileName();
  out << fileContent;
  contentChanged = false;

  if(fileNameChanged)
  {
    if(layoutSettings)
      delete layoutSettings;
    layoutSettings = new QSettings("B-Human", Main::main->appString);
    layoutSettings->beginGroup("Layouts");
    layoutSettings->beginGroup(file.isEmpty() ? "default" : getUserFriendlyFile());
  }

  emit saved();
  statusMessage = tr("File saved");
  emit updateStatusBarSignal();
  return true;
}

bool Document::isOpened() const
{
  return opened;
}

bool Document::isRunning() const
{
  return opened && running;
}

void Document::initDocument()
{
  opened = true;
  Q_ASSERT(!running);
  running = false;
  performStep = false;
  updateViews = false;
  contentChanged = false;
  Q_ASSERT(!layoutSettings);

  layoutSettings = new QSettings("B-Human", Main::main->appString);
  layoutSettings->beginGroup("Layouts");
  layoutSettings->beginGroup(file.isEmpty() ? "default" : getUserFriendlyFile());

  emit openedSignal();

  restoreLayout();
}
void Document::close()
{
  if(opened)
  {
    restoredLayout = false;
    emit aboutToCloseSignal();
    writeLayout();
    for(QLinkedList<SceneObject>::Iterator i = openedObjects.begin(), end  = openedObjects.end(); i != end; i = openedObjects.begin())
    {
      SceneObject obj(*i);
      openedObjects.erase(i);
      emit closeObjectSignal(obj);
    }
    file.clear();
    fileContent.clear();
    opened = false;
    contentChanged = false;
    if(running)
    {
      running = false;
      emit pausedSim();
    }
    performStep = false;
    updateViews = false;
    Q_ASSERT(layoutSettings);
    delete layoutSettings;
    layoutSettings = 0;
    
    if(controller)
    {
      controller->destroy();
      delete controller;
      controller = 0;
    }
    if(simulation)
    {
      delete simulation;
      simulation = 0;
      objectDescriptionTree.clear();
      emit updatedSceneGraph();
    }
    clearConsole();
    emit closeSignal();
    statusMessage = tr("File closed");
    emit updateStatusBarSignal();

    killTimer(0);
    QApplication::processEvents();
  }
}

const QString& Document::getFile() const
{
  return file;
}

QString Document::getUserFriendlyFile() const
{
  return QFileInfo(file).fileName();
}

const QString& Document::getStatusMessage() const
{
  if(controllerStatusMessage.isEmpty())
    return statusMessage;
  return controllerStatusMessage;
}

void Document::simReset() 
{
  bool wasRunning = running;
  simStop();
  if(controller)
  {
    controller->destroy();
    delete controller;
    controller = 0;
  }
  if(simulation)
  {
    delete simulation;
    simulation = 0;
    objectDescriptionTree.clear();
    emit updatedSceneGraph();
  }
  compile();
  if(simulation && wasRunning)
    simStart();
}

void Document::simStop()
{
  if(running)
  {
    running = false;
    emit pausedSim();
  }
}

void Document::simStart() 
{
  if(running)
    return;

  if(!simulation)
    simReset();
  if(simulation && !running)
  {
    running = true;
    startTimer(0);
    emit startedSim();
  }
}

void Document::simStep() 
{
  if(running)
  {
    simStop();
    performStep = true;
    return;
  }
  if(!simulation)
    simReset();
  if(simulation)
  {
    performStep = true;
    startTimer(0);
  }
}

void Document::compile()
{
  unsigned int startTime = Main::getSystemTime();

  // check if document has a name
  if(file.isEmpty())
    return;
  
  // set directory for relative paths in texture load commands
  QDir::setCurrent(QFileInfo(file).dir().path());
  QString roSiFilePath = QFileInfo(Main::main->appPath).dir().path() + "/RoSi.xsd";

  Q_ASSERT(simulation == 0);
  Q_ASSERT(controller == 0);

  // compile
  simulation = new Simulation();
  // check for an error
  std::string stdFile(file.toAscii().constData());
  std::string stdRoSiFile(roSiFilePath.toAscii().constData());
  if(!simulation->loadFile(stdFile, stdRoSiFile))
  {
    std::vector<ErrorDescription> errors;
    simulation->getFirstError(errorDescription);
    simulation->getAllErrors(errors);
    statusMessage = tr(errorDescription.text.c_str());
    emit updateStatusBarSignal();
    delete simulation;
    simulation = 0;
    QApplication::beep();
    std::vector<ErrorDescription>::const_iterator it;
    print("----------------------\n");
    print("Scene Compiler Errors:\n");
    print(statusMessage + "\n");
    for(it = errors.begin(); it < errors.end(); it++)
      print(((*it).text+"\n").c_str());
    print("----------------------\n");
  }
  else
  {
    // print name of scene and description
    print((std::string("Loaded scene ")+simulation->getSceneName()+":\n").c_str());
    print((simulation->getSceneDescription()+"\n\n").c_str());
    // search for controller
    SimObject* objectTree = simulation->getObjectReference("");
    const std::string& sceneName(objectTree->getName());
    Connection* pThis = Connection::start;
    while (pThis && !(sceneName == pThis->sceneName))
      pThis = pThis->next;
    if(pThis)
    {
      controller = pThis->createController(*simulation, simulatorGui);
      controller->init();
      simulation->getObjectDescriptionTree(objectDescriptionTree);
      simulation->resetSceneGraphChanged();
      emit updatedSceneGraph();
      statusMessage = tr("Compilation successful (%1ms)").arg(Main::getSystemTime() - startTime);
      emit updateStatusBarSignal();
    }
    else
    {
      simulation->getObjectDescriptionTree(objectDescriptionTree);
      simulation->resetSceneGraphChanged();
      emit updatedSceneGraph();
      statusMessage = tr("There's no controller for this scene");
      emit updateStatusBarSignal();
    }    
  }
}

Simulation* Document::getSimulation() const
{
  return simulation;
}

bool Document::openObject(const SceneObject& object)
{
  if(!opened)
    return false;

  for(QLinkedList<SceneObject>::iterator i = openedObjects.begin(), end = openedObjects.end(); i != end; ++i)
    if(*i == object)
    {
      emit raiseObjectSignal(object);
      return true;
    }

  openedObjects.append(object);
  emit openObjectSignal(object);
  return true;
}

bool Document::raiseObject(const SceneObject& object)
{
  for(QLinkedList<SceneObject>::iterator i = openedObjects.begin(), end = openedObjects.end(); i != end; ++i)
    if(*i == object)
    {
      openedObjects.erase(i);
      openedObjects.append(object);
      return true;
    }
  return false;
}

bool Document::closeObject(const SceneObject& object)
{
  for(QLinkedList<SceneObject>::iterator i = openedObjects.begin(), end = openedObjects.end(); i != end; ++i)
    if(*i == object)
    {
      openedObjects.erase(i);
      emit closeObjectSignal(object);
      return true;
    }
  return false;
}

void Document::timerEvent(QTimerEvent* event) 
{
  if(performStep || running)
  {
    performStep = false;
    if(controller)
    {
      controller->execute();
      if(simulation->getResetFlag())
        simulation->resetSimulation();
      if(simulation->hasSceneGraphChanged())
      {
        simulation->getObjectDescriptionTree(objectDescriptionTree);
        simulation->resetSceneGraphChanged();
        emit updatedSceneGraph();
      }
    }
    emit updateActuatorsSignal();
    simulation->doSimulationStep();
    updateViews = true;
  }

  if(updateViews)
  {
    unsigned int t = Main::getSystemTime();
    if(t - lastUpdateViewsTime >= updateViewsDelay)
    {      
      lastUpdateViewsTime = t;
      updateViews = false;
      emit updateViewsSignal();
      emit updateStatusBarSignal();
    }
  }
  else
    killTimer(event->timerId());
}

void Document::setUpdateViewsDelay(unsigned int ms)
{
  updateViewsDelay = ms;
}

const std::vector<ObjectDescription>& Document::getObjectTree() const
{
  return objectDescriptionTree;
}

QDataStream& operator>>(QDataStream& s, SceneObject& obj)
{
  int type;
  s >> obj.name >> type;
  obj.type = SceneObject::Type(type);
  return s;
}

QDataStream& operator<<(QDataStream& s, const SceneObject& obj)
{
  s << obj.name << int(obj.type);
  return s;
}

void Document::restoreLayout()
{
  Q_ASSERT(layoutSettings);
  Q_ASSERT(openedObjects.size() == 0);

  const QByteArray& buffer(layoutSettings->value("Views").toByteArray());
  QDataStream in(buffer);
  in.setVersion(QDataStream::Qt_4_3);
  in >> openedObjects;

  if(layoutSettings->value("Version", 1).toInt() != 2)
    openedObjects.clear();
  
  bool hasSceneGraph = false, hasConsole = false, hasEditor = false;
  for(QLinkedList<SceneObject>::Iterator i = openedObjects.begin(), end = openedObjects.end(); i != end; ++i)
  {
    emit openObjectSignal(*i);
    switch(i->type)
    {
    case SceneObject::SceneGraph:
      hasSceneGraph = true;
      break;
    case SceneObject::Console:
      hasConsole = true;
      break;
    case SceneObject::Editor:
      hasEditor = true;
      break;
    default:
      break;
    }
  }
  if(!hasSceneGraph)
  {
    openedObjects.append(SceneObject(".SceneGraph", SceneObject::SceneGraph));
    emit openObjectSignal(openedObjects.last());
  }
  if(!hasConsole)
  {
    openedObjects.append(SceneObject(".Console", SceneObject::Console));
    emit openObjectSignal(openedObjects.last());
  }
  if(!hasEditor)
  {
    openedObjects.append(SceneObject(".Editor", SceneObject::Editor));
    emit openObjectSignal(openedObjects.last());
  }

  emit restoreLayoutSignal();
  restoredLayout = true;

  if(layoutSettings->value("Compile", false).toBool())
    compile();

  if(simulation)
  {
    if(layoutSettings->value("Run", false).toBool())
      simStart();
  }
}

void Document::writeLayout()
{
  Q_ASSERT(layoutSettings);
  if(file.isEmpty())
    return;

  layoutSettings->setValue("Version", 2);
  layoutSettings->setValue("Compile", simulation ? true : false);
  layoutSettings->setValue("Run", running);

  QByteArray buffer;
  QDataStream out(&buffer, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_3);
  out << openedObjects;
  layoutSettings->setValue("Views", buffer);

  emit writeLayoutSignal();
}

QSettings* Document::getLayoutSettings()
{
  return layoutSettings;
}

void Document::print(const QString& text)
{    
  emit printSignal(text);
}

void Document::clearConsole()
{
  emit clearConsoleSignal();
}

void Document::setStatusText(const QString& text)
{
  controllerStatusMessage = text;
}

void Document::handleConsoleCommand(const QString& text)
{
  if(controller)
    controller->onConsoleCommand(text.toAscii().constData());
}

void Document::completeConsoleCommand(QString& text, bool forward, bool nextSection)
{
  if(controller)
  {
    std::string cmd(text.toAscii().constData());
    controller->onConsoleCompletion(cmd, forward, nextSection);
    text = cmd.c_str();
  }
}

bool Document::handleKeyEvent(int key, bool pressed)
{
  if(controller)
  {
    if(key >= Qt::Key_0 && key <= Qt::Key_9)
    {
      controller->onKeyEvent(key - Qt::Key_0, pressed);
      return true;
    }
    else if(key >= Qt::Key_A && key <= Qt::Key_Z)
    {
      controller->onKeyEvent(key - Qt::Key_A + 11, pressed);
      return true;
    }
  }
  return false;
}

const QString& Document::getFileContent() const
{
  return fileContent;
}

void Document::setFileContent(const QString& fileContent)
{
  this->fileContent = fileContent;
  contentChanged = true;
}
