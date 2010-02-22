/**
 * @file SAX2Parser.h
 * 
 * Definition of class SAX2Parser
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
 */ 

#ifndef SAX2PARSER_H_
#define SAX2PARSER_H_

#include <string>
#include <vector>
#include <set>
#include <stack>

#include "ParserUtilities.h"

class Actuatorport;
class Simulation;
class SimObject;
class Actuator;
class Simulation;
class Surface;
class SimMacro;
class ErrorManager;
class GraphicsManager;
class Material;
class Light;
class Environment;


/** 
* @class SAX2Parser
*
* A SAX2 parser for parsing the XML style *.ros files
*/
class SAX2Parser
{
public:
  /** Constructor*/
  SAX2Parser();

  /** The parse function. Parses a file and builds all internal
  * data structures.
  * @param filename The name of the file to parse
  * @param schemaname The name the XML:Schema to use for validation
  * @param sim A pointer to the simulation object
  * @param objectTree A pointer to the tree (call with empty pointer)
  * @param errorManager A pointer to the error handler
  * @param graphicsManager A pointer to the graphics manager
  * @param sensorportList A list of all sensorports
  * @param actuatorportList A list of all actuatorports
  * @param actuatorList A list of all actuators
  * @param surfaces A list of all object surfaces
  * @param materials A list of all object materials
  * @param environments A list of all environments
  */
  void parse(const std::string& filename, 
             const std::string& schemaname,
             Simulation* sim,
             SimObject*& objectTree,
             ErrorManager* errorManager,
             GraphicsManager* graphicsManager,
             std::vector<Surface*>* surfaces,
             std::vector<Material*>* materials,
             std::vector<Environment*>* environments);

  /** A function automatically called at the beginning of the parsing process*/
  void startDocument();

  /** A function automatically called at the end of the parsing process*/
  void endDocument();

  /** A function called automatically at the beginning of a new element
  * @param name The name of the Element
  * @param attributeSet The attributes of the element
  * @param line The number of the line
  * @param column The column
  */
  void startElement(const std::string& name,
                    const AttributeSet& attributeSet,
                    int line, int column);

  /** A function called automatically at the closing tag of an element
  * @param name The name of the Element
  * @param line The number of the line
  * @param column The column
  */
  void endElement(const std::string& name,
                  int line, int column);

  /** Return a pointer to the currentNode
  * @return The current processed node
  */
  SimObject* getCurrentNode()
  { return currentNode; };

private:
  /** Enumeration describung the state of the parser*/
  enum ParsingState {PARSING_SIMULATION, PARSING_MAJOR_ELEMENT, PARSING_SCENE, 
                     PARSING_SURFACE, PARSING_VERTEX_LIST, PARSING_USE,
                     PARSING_MATERIAL, PARSING_LIGHT, OPENGL_SETTINGS,
                     PARSING_ENVIRONMENT} parsingState;

  /** Tool function to determine if an XML-element is also a simulated element
  * @param name The name of the element
  * @return true, if it is a major element
  */
  bool isMajorElement(const std::string& name);

  /** A pointer to the surface list*/
  std::vector<Surface*>* surfaces;
  /** A pointer to the material list*/
  std::vector<Material*>* materials;
  /** A pointer to the environment list*/
  std::vector<Environment*>* environments;
  /** A list of macros*/
  std::vector<SimMacro*> macros;
  /** Buffer for the names of major elements*/
  std::set<std::string> majorElementNames;
  /** A stack for the parsing process*/
  std::stack<SimObject*> treeStack;
  /** The node, which is currently parsed*/
  SimObject* currentNode;
  /** The root node of the object tree*/
  SimObject* rootNode;
  /** A pointer to the simulation kernel*/
  Simulation* simulation;
  /** The surface, which is currently parsed*/
  Surface* currentSurface;
  /** The material, which is currently parsed*/
  Material* currentMaterial;
  /** The environment, which is currently parsed*/
  Environment* currentEnvironment;
  /** The light, which is currently parsed*/
  Light* currentLight;
  /** A pointer to the error manager*/
  ErrorManager* errorManager;
  /** A pointer to the graphics manager*/
  GraphicsManager* graphicsManager;
  /** Flag: true, if the coefficient tables have already been created*/
  bool coefficientTablesGenerated;
};


#endif //SAX2PARSER_H_
