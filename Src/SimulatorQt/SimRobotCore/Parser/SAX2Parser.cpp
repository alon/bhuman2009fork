/**
* @file SAX2Parser.cpp
*
* Implementation of class SAX2Parser
*
* @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
* @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#include <Simulation/SimObject.h>
#include <Simulation/Simulation.h>
#include "SAX2Parser.h"
#include "LibXML2ParserImpl.h"
#include <Tools/Debug.h>
#include <Tools/Errors.h>
#include <Tools/SimGraphics.h>
#include <Actuators/Hinge.h>
#include <Actuators/Slider.h>
#include <Sensors/PressureSensor.h>
#include <Actuators/UniversalJoint.h>
#include <Actuators/WheelSuspension.h>
#include <Actuators/BallJoint.h>
#include <Actuators/Led.h>
#include <PhysicalObjects/Box.h>
#include <PhysicalObjects/ComplexShape.h>
#include <PhysicalObjects/Cylinder.h>
#include <PhysicalObjects/Sphere.h>
#include <Sensors/Camera.h>
#include <Sensors/DistanceSensor.h>
#include <Sensors/SingleDistanceSensor.h>
#include <Sensors/PositionSensor.h>
#include <Sensors/Bumper.h>
#include <Sensors/InteractiveButton.h>
#include <Sensors/Gyroscope.h>
#include <Sensors/Accelerometer.h>


SAX2Parser::SAX2Parser():parsingState(PARSING_SIMULATION), coefficientTablesGenerated(false)
{
  //Physical objects:
  majorElementNames.insert("Box");
  majorElementNames.insert("ComplexShape");
  majorElementNames.insert("Cylinder");
  majorElementNames.insert("Sphere");
  //Simple physical object, this is a HACK
  majorElementNames.insert("SimpleBox");
  majorElementNames.insert("SimpleCylinder");
  majorElementNames.insert("SimpleSphere");
  //Actuators:
  majorElementNames.insert("Hinge");
  majorElementNames.insert("Led");
  majorElementNames.insert("BallJoint");
  majorElementNames.insert("Slider");
  majorElementNames.insert("UniversalJoint");
  majorElementNames.insert("WheelSuspension");
  //Sensors:
  majorElementNames.insert("Camera");
  majorElementNames.insert("DistanceSensor");
  majorElementNames.insert("SingleDistanceSensor");
  majorElementNames.insert("PositionSensor");
  majorElementNames.insert("Bumper");
  majorElementNames.insert("PressureSensor");
  majorElementNames.insert("InteractiveButton");
  majorElementNames.insert("Accelerometer");
  majorElementNames.insert("Gyroscope");
  //Macro and Compound:
  majorElementNames.insert("Macro");
  majorElementNames.insert("Use");
  majorElementNames.insert("Compound");
}


void SAX2Parser::parse(const std::string& filename,
                       const std::string& schemaname,
                       Simulation* sim,
                       SimObject*& objectTree,
                       ErrorManager* errorManager,
                       GraphicsManager* graphicsManager,
                       std::vector<Surface*>* surfaces,
                       std::vector<Material*>* materials,
                       std::vector<Environment*>* environments)
{
  //Create and initialize object tree and internal data structures:
  objectTree = new SimObject();
  objectTree->setName("Scene");
  objectTree->setRootNode(objectTree);
  objectTree->setSimulation(sim);
  objectTree->setGraphicsManager(graphicsManager);
  currentNode = objectTree;
  rootNode = objectTree;
  simulation = sim;
  this->errorManager = errorManager;
  this->graphicsManager = graphicsManager;
  this->surfaces = surfaces;
  this->materials = materials;
  this->environments = environments;

  //Parse file with the libXML2 parser:
  LibXML2ParserImpl libxml2Impl;
  libxml2Impl.parse(filename, schemaname, this, errorManager);

  //Check for parsing errors:
  if(errorManager->getNumberOfErrors())
  {
    if(objectTree)
    {
      delete objectTree;
      objectTree = 0;
    }
  }
}


void SAX2Parser::startDocument()
{
  //Nothing to do.
  //Preparations are done by simulation kernel.
}


void SAX2Parser::endDocument()
{
  //Nothing to do.
  //Postprocessing is done by simulation kernel.
}


void SAX2Parser::startElement(const std::string& name,
                              const AttributeSet& attributeSet,
                              int line, int column)
{
  // Do this test first, since many elements are vertices
  if(parsingState == PARSING_VERTEX_LIST)
  {
    graphicsManager->addVertex(attributeSet);
  }
  else if(isMajorElement(name))
  {
    parsingState = PARSING_MAJOR_ELEMENT;
    treeStack.push(currentNode);
    if(name == "Box" || name == "SimpleBox")
    {
      Box* box = new Box(attributeSet);
      currentNode->addChildNode(box, true);
      currentNode = box;
    }
    else if(name =="ComplexShape")
    {
      ComplexShape* complexShape = new ComplexShape(attributeSet);
      currentNode->addChildNode(complexShape, true);
      currentNode = complexShape;
    }
    else if(name == "Cylinder" || name == "SimpleCylinder")
    {
      Cylinder* cylinder = new Cylinder(attributeSet);
      currentNode->addChildNode(cylinder, true);
      currentNode = cylinder;
    }
    else if(name == "Sphere" || name == "SimpleSphere")
    {
      Sphere* sphere = new Sphere(attributeSet);
      currentNode->addChildNode(sphere, true);
      currentNode = sphere;
    }
    else if(name == "Hinge")
    {
      Hinge* hinge = new Hinge(attributeSet);
      hinge->setPosition(currentNode->getPosition());
      currentNode->addChildNode(hinge, true);
      currentNode = hinge;
    }
    else if(name == "Led")
    {
      Led* led = new Led(attributeSet);
      currentNode->addChildNode(led, true);
      currentNode = led;
    }
    else if(name == "Slider")
    {
      Slider* slider = new Slider(attributeSet);
      currentNode->addChildNode(slider, true);
      currentNode = slider;
    }
    else if(name == "PressureSensor")
    {
      PressureSensor* pressure = new PressureSensor(attributeSet);
      currentNode->addChildNode(pressure, true);
      currentNode = pressure;
    }
    else if(name == "UniversalJoint")
    {
      UniversalJoint* universalJoint = new UniversalJoint(attributeSet);
      currentNode->addChildNode(universalJoint, true);
      currentNode = universalJoint;
    }
    else if(name == "WheelSuspension")
    {
      WheelSuspension* wheelSuspension = new WheelSuspension(attributeSet);
      currentNode->addChildNode(wheelSuspension, true);
      currentNode = wheelSuspension;
    }
    else if(name == "BallJoint")
    {
      BallJoint* ballJoint = new BallJoint(attributeSet);
      currentNode->addChildNode(ballJoint, true);
      currentNode = ballJoint;
    }
    else if(name == "Camera")
    {
      Camera* camera = new Camera(attributeSet);
      currentNode->addChildNode(camera, true);
      currentNode = camera;
    }
    else if(name == "DistanceSensor")
    {
      DistanceSensor* distanceSensor = new DistanceSensor(attributeSet);
      currentNode->addChildNode(distanceSensor, true);
      currentNode = distanceSensor;
    }
    else if(name == "SingleDistanceSensor")
    {
      SingleDistanceSensor* singleDistanceSensor = new SingleDistanceSensor(attributeSet);
      currentNode->addChildNode(singleDistanceSensor, true);
      currentNode = singleDistanceSensor;
    }
    else if(name == "PositionSensor")
    {
      PositionSensor* positionSensor = new PositionSensor(attributeSet);
      currentNode->addChildNode(positionSensor, true);
      currentNode = positionSensor;
    }
    else if(name == "Accelerometer")
    {
      Accelerometer* accelerometer = new Accelerometer(attributeSet);
      currentNode->addChildNode(accelerometer, true);
      currentNode = accelerometer;
    }
    else if(name == "Gyroscope")
    {
      Gyroscope* gyroscope = new Gyroscope(attributeSet);
      currentNode->addChildNode(gyroscope, true);
      currentNode = gyroscope;
    }
    else if(name == "Bumper")
    {
      Bumper* bumper = new Bumper(attributeSet);
      currentNode->addChildNode(bumper, true);
      currentNode = bumper;
    }
    else if(name == "InteractiveButton")
    {
      InteractiveButton* interactiveButton = new InteractiveButton(attributeSet);
      currentNode->addChildNode(interactiveButton, true);
      currentNode = interactiveButton;
    }
    else if(name == "Macro")
    {
      SimObject* macro = new SimObject();
      simulation->addMacro(ParserUtilities::getValueFor(attributeSet,"name"), macro);
      currentNode = macro;
    }
    else if(name == "Use")
    {
      parsingState = PARSING_USE;
      std::string macroName(ParserUtilities::getValueFor(attributeSet,"macroName"));
      if(simulation->instantiateMacro(macroName, ParserUtilities::getValueFor(attributeSet,"instanceName"), 
        currentNode))
      {
        //Check, if the macro instance has child elements. Find node to attach these elements:
        std::string newCurrentNode(ParserUtilities::getValueFor(attributeSet, "nodeForElements"));
        currentNode = simulation->getNodeInMacro(newCurrentNode);
      }
      else
      {
        errorManager->addError("Unknown Macro","The macro "+macroName+" has not been found.",line,column);
        currentNode = simulation->getNodeInMacro("");
      }
    }
    else if(name == "Compound")
    {
      SimObject* compound = new SimObject();
      if(attributeSet.size())
      {
        std::string objectName(ParserUtilities::getValueFor(attributeSet,"name"));
        compound->setName(objectName);
      }
      currentNode->addChildNode(compound, true);
      currentNode = compound;
    }
    else
    {
      assert(false); //Unimplemented object! :-(
    }
    if(parsingState != PARSING_USE)
    {
      int currentMovID = this->simulation->getCurrentMovableID();
      currentNode->setMovableID(currentMovID);

      if(currentNode->castToJoint())
      {
        this->simulation->createNewMovableID();
        if(simulation->getCurrentSpace() != simulation->getMovableSpace())
        {
          simulation->setCurrentSpace(simulation->getMovableSpace());
        }
      }
    }
  }
  else if(name == "Movable")
  {
    this->simulation->createNewMovableID();
    if(simulation->getCurrentSpace() != simulation->getMovableSpace())
    {
      simulation->setCurrentSpace(simulation->getMovableSpace());
    }
  }
  else if(name == "AppearanceDefinition")
  {
    parsingState = PARSING_SURFACE;
    Surface* surface = new Surface(attributeSet);
    surfaces->push_back(surface);
    currentSurface = surface;
  }
  else if(name == "LightDefinition")
  {
    parsingState = PARSING_LIGHT;
    Light* light = new Light(attributeSet);
    graphicsManager->defineLight(light);
    currentLight = light;
  }
  else if(name == "EnvironmentDefinition")
  {
    parsingState = PARSING_ENVIRONMENT;
    Environment* environment = new Environment(attributeSet, graphicsManager);
    environments->push_back(environment);
    currentEnvironment = environment;
  }
  else if(name == "MaterialDefinition")
  {
    parsingState = PARSING_MATERIAL;
    Material* material = new Material(attributeSet);
    material->setIndex(materials->size());
    materials->push_back(material);
    currentMaterial = material;
  }
  else if(name == "VertexList")
  {
    parsingState = PARSING_VERTEX_LIST;
    std::string listName(ParserUtilities::getValueFor(attributeSet, "name"));
    graphicsManager->newVertexList(listName, 0);
  }
  else if(name == "VertexListWithTexCoords")
  {
    parsingState = PARSING_VERTEX_LIST;
    std::string listName(ParserUtilities::getValueFor(attributeSet, "name"));
    graphicsManager->newVertexList(listName, 2);
  }
  else if(name == "OpenGLSettings")
  {
    parsingState = OPENGL_SETTINGS;
  }
  else if(name == "Scene")
  {
    if(!coefficientTablesGenerated)
    {
      simulation->createPhysicalCoefficientTables();
      coefficientTablesGenerated = true;
    }
    parsingState = PARSING_SCENE;
    simulation->parseAttributeElements(name, attributeSet, line, column, errorManager);
  }
  else if(name == "Simulation")
  {
    parsingState = PARSING_SIMULATION;
  }
  else if(name == "RoSiIncludeFile")
  {
    //Ignore this tag completely
  }
  else //Currently an element seems to be parsed and some attributes are following:
  {
    if(parsingState == PARSING_MAJOR_ELEMENT)
    {
      if(!currentNode->parseAttributeElements(name, attributeSet, line, column, errorManager))
      {
        assert(false); //There is an attribute which cannot be parsed
      }
    }
    else if((parsingState == PARSING_SCENE) ||(parsingState == PARSING_USE))
    {
      simulation->parseAttributeElements(name, attributeSet, line, column, errorManager);
    }
    else if(parsingState == PARSING_SURFACE)
    {
      currentSurface->parseAttributeElements(name, attributeSet, line, column, errorManager);
    }
    else if(parsingState == PARSING_MATERIAL)
    {
      currentMaterial->parseAttributeElements(name, attributeSet, line, column, errorManager);
    }
    else if(parsingState == PARSING_LIGHT)
    {
      currentLight->parseAttributeElements(name, attributeSet, line, column, errorManager);
    }
    else if(parsingState == PARSING_ENVIRONMENT)
    {
      if(name == "Elements")
        simulation->parseAttributeElements(name, attributeSet, line, column, errorManager);
      else
        currentEnvironment->parseAttributeElements(name, attributeSet, line, column, errorManager);
    }
    else if(parsingState == OPENGL_SETTINGS)
    {
      graphicsManager->parseOpenGLSettings(name, attributeSet, line, column, errorManager);
    }
  }
}


void SAX2Parser::endElement(const std::string& name,
                            int line, int column)
{
  if(parsingState == PARSING_VERTEX_LIST)
  {
    if(name == "VertexList" || name == "VertexListWithTexCoords")
    {
      parsingState = PARSING_MAJOR_ELEMENT;
    }
  }
  else if(parsingState == PARSING_MAJOR_ELEMENT)
  {
    currentNode->finishedParsingCurrentElement(name);
  }
  else if(name == "Use")
  {
    currentNode = treeStack.top();
    treeStack.pop();
    simulation->addCurrentMacroInstanceToObjectTree(currentNode);
  }
  if(isMajorElement(name) && (name != "Use"))
  {
    // if(this->simulation->getCurrentMovableID() != 0 && currentNode->castToJoint())
    if(currentNode->castToJoint())
    {
      simulation->removeCurrentMovableID();
      if(simulation->getCurrentMovableID() == 0)
      {
        simulation->setCurrentSpace(simulation->getStaticSpace());
      }
    }
    currentNode = treeStack.top();
    treeStack.pop();
  }
  else if(name == "Movable")
  {
    simulation->removeCurrentMovableID();
    if(simulation->getCurrentMovableID() == 0)
    {
      simulation->setCurrentSpace(simulation->getStaticSpace());
    }
  }
}


bool SAX2Parser::isMajorElement(const std::string& name)
{
  std::set<std::string>::const_iterator iter;
  iter = majorElementNames.find(name);
  return (iter != majorElementNames.end());
}
