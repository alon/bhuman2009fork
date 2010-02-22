/**
 * @file Errors.h
 * 
 * Definition of class ErrorManager
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef ERRORS_H_
#define ERRORS_H_

#include <deque>
#include <vector>

#include "../Simulation/APIDatatypes.h"


/**
* @class ErrorManager
*
* A class for storing errors which occur at runtime
*/
class ErrorManager
{
private:
  /** The error queue*/
  std::deque<ErrorDescription> errors;

public:
  /** Destructor*/
  ~ErrorManager() {errors.clear();}

  /** Returns the number of stored errors
  * @return The number of errors
  */
  unsigned int getNumberOfErrors() const {return errors.size();}

  /** Returns the first (oldest) error
  * @param error The error to be returned
  */
  void getFirstError(ErrorDescription& error) const {error = errors.front();}

  /** Returns a list of all errors
  * @param errors The errors to be returned
  */
  void getAllErrors(std::vector<ErrorDescription>& errors);

  /** Deletes the first (oldest) error*/
  void deleteFirstError() {errors.pop_front();}

  /** Deletes all errors*/
  void deleteAllErrors() {errors.clear();}

  /** Adds a new error at the end of the queue
  * @param error The error
  */
  void addError(const ErrorDescription& error) {errors.push_back(error);}

  /** Adds a new error at the end of the queue
  * (line and column will be set to 0, error.position to false)
  * @param title The title of the error
  * @param text Detailed description
  */
  void addError(const std::string& title, const std::string& text);

  /** Adds a new error at the end of the queue
  * (error.position will be set to true)
  * @param title The title of the error
  * @param text Detailed description
  * @param line The line in the XML file in which the error occured
  * @param column The column in the XML file in which the error occured
  */
  void addError(const std::string& title, const std::string& text,
                int line, int column);

  /** Adds a new error abount an unknown element at the end of the queue
  * (error.position will be set to true)
  * @param elementName The name of the unknown element
  * @param line The line in the XML file in which the error occured
  * @param column The column in the XML file in which the error occured
  */
  void addUnknownElementError(const std::string& elementName,
                              int line, int column);
};

#endif //ERRORS_H_
