

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

class System
{
public:

  /** Returns the current system time in msecs.
  * \return the time
  */
  static unsigned int getTime();

  /** Returns the amount of msecs passed since <code>when</code>.
  * \param when 
  * \return The amount
  */
  static inline unsigned int getTimeSince(unsigned int when)
  {
    return getTime() - when;
  }

};


#endif
