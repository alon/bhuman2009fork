/**
 * @file LibXML2ParserImpl.cpp
 * 
 * Implementation of class LibXML2ParserImpl.
 * An implementation of a SAX2 parser using libXML2
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */

#include "LibXML2ParserImpl.h"
#include <Tools/Errors.h>
#include <libxml/xmlschemas.h>
#include <libxml/xinclude.h>

ErrorManager* LibXML2ParserImpl::errorManager = 0;
int LibXML2ParserImpl::numberOfXIncludeSubstitutions = 0;


void LibXML2ParserImpl::validate(const std::string& filename, 
                                 const std::string& schemaname)
{
  xmlSchemaPtr schema = 0;
  xmlSchemaParserCtxtPtr ctxt;
  ctxt = xmlSchemaNewParserCtxt(schemaname.c_str());
  schema = xmlSchemaParse(ctxt);
  xmlSchemaFreeParserCtxt(ctxt);
  if(schema)
  {
    xmlDocPtr doc;
    xmlSchemaValidCtxtPtr ctxt;
    ctxt = xmlSchemaNewValidCtxt(schema);
    xmlSetStructuredErrorFunc(ctxt, (xmlStructuredErrorFunc)(this->structuredErrorFunc));
    doc = xmlReadFile(filename.c_str(),"ISO-8859-1",0);
    //Set to a value to cover the case of errors during include process. This forces
    //the error handler to print the name of the file in which the error occured
    numberOfXIncludeSubstitutions = 1; 
    numberOfXIncludeSubstitutions = xmlXIncludeProcess(doc);
    if(xmlSchemaValidateDoc(ctxt, doc)) 
    {
      errorManager->addError("File not valid!",
        filename+" is not compatible with RoSiML or has not been found :-(");
    }
    xmlSchemaFreeValidCtxt(ctxt);
    xmlFreeDoc(doc);
    xmlSchemaFree(schema);
  }
  else
  {
    errorManager->addError("File not valid!",
                            "Error while loading "+schemaname);
  }
  xmlCleanupParser();
  xmlMemoryDump();
}


void LibXML2ParserImpl::processNode(xmlTextReaderPtr reader)
{
  int nodeType(xmlTextReaderNodeType(reader));
  int line(xmlTextReaderGetParserLineNumber(reader));
  int column(xmlTextReaderGetParserColumnNumber(reader));
  xmlChar* xmlString = xmlTextReaderName(reader);
  std::string nodeName((const char*)xmlString);
  xmlFree(xmlString);
  if(nodeType == 1) //Start element
  {
    int numberOfAttributes(xmlTextReaderAttributeCount(reader));
    bool isEmptyElement(xmlTextReaderIsEmptyElement(reader) != 0);
    attributeSet.clear();
    for(int i=0; i<numberOfAttributes; i++)
    {
      xmlTextReaderMoveToAttributeNo(reader,i);
      xmlString = xmlTextReaderName(reader);
      std::string attName((const char*)xmlString);
      xmlFree(xmlString);
      xmlString = xmlTextReaderValue(reader);
      std::string attValue((const char*)xmlString);
      xmlFree(xmlString);
      attributeSet.push_back(AttributeValuePair(attName, attValue));
    }
    parser->startElement(nodeName, attributeSet, line, column);
    if(isEmptyElement)  //Element has no content
    {
      parser->endElement(nodeName,line,column);
    }
  }
  else if(nodeType == 15) //Closing element
  {
    parser->endElement(nodeName,line,column);
  }
}


void LibXML2ParserImpl::parse(const std::string& filename, 
                              const std::string& schemaname,
                              SAX2Parser* sax2Parser,
                              ErrorManager* errorManager)
{
  this->parser = sax2Parser;
  this->errorManager = errorManager;
  numberOfXIncludeSubstitutions = 0;
  //Validate file and stop parsing in case of errors:
  //Validation and parsing are separated since xmlTextReader
  //does not seem to support XML:Schema validation while parsing.
  validate(filename, schemaname);
  if(errorManager->getNumberOfErrors())
  {
    return;
  }
  //Parse file:
  xmlTextReaderPtr reader = xmlReaderForFile(filename.c_str(),"ISO-8859-1",XML_PARSE_XINCLUDE | XML_PARSE_DTDATTR);
  xmlTextReaderSetParserProp(reader, XML_PARSER_DEFAULTATTRS, 1);
  int ret;
  if (reader) 
  {
    parser->startDocument();
    ret = xmlTextReaderRead(reader);
    while (ret == 1) 
    {
      processNode(reader);
      ret = xmlTextReaderRead(reader);
    }
    xmlTextReaderClose(reader);
    xmlFreeTextReader(reader);
    if (ret) 
    {
      errorManager->addError("File not valid!",
                             "Error while parsing "+filename);
    }
    parser->endDocument();
  }
}
