/**
* @file Tools/Streams/InOut.cpp
*
* Implementation of the streamable function endl.
*
* @author <a href="mailto:oberlies@sim.tu-darmstadt.de">Tobias Oberlies</a>
*/

#include "InOut.h"

Out& endl(Out& stream)
{
  stream.outEndL(); 
  return stream;
}

In& endl(In& stream)
{
  stream.inEndL();
  return stream;
}
