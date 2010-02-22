/**
* @file InOut.h
*
* Definition of the abstract base classes In and Out for streams. 
* Include this header file for declaring streaming operators.
*
* @author Thomas Röfer
* @author Martin Lötzsch
*/

#ifndef __InOut_h_
#define __InOut_h_

#include <typeinfo>

#include <string>

/**
* The class Out is the abstract base class for all classes
* that implement writing into streams.
*/
class Out
{
protected:
/**
* Virtual redirection for operator<<(const char& value).
  */
  virtual void outChar(char) = 0;
  
  /**
  * Virtual redirection for operator<<(const unsigned char& value).
  */
  virtual void outUChar(unsigned char) = 0;
  
  /**
  * Virtual redirection for operator<<(const short& value).
  */
  virtual void outShort(short) = 0;
  
  /**
  * Virtual redirection for operator<<(const unsigned short& value).
  */
  virtual void outUShort(unsigned short) = 0;
  
  /**
  * Virtual redirection for operator<<(const int& value).
  */
  virtual void outInt(int) = 0;
  
  /**
  * Virtual redirection for operator<<(const unsigned& value).
  */
  virtual void outUInt(unsigned int) = 0;
  
  /**
  * Virtual redirection for operator<<(const long& value).
  */
  virtual void outLong(long) = 0;
  
  /**
  * Virtual redirection for operator<<(const unsigned long& value).
  */
  virtual void outULong(unsigned long) = 0;
  
  /**
  * Virtual redirection for operator<<(const float& value).
  */
  virtual void outFloat(float) = 0;
  
  /**
  * Virtual redirection for operator<<(const double& value).
  */
  virtual void outDouble(double) = 0;
  
  /**
  * Virtual redirection for operator<<(const char* value).
  */
  virtual void outString(const char *) = 0;
  
  /**
  * Virtual redirection for operator<<(Out& (*f)(Out&)) that writes
  * the symbol "endl";
  */
  virtual void outEndL() = 0;
  
  /**
  * Allow endl to call the protected function outEndL().
  */
  friend Out& endl(Out& stream);
  
public:
/**
* Operator that writes a char into a stream.
* @param value The value that is written.
* @return The stream.
  */
  Out& operator<<(const char value) {outChar(value); return *this;}
  
  /**
  * Operator that writes an unsigned char into a stream.
  * @param value The value that is written.
  * @return The stream.
  */
  Out& operator<<(const unsigned char value) {outUChar(value); ; return *this;}
  
  /**
  * Operator that writes a short int into a stream.
  * @param value The value that is written.
  * @return The stream.
  */
  Out& operator<<(const short value) {outShort(value); ; return *this;}
  
  /**
  * Operator that writes an unsigned short int into a stream.
  * @param value The value that is written.
  * @return The stream.
  */
  Out& operator<<(const unsigned short value) {outUShort(value); ; return *this;}
  
  /**
  * Operator that writes an int into a stream.
  * @param value The value that is written.
  * @return The stream.
  */
  Out& operator<<(const int value) {outInt(value); ; return *this;}
  
  /**
  * Operator that writes an unsigned int into a stream.
  * @param value The value that is written.
  * @return The stream.
  */
  Out& operator<<(const unsigned int value) {outUInt(value); ; return *this;}
  
  /**
  * Operator that writes a long int into a stream.
  * @param value The value that is written.
  * @return The stream.
  */
  Out& operator<<(const long value) {outLong(value); ; return *this;}
  
  /**
  * Operator that writes an unsigned long int into a stream.
  * @param value The value that is written.
  * @return The stream.
  */
  Out& operator<<(const unsigned long value) {outULong(value); ; return *this;}
  
  /**
  * Operator that writes a float into a stream.
  * @param value The value that is written.
  * @return The stream.
  */
  Out& operator<<(const float value) {outFloat(value); ; return *this;}
  
  /**
  * Operator that writes a double into a stream.
  * @param value The value that is written.
  * @return The stream.
  */
  Out& operator<<(const double value) {outDouble(value); ; return *this;}
  
  /**
  * Operator that writes a string into a stream.
  * @param value The value that is written.
  * @return The stream.
  */
  Out& operator<<(const char* value) {outString(value); ; return *this;};
  
  /**
  * Operator that writes a string into a stream.
  * @param value The value that is written.
  * @return The stream.
  */
  Out& operator<<(const std::string& value) {outString(value.c_str()); ; return *this;};
  
  /**
  * Operator that writes a Boolean into a stream.
  * @param value The value that is written.
  * @return The stream.
  */
  Out& operator<<(const bool value) {return *this << (int) value;}

  /**
  * Operator that calls the function pointed to by f.
  * @param f A function that is normally endl.
  * @return The stream.
  */
  Out& operator<<(Out& (*f)(Out&)) {return f(*this);}
  
  /**
  * The function writes a number of bytes into a stream.
  * @param p The address the data is located at.
  * @param size The number of bytes to be written.
  */
  virtual void write(const void* p,int size) = 0;
  
  /**
  * The function returns whether this is a binary stream.
  * @return Does it output data in binary format?
  */
  virtual bool isBinary() const {return false;}
};


/**
* This function can be inserted into a stream to represent an end of line.
* @param stream The stream the endl-symbol is inserted into.
* @return The stream.
*/
Out& endl(Out& stream);


/**
* The class In is the abstract base class for all classes
* that implement reading from streams.
*/
class In
{
protected:
/**
* Virtual redirection for operator>>(char& value).
  */
  virtual void inChar(char&) = 0;
  
  /**
  * Virtual redirection for operator>>(unsigend char& value).
  */
  virtual void inUChar(unsigned char&) = 0;
  
  /**
  * Virtual redirection for operator>>(short& value).
  */
  virtual void inShort(short&) = 0;
  
  /**
  * Virtual redirection for operator>>(unsigend short& value).
  */
  virtual void inUShort(unsigned short&) = 0;
  
  /**
  * Virtual redirection for operator>>(int& value).
  */
  virtual void inInt(int&) = 0;
  
  /**
  * Virtual redirection for operator>>(unsigend int& value).
  */
  virtual void inUInt(unsigned int&) = 0;
  
  /**
  * Virtual redirection for operator>>(long& value).
  */
  virtual void inLong(long&) = 0;
  
  /**
  * Virtual redirection for operator>>(unsigend long& value).
  */
  virtual void inULong(unsigned long&) = 0;
  
  /**
  * Virtual redirection for operator>>(float& value).
  */
  virtual void inFloat(float&) = 0;
  
  /**
  * Virtual redirection for operator>>(double& value).
  */
  virtual void inDouble(double&) = 0;
  
  /**
  * Virtual redirection for operator>>(std::string& value).
  */
  virtual void inString(std::string&) = 0;
  
  /**
  * Virtual redirection for operator>>(In& (*f)(In&)) that reads
  * the symbol "endl";
  */
  virtual void inEndL() = 0;

  /**
  * Allow endl to call the protected function inEndL().
  */
  friend In& endl(In& stream);
  
public:
/**
* Operator that reads a char from a stream.
* @param value The value that is read.
* @return The stream.
  */
  In& operator>>(char& value) {inChar(value); return *this;}
  
  /**
  * Operator that reads an unsigned char from a stream.
  * @param value The value that is read.
  * @return The stream.
  */
  In& operator>>(unsigned char& value) {inUChar(value); return *this;}
  
  /**
  * Operator that reads a short int from a stream.
  * @param value The value that is read.
  * @return The stream.
  */
  In& operator>>(short& value) {inShort(value); return *this;}
  
  /**
  * Operator that reads an unsigned short int from a stream.
  * @param value The value that is read.
  * @return The stream.
  */
  In& operator>>(unsigned short& value) {inUShort(value); return *this;}
  
  /**
  * Operator that reads an int from a stream.
  * @param value The value that is read.
  * @return The stream.
  */
  In& operator>>(int& value) {inInt(value); return *this;}
  /**
  * Operator that reads an unsigned int from a stream.
  * @param value The value that is read.
  * @return The stream.
  */
  In& operator>>(unsigned int& value) {inUInt(value); return *this;}
  
  /**
  * Operator that reads a long int from a stream.
  * @param value The value that is read.
  * @return The stream.
  */
  In& operator>>(long& value) {inLong(value); return *this;}
  /**
  * Operator that reads an unsigned long int from a stream.
  * @param value The value that is read.
  * @return The stream.
  */
  In& operator>>(unsigned long& value) {inULong(value); return *this;}
  
  /**
  * Operator that reads a float from a stream.
  * @param value The value that is read.
  * @return The stream.
  */
  
  In& operator>>(float& value) {inFloat(value); return *this;}
  
  /**
  * Operator that reads a double from a stream.
  * @param value The value that is read.
  * @return The stream.
  */
  In& operator>>(double& value) {inDouble(value); return *this;}
  
  /**
  * Operator that reads a string from a stream.
  * @param value The value that is read.
  * @return The stream.
  */
  In& operator>>(std::string& value) {inString(value); return *this;}
  
  /**
  * Operator that reads a Boolean from a stream.
  * @param value The value that is read.
  * @return The stream.
  */
  In& operator>>(bool& value) {int temp; *this >> temp; value = temp != 0; return *this;}

  /**
  * Operator that reads the endl-symbol from a stream.
  * @param f A function that is normally endl.
  * @return The stream.
  */
  In& operator>>(In& (*f)(In&)) {return f(*this);}
  
  /**
  * The function reads a number of bytes from a stream.
  * @param p The address the data is written to. Note that p
  *          must point to a memory area that is at least
  *          "size" bytes large.
  * @param size The number of bytes to be read.
  */
  virtual void read(void* p,int size) = 0;

  /**
  * The function skips a number of bytes in a stream.
  * @param size The number of bytes to be skipped.
  */
  virtual void skip(int size) = 0;

  /**
  * Determines whether the end of file has been reached.
  */ 
  virtual bool eof() const = 0;
  
  /**
  * The function returns whether this is a binary stream.
  * @return Does it output data in binary format?
  */
  virtual bool isBinary() const {return false;}
};


/**
* This function can be read from a stream to represent an end of line.
* @param stream The stream the endl-symbol is read from.
* @return The stream.
*/
In& endl(In& stream);



#endif //__InOut_h_
