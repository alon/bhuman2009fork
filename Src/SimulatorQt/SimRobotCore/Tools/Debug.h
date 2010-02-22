#ifndef SIMROBOT_DEBUG_H_
#define SIMROBOT_DEBUG_H_

#include <string>

class Matrix3d;
class Vector3d;
class ErrorDescription;

class DOut
{
private:
  char conversionBuffer[200];
 
public:
  DOut& operator<<(double d);
  DOut& operator<<(int i);
  DOut& operator<<(unsigned int u);
  DOut& operator<<(bool b);
  DOut& operator<<(const char* c);
  DOut& operator<<(const std::string& s);
  DOut& operator<<(const Vector3d& v);
  DOut& operator<<(const Matrix3d& m);
  DOut& operator<<(const ErrorDescription& e);

  void print(const char* s);

  static DOut& getDebugObject();

};


#define DOUT(expression) DOut::getDebugObject() << expression;

#endif //SIMROBOT_DEBUG_H_
