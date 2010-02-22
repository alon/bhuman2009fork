/**
 * @file Errors.cpp
 * 
 * Implementation of class ErrorManager
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#include "Errors.h"


void ErrorManager::getAllErrors(std::vector<ErrorDescription>& errors)
{
  for(unsigned int i = 0; i < this->errors.size(); i++)
  {
    errors.push_back(this->errors[i]);
  }
}

void ErrorManager::addError(const std::string& title, const std::string& text,
                            int line, int column)
{
  ErrorDescription error;
  error.title = title;
  error.text = text;
  error.position = true;
  error.line = line;
  error.column = column;
  addError(error);
}

void ErrorManager::addError(const std::string& title, const std::string& text)
{
  ErrorDescription error;
  error.title = title;
  error.text = text;
  error.position = false;
  error.line = 0;
  error.column = 0;
  addError(error);
}

void ErrorManager::addUnknownElementError(const std::string& elementName,
                                          int line, int column)
{
  ErrorDescription error;
  error.title = "Unknown element!";
  error.text = "The element <"+elementName+"> does not exist.";
  error.position = true;
  error.line = line;
  error.column = column;
  addError(error);
}
