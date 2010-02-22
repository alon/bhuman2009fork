/**
 * @file LibXML2ParserImpl.h
 * 
 * Definition of class LibXML2ParserImpl.
 * An implementation of a SAX2 parser using libXML2
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef LIBXML2PARSERIMPL_H_
#define LIBXML2PARSERIMPL_H_

#include <libxml/xmlerror.h>
#include <libxml/xmlreader.h>

#include "SAX2Parser.h"

#include "../Tools/Errors.h"

class Simulation;

/** 
* @class LibXML2ParserImpl
*
* A SAX2 parser for parsing the XML style *.ros files
*/
class LibXML2ParserImpl
{
public:
  /** The parse function. Parses a file and builds all internal
  * data structures.
  * @param filename The name of the file to parse
  * @param schemaname The name the XML:Schema to use for validation
  * @param sax2Parser A pointer to the SimRobot parser
  * @param errorManager A pointer to the error handler
  */
  void parse(const std::string& filename, 
             const std::string& schemaname,
             SAX2Parser* sax2Parser,
             ErrorManager* errorManager);

private:
  void processNode(xmlTextReaderPtr reader);

  void validate(const std::string& filename, 
                const std::string& schemaname);

  static void structuredErrorFunc(void * userData, xmlErrorPtr error)
  {
    std::string message(error->message);
    int pos(message.find("\n"));
    message.replace(pos,pos," ");
    //int test(numberOfXIncludeSubstitutions);
    if(numberOfXIncludeSubstitutions)
    {
      // If XInclude file not found, there is no file information, only the message from libxml
      if(error->file)
      {
        message = "In "+std::string(error->file)+": "+message;
      }
    }
    if((error->int2) || (error->line > 1)) //Check, if position information is available
    {
      errorManager->addError("File not valid!",std::string(message), 
                             error->line, error->int2);
    }
    else
    {
      errorManager->addError("File not valid!",std::string(message)); 
    }
  }

  SAX2Parser* parser;
  AttributeSet attributeSet;
  static ErrorManager* errorManager;
  static int numberOfXIncludeSubstitutions;
};


#endif //LIBXML2PARSERIMPL_H_


