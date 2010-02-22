/**
* @file Controller.h
*
* Interface between user-written robot controllers
* and the simulator.
*
* @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
*/

#ifndef Controller_H
#define Controller_H

#include <string>
#include <vector>

class Simulation;
class SimulatorGui;
class SimObject;
class Surface;
class Controller3DDrawing;
class Actuator;
class PhysicsParameterSet;
class View;
class GuiControllerInterface;
class Vector3d;
class SensorReading;


/**
* The class is the interface between user-written robot controllers
* and the simulator. A user-written controller has to be derived from
* this class and the following macro has to be placed in the code:
* CONNECT_CONTROLLER_TO_SCENE(ControllerClass, "SceneName");
*/
class Controller
{
public:
  /** Constructor. */
  Controller();

  /** Destructor. */
  virtual ~Controller() {}

  /**
  * The function returns the name of the simulation file.
  * @return The file name.
  */
  std::string getSimulationFileName() const;

  /**
  * The function returns the current simulation step.
  * @return The number of the current step, starting with 0.
  */
  unsigned int getSimulationStep() const;

  /**
  * The function returns a pointer to an object.
  * An assertion will fail if the object does not exist.
  * @param objectName The full name of the object.
  * @return A pointer to the object.
  */
  SimObject* getObjectReference(const std::string& objectName);

  /** Gives direct access to actuator data. Handle with care!
  * @return A pointer to the actuator list
  */
  std::vector<Actuator*>* getActuatorList();

  /**
  * The function returns the surface with a given name.
  * An assertion will fail if the surface does not exist.
  * @param name The name of the surface searched for.
  * @return A pointer to the surface.
  */
  Surface* getSurface(const std::string& name) const;

  /**
  * The function sets the color of the background of the scene.
  * The color has to be a triple of (R,G,B) values in the range of [0..1].
  * @param color The color
  */
  void setBackgroundColor(const Vector3d& color);

  /** 
  * The function returns the id of an actuatorport that contains a given name.
  * @param parts An array of parts of the name of the actuatorport, 
  *              in which they must appear in the given sequence.
  * @return The id.
  */
  int getActuatorportId(const std::vector<std::string>& parts) const;

  /**
  * The function returns the minimum value to be set for a certain actuatorport.
  * @param id The id of the port.
  * @return The minimum value.
  */
  double getActuatorportMinValue(int id) const;

  /**
  * The function returns the maximum value to be set for a certain actuatorport.
  * @param id The id of the port.
  * @return The maximum value.
  */
  double getActuatorportMaxValue(int id) const;

  /**
  * The function sets the value of an actuatorport.
  * @param id The id of the port.
  * @param value The new value.
  */
  void setActuatorport(int id, double value);

  /** 
  * The function returns the id of an sensorport that contains a given name.
  * @param parts An array of parts of the name of the sensorport, 
  *              in which they must appear in the given sequence.
  * @return The id.
  */
  int getSensorportId(const std::vector<std::string>& parts) const;

  /** 
  * Returns a constant reference to the data computed for a sensor
  * @param id The sensor port id 
  * @return The sensor reading
  */
  const SensorReading& getSensorReading(int id); 

  /**
  * The function adds a view to the scene.
  * The view can be found in the object tree in the group "views".
  * @param The view.
  * @param name The name of the view will have in the group "views".
  */
  void addView(View* view, const std::string& name);

  /**
  * The function prints text into the console window.
  * @param text The text to be printed .
  */
  void print(const std::string& text);

  /**
  * The function prints text followed by a line break into the console window.
  * @param text The text to be printed.
  */
  void printLn(const std::string& text);

  /**
  * The function clears the console window.
  */
  void clear();

  /**
  * The function sets the text in the status bar of the application.
  * @param text The text to be printed in the status bar.
  */
  void setStatusText(const std::string& text);

  /**
  * The function sets the update delay for the GUI.
  * @param ms The delay in ms. The larger this value, the less performance 
  *           is required to update the GUI while the simulation is running.
  */
  void setGUIUpdateDelay(unsigned ms);

  /**
  * The function is called before every simulation step.
  * It must be implemented in a derived class.
  */
  virtual void execute() = 0;

  /**
  * This function is evil and will reboot the Matrix ;-)
  * Usually, no normal Controller does not need this function,
  * so please ignore it. This function is made for controllers
  * which do learning experiments inside the simulation and 
  * need to reset the whole world.
  */
  void resetSimulation();

  /** Checks the flag of the simulation core, used together with resetSimulation.
  *   After resetting, the controller needs to wait until the simulation is ready
  *   @return True, if the simulation is ready, of course.
  */
  bool isSimulationReady() const;

  /** This function changes the physical parameters while
  *   the simulator is running. This is useful for parameters optimization.
  * @param newParams The new parameters
  */
  void changePhysicsParametersAtRunTime(const PhysicsParameterSet& newParams);

  /**
   * Gets the physics parameters for the simulation of the world represented by the scene
   * @return Pointer to the world physics object
   */
  PhysicsParameterSet* getPhysicsParameters();

  /**
  * The function is called before the first call to execute().
  * When this function is called the controller is already constructed.
  */
  virtual void init() {}

  /**
  * The function is called before the controller is destructed.
  */
  virtual void destroy() {}

  /**
  * The function is called when a key on the numerical keypad or 
  * Shift+Ctrl+letter was pressed or released.
  * @param key 0..9 for the keys 0..9, 10 for the decimal point, and 
  * above for Shift+Ctrl+A-Z.
  * @param pressed Whether the key was pressed or released.
  */
  virtual void onKeyEvent(int key, bool pressed) {}

  /**
  * The function is called when a movable object has been selected.
  * @param obj The object.
  */
  virtual void onSelected(SimObject* obj) {}

  /**
  * The function is called when a console command has been entered.
  * @param command The command.
  */
  virtual void onConsoleCommand(const std::string&) {}

  /**
  * The function is called when the tabulator key is pressed.
  * It can replace the given command line by a new one.
  * @param command The command.
  * @param forward Complete in forward direction.
  * @param nextSection Progress to next section in the command.
  */
  virtual void onConsoleCompletion(std::string& command, bool forward, bool nextSection) {}

  /** 
  * Registers controller drawings at an object in the simulation scene
  * @param parts An array of parts of the name of the object, 
  *              in which they must appear in the given sequence.
  * @param drawing The drawing
  */
  void registerDrawingByPartOfName(const std::vector<std::string>& parts,
                                   Controller3DDrawing* drawing);

  /** 
  * Unregisters controller drawings at an object in the simulation scene
  * @param parts An array of parts of the name of the object, 
  *              in which they must appear in the given sequence.
  * @param drawing The drawing
  */
  void unregisterDrawingByPartOfName(const std::vector<std::string>& parts,
                                     Controller3DDrawing* drawing);

  /** 
  * Move object to target position.
  * @param object The object to move.
  * @param pos The target position.
  */
  void moveObject(SimObject* object, const Vector3d& pos);
    
  /** 
  * Move object to target position.
  * @param object The object to move.
  * @param pos The target position.
  * @param rot The target rotation.
  */
  void moveObject(SimObject* object, const Vector3d& pos, const Vector3d& rot);

  Simulation& simulation; /**< The controlled simulation. */
  SimulatorGui& simulatorGui; /**< An interface to the Gui of the Simulator. */
};

/**
 * The class encapsulates the function calls between the GUI
 * and a user-defined controller
 */
class SimulatorGui
{
public:

  /**
  * The function prints text into the console window.
  * @param text The text to be printed .
  */
  void print(const std::string& text);

  /**
  * The function clears the console window.
  */
  void clear();

  /**
  * The function sets the update delay for the GUI.
  * @param ms The delay in ms. The larger this value, the less performance 
  *           is required to update the GUI while the simulation is running.
  */
  void setUpdateDelay(unsigned ms);

  /**
  * The function sets the text in the status bar of the application.
  * @param text The text to be printed in the status bar.
  */
  void setStatusText(const std::string& text);
};

/**
* The class is a helper to connect controllers to simulation scenes.
*/
class Connection
{
public:
  static Connection* start; /**< The start of the list of all controllers. */
  static Simulation* simulation; /**< The simulation. */
  static SimulatorGui* simulatorGui; /**< An interface to the Gui of the Simulator. */
  const char* sceneName; /**< The name of the scene the controller is connected to. */
  Connection* next; /** The next entry in the list of all controllers. */

  /**
  * Constructor.
  * @param The name of the scene the controller is connected to.
  */
  Connection(const char* sceneName);

  /**
  * The function is called to create an instance of the controller.
  * @return A pointer to a new instance of the controller.
  */
  virtual Controller* createController(Simulation& simulation, SimulatorGui& simulatorGui) = 0;
};

/**
* The template class is also helper to connect controllers to simulation scenes.
* The template parameter is the controller class to construct.
*/
template <class T> class ConnectionTemplate : public Connection
{
public:
  /**
  * Constructor.
  * @param The name of the scene the controller is connected to.
  */
  ConnectionTemplate(const char* sceneName) : Connection(sceneName) {}

  /**
  * The function is called to create an instance of the controller for a simulation.
  * @param simulator The simulation.
  * @param simulatorGui An interface to the Gui of the Simulator.
  * @return A pointer to a new instance of the controller.
  */
  Controller* createController(Simulation& simulation, SimulatorGui& simulatorGui)
  {
    Connection::simulation = &simulation;
    Connection::simulatorGui = &simulatorGui;
    return new T;
  }
};

/**
* The macro connects a controller to a scene.
* @param Controller The class of the controller.
* @param sceneName The name of the scene.
*/
#define CONNECT_CONTROLLER_TO_SCENE(Controller,sceneName) \
  ConnectionTemplate<Controller> dummy##ClassName(sceneName);

#endif //Controller_H
