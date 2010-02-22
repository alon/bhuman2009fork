/**
 * @file Platform/Win32/File.h
 * 
 * Declaration of class File for Win32.
 */

#ifndef __File_h__
#define __File_h__

#include <stdio.h>
#include <string>

/**
 * This class provides basic file input/output capabilies. 
 */
class File
{
public:
  /**
   * Constructor.
   * @param name File name or path. If it is a relative path, it is assumed
   *             to be relative to the path for configuration files. Otherwise,
   *             the path is used directly.
   * @param mode File open mode as used by fopen defined in stdio.h.
   */
  File(const std::string& name, const std::string& mode);

  /**
   * Destructor.
   */
  ~File();

  /**
   * The function read a number of bytes from the file to a certain
   * memory location.
   * @param p The start of the memory space the data is written to.
   * @param size The number of bytes read from the file.
   */
  void read(void* p, unsigned size);

  /**
   * The function writes a number of bytes from a certain memory 
   * location into the file.
   * @param p The start of the memory space the data is read from.
   * @param size The number of bytes written into the file.
   */
  void write(const void *p, unsigned size);

  /**
   * The function implements printf for the stream represented by
   * instances of this class.
   * @param format Format string as used by printf defined in stdio.h.
   * @param ... See printf in stdio.h.
   */
  void printf(const char* format, ...);
  
  /**
   * The function returns whether the file represented by an
   * object of this class actually exists.
   * @return The existence of the file.
   */
  bool exists() const {return stream != 0;}

  /**
   * The function returns whether the end of the file represented 
   * by an object of this class was reached.
   * @return End of file reached?
   */
  bool eof();

  /**
  * The function returns the current GT directory,
  * e.g. /MS/OPENR/APP or <...>/GT2003 or /usr/local/GT2003
  * @return The current GTDir
  */
  static const char* getGTDir();

private:
  FILE* stream; /**< File handle. */
};

#endif // __File_h__
