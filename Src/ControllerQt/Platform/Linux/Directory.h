/**
* \file Platform/Linux/Directory.h
* Declares a platform dependend class for accessing directories.
* Yeah, this is the POSIX implementation.
* \author Colin Graf
*/

#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__

#include <string>

/**
* A Class for accessing directories.
*/
class Directory
{
public:

  /**
  * Default constructor.
  */
  Directory();

  /**
  * Destructor.
  */
  ~Directory();

  /**
  * Opens a directory for searching files.
  * \param pattern A search pattern like "/etc/a*.ini"
  * \return Whether the directory was opened successfully.
  */
  bool open(const std::string& pattern);

  /**
  * Searches the next matching entry in the opened directory.
  * \param name The name of the next matching entry.
  * \param isDir Whether the next entry is a directory.
  * \return true when a matching entry was found.
  */
  bool read(std::string& name, bool& isDir);

private:

  void* dp; /**< Directory descriptor. */
  std::string dirname; /**< The name of the directory. */
  std::string filepattern; /**< The pattern for file name matching (e.g. "*.dll"). */
};

#endif //!__DIRECTORY_H__

