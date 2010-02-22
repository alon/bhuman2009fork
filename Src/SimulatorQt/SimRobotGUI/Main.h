
#ifndef Main_H
#define Main_H

#include <QString>

class Main
{
public:

  Main();
  ~Main();

  static Main* main;

  QString appPath;
  QString appString;

  static unsigned int getSystemTime();

private:  
  unsigned int getAppLocationSum();

  friend int main(int argc, char *argv[]);
};


#endif // Main_H
