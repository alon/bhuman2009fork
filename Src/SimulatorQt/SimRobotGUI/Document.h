/** 
* @file SimRobotGUI/Document.h
* Declaration of class Document.
* @author Colin Graf
*/

#ifndef Document_H
#define Document_H

#include <QObject>
#include <QLinkedList>

#include <Controller/Controller.h>
#include <Simulation/APIDatatypes.h>

class QMainWindow;
class QSettings;
class Simulation;

class SceneObject
{
public:
  enum Type
  {
    None = OBJECT_TYPE_NONE,
    Object = OBJECT_TYPE_OBJECT,
    Sensor = OBJECT_TYPE_SENSORPORT, 
    Actuator = OBJECT_TYPE_ACTUATORPORT,
    SceneGraph,
    Console,
    Actuators,
    Editor,
  };

  QString name;
  Type type;

  SceneObject() : type(None) {}
  SceneObject(const SceneObject& other) : name(other.name), type(other.type) {}
  SceneObject(const QString& name, Type type) : name(name), type(type) {}

  bool operator==(const SceneObject& other) const
  {
    return name == other.name && type == other.type;
  }
};

/** 
* @class Document
* A global accessible class for managing an opened document.
*/
class Document : public QObject
{
  Q_OBJECT

public:
  static Document* document;

  Document(QMainWindow* parent);
  ~Document();
  const QString& getFile() const;
  QString getUserFriendlyFile() const;
  const QString& getStatusMessage() const;
  bool openObject(const SceneObject& object);
  bool raiseObject(const SceneObject& object);
  bool closeObject(const SceneObject& object);
  Simulation* getSimulation() const;
  void setUpdateViewsDelay(unsigned int ms);
  const std::vector<ObjectDescription>& getObjectTree() const;
  QSettings* getLayoutSettings();
  void setStatusText(const QString& text);
  void print(const QString& text);
  void clearConsole();
  void handleConsoleCommand(const QString& text);
  void completeConsoleCommand(QString& text, bool forward, bool nextSection);
  bool isOpened() const;
  bool isRunning() const;
  bool isLayoutRestored() const { return restoredLayout; }
  bool handleKeyEvent(int key, bool pressed);
  void newFile();
  bool openFile(const QString& fileName, QString& error);
  const QString& getFileContent() const;
  bool saveFile(const QString& fileName, QString& error);
  void setFileContent(const QString& fileContent);
  void close();
  void simReset();
  void simStart();
  void simStop();
  void simStep();

signals:
  void openedSignal();
  void startedSim();
  void pausedSim();
  void aboutToCloseSignal();
  void closeSignal();
  void updatedSceneGraph();
  void aboutToSave();
  void saved();
  void openObjectSignal(const SceneObject& object);
  void raiseObjectSignal(const SceneObject& object);
  void closeObjectSignal(const SceneObject& object);
  void updateStatusBarSignal();
  void updateViewsSignal();
  void updateActuatorsSignal();
  void restoreLayoutSignal();
  void writeLayoutSignal();
  void clearConsoleSignal();
  void printSignal(const QString& text);

private:
  QString file;
  QString fileContent;
  bool opened;
  bool restoredLayout;
  bool contentChanged;
  Simulation* simulation;
  Controller* controller;
  bool running;
  bool performStep;
  bool updateViews;
  unsigned int lastUpdateViewsTime;
  unsigned int updateViewsDelay;
  ErrorDescription errorDescription;
  QString statusMessage;
  QString controllerStatusMessage;
  SimulatorGui simulatorGui;
  std::vector<ObjectDescription> objectDescriptionTree;
  QSettings* layoutSettings;
  QLinkedList<SceneObject> openedObjects; /**< Opened objects in window z-order. */

  void compile();
  void timerEvent(QTimerEvent* event);
  void initDocument();
  void restoreLayout();
  void writeLayout();
};

#endif // Document_H
