
#include <cstdio>

#include "Debug.h"
#include "Platform/Assert.h"
#include "SimMath.h"
#include "Simulation/APIDatatypes.h"

void DOut::print(const char* s)
{
  TRACE("%s", s);
}

DOut& DOut::operator<<(double d)
{
  sprintf(&conversionBuffer[0],"%f",d);
  print(&conversionBuffer[0]);
  return (*this);
}

DOut& DOut::operator<<(int i)
{
  sprintf(&conversionBuffer[0],"%d",i);
  print(&conversionBuffer[0]);
  return (*this);
}

DOut& DOut::operator<<(unsigned int u)
{
  sprintf(&conversionBuffer[0],"%u",u);
  print(&conversionBuffer[0]);
  return (*this);
}

DOut& DOut::operator<<(bool b)
{
  if(b)
    print("true");
  else
    print("false");
  return (*this);
}

DOut& DOut::operator<<(const char* c)
{
  print(c);
  return (*this);
}

DOut& DOut::operator<<(const Vector3d& v)
{
  *(this)<<"( "<<v.v[0]<<" | "<<v.v[1]<<" | "<<v.v[2]<<" )"<<"\n";
  return (*this);
}

DOut& DOut::operator<<(const Matrix3d& m)
{
  *(this)<<m.col[0].v[0]<<"  "<<m.col[1].v[0]<<"  "<<m.col[2].v[0]<<"\n";
  *(this)<<m.col[0].v[1]<<"  "<<m.col[1].v[1]<<"  "<<m.col[2].v[1]<<"\n";
  *(this)<<m.col[0].v[2]<<"  "<<m.col[1].v[2]<<"  "<<m.col[2].v[2]<<"\n";
  return (*this);
}

DOut& DOut::operator<<(const ErrorDescription& e)
{
  return (*this);
}

DOut& DOut::getDebugObject()
{
  static DOut instance;
  return instance;
}
