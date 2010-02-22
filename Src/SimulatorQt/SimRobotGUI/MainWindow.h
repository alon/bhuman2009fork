

#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>
#include <QSettings>
#include <QSignalMapper>

#include "Document.h"

class DockWidget;
class MenuBar;
class StatusBar;
class ActuatorWidget;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(int argc, char *argv[]);

  QAction* fileNewAct;
  QAction* fileOpenAct;
  QAction* fileSaveAct;
  QAction* fileSaveAsAct;
  QAction* fileCloseAct;
  QAction* fileExitAct;
  QAction* simResetAct;
  QAction* simStartAct;
  QAction* simStepAct;
  QAction* fitWindowSizeAct;

private slots:
  void open();
  void openFile(const QString& file);
  void about();
  void save();
  void saveAs();
  void close();
  void newFile();
  void openObject(const SceneObject& object);
  void raiseObject(const SceneObject& object);
  void closeObject(const SceneObject& object);
  void writeLayout();
  void restoreLayout();
  void openedDocument();
  void aboutToCloseDocument();
  void closedDocument();
  void startedSim();
  void pausedSim();
  void updateFileMenu();
  void updateViewMenu();
  void focusChanged(QWidget *old, QWidget* now);
  void fitWindowSize();
  void modificationChanged(bool changed);
  void simReset();
  void simStart();
  void simStep();

private:
  class DockWidgetItem
  {
  public:
    DockWidget* widget;

    DockWidgetItem(DockWidget* widget) : widget(widget) {}

    bool operator<(const DockWidgetItem& other) const;
  };

  QSettings settings;
  QStringList recentFiles;
  QSignalMapper recentFileMapper;
  QList<DockWidgetItem> dockWidgets;
  MenuBar* menuBar;
  QToolBar* toolBar;
  StatusBar* statusBar;
  QRect normalGeometry; /**< The restored or last recoginzed, not maximized or minimized window geometry. */
  QMenu* fileMenu;
  QMenu* simMenu;
  QMenu* viewMenu;
  QMenu* helpMenu;
  QMenu* dockWidgetMenu;
  ActuatorWidget* actuatorsWidget;
  DockWidget* consoleWidget;
  DockWidget* sceneGraphWidget;
  DockWidget* activeDockWidget;

  /**
  * Saves the geometry (position, size and state) of the window. (Since QWidget::saveGeometry is buggy.)
  * Use this in writeLayout().
  * @param settings The layout settings used to save the geometry.
  */
  void writeGeometry(QSettings& settings);

  /**
  * Restores the geometry (position, size and state) of the window.
  * Use this in restoreLayout().
  * @param settings The layout settings in which the geomerty was saved.
  */
  void restoreGeometry(QSettings& settings);
  
  void moveEvent(QMoveEvent* event);
  void resizeEvent(QResizeEvent* event);
  void closeEvent(QCloseEvent* event);
  void keyPressEvent(QKeyEvent *event);
  void keyReleaseEvent(QKeyEvent *event);
  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);
  QMenu* createPopupMenu();
  void updateViewMenu(QMenu* menu);
  void createActions();
  void updateMenuAndToolBarAndTitle();
  void updateWindowTitle();
};

#endif