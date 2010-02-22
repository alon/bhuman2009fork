
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QApplication>

#ifdef _WIN32
#include <windows.h>
//#include <mmsystem.h>
#include <shlobj.h>
#else
#include <sys/time.h>
#endif

#include "Main.h"
#include "MainWindow.h"

Main* Main::main = 0;

Main::Main()
{
  Q_ASSERT(main == 0);
  main = this;
}

Main::~Main()
{
  Q_ASSERT(main == this);
  main = 0;
}

unsigned int Main::getAppLocationSum()
{
  unsigned int sum = 0;
  const QString& path(QFileInfo(QFileInfo(appPath).dir().path()).dir().path());
  const QChar* data = path.data();
  const QChar* dataEnd = data + path.count();
  for(; data < dataEnd; ++data)
  {
    sum ^= sum >> 16;
    sum <<= 1;
    sum += data->toLower().unicode();
  }
  return sum;
}

unsigned int Main::getSystemTime()
{
#ifdef _WIN32
  return GetTickCount();
#else
  struct timeval tv;
  gettimeofday(&tv, 0);
  return (unsigned int) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
}

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(SimRobotGUI);

  Main main;
  {
#ifdef _WIN32
    char fileName[_MAX_PATH];
    char longFileName[_MAX_PATH];
    GetModuleFileNameA(GetModuleHandleA(0), fileName, _MAX_PATH);
    GetLongPathNameA(fileName, longFileName, _MAX_PATH);
    Main::main->appPath = longFileName;
    QSettings settings("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
    settings.setValue(".ros/.", "RoSi");
    settings.setValue(".ros/ShellNew", "NullFile");
    settings.setValue("RoSi/.", "Robot Simulation");
    settings.setValue("RoSi/shell/open/command/.", "\"" + Main::main->appPath + "\" \"%1\"");
    const QString& iconPath("\"" + Main::main->appPath + "\",1");
    if(settings.value("RoSi/DefaultIcon/.").toString() != iconPath)
    {
      settings.setValue("RoSi/DefaultIcon/.", iconPath);
      SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);
    }
#else
    if(*argv[0] == '/')
      Main::main->appPath = QDir::cleanPath(QObject::tr(argv[0]));
    else
      Main::main->appPath = QDir::cleanPath(QDir::root().current().path() + "/" + argv[0]);
#endif
  }

  Main::main->appString = QString("SimRobot\\%1").arg(Main::main->getAppLocationSum());


  QApplication app(argc, argv);
  MainWindow mainWindow(argc, argv);
  mainWindow.show();
  return app.exec();
}
