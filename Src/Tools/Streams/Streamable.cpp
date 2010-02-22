#include "Streamable.h"

void Streamable::streamOut(Out& out) const
{
  const_cast<Streamable*>(this)->serialize(NULL, &out);
}
void Streamable::streamIn(In& in)
{
  serialize(&in,NULL);
}
In& operator>>(In& in, Streamable& streamable)
{
    streamable.streamIn( in);
    return in;
}
Out& operator<<(Out& out, const Streamable& streamable)
{
  streamable.streamOut(out);
  return out;
}
