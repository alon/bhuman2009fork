
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QUrl>
#include <QToolBar>
#include <QToolButton>

#ifdef _WIN32
#include <windows.h>
#endif

#include "MainWindow.h"
#include "MenuBar.h"
#include "StatusBar.h"
#include "Main.h"
#include "SceneGraphWidget.h"
#include "ObjectWidget.h"
#include "ConsoleWidget.h"
#include "SensorWidget.h"
#include "ActuatorWidget.h"
#include "EditorWidget.h"

MainWindow::MainWindow(int argc, char *argv[]) : settings("B-Human", Main::main->appString),
  recentFiles(settings.value("RecentFiles").toStringList()), dockWidgetMenu(0), actuatorsWidget(0), consoleWidget(0), sceneGraphWidget(0), activeDockWidget(0)

{
  setWindowTitle(tr("SimRobot"));
  setWindowIcon(QIcon(":/icons/Simulator.png"));
  setAcceptDrops(true);
  setDockNestingEnabled(true);
  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
  resize(600, 400);

  Document* document = new Document(this);
  connect(document, SIGNAL(openObjectSignal(const SceneObject&)), this, SLOT(openObject(const SceneObject&)));
  connect(document, SIGNAL(raiseObjectSignal(const SceneObject&)), this, SLOT(raiseObject(const SceneObject&)));
  connect(document, SIGNAL(closeObjectSignal(const SceneObject&)), this, SLOT(closeObject(const SceneObject&)));
  connect(document, SIGNAL(restoreLayoutSignal()), this, SLOT(restoreLayout()));
  connect(document, SIGNAL(writeLayoutSignal()), this, SLOT(writeLayout()));
  connect(document, SIGNAL(aboutToCloseSignal()), this, SLOT(aboutToCloseDocument()));
  connect(document, SIGNAL(openedSignal()), this, SLOT(openedDocument()));
  connect(document, SIGNAL(closeSignal()), this, SLOT(closedDocument()));
  connect(document, SIGNAL(startedSim()), this, SLOT(startedSim()));
  connect(document, SIGNAL(pausedSim()), this, SLOT(pausedSim()));

  createActions();

  statusBar = new StatusBar(this);
  setStatusBar(statusBar);

  menuBar = new MenuBar(this);
  setMenuBar(menuBar);

  toolBar = addToolBar(tr("&Toolbar"));
  toolBar->setObjectName("Toolbar");
  toolBar->setIconSize(QSize(16, 16));

  fileMenu = new QMenu(tr("&File"), this);
  connect(&recentFileMapper, SIGNAL(mapped(const QString&)), this, SLOT(openFile(const QString&)));
  connect(fileMenu, SIGNAL(aboutToShow()), this, SLOT(updateFileMenu()));
  updateFileMenu();

  simMenu = new QMenu(tr("&Simulation"), this);
  simMenu->addAction(simStartAct);
  simMenu->addAction(simResetAct);
  simMenu->addAction(simStepAct);

  viewMenu = new QMenu(tr("&View"), this);
  connect(viewMenu, SIGNAL(aboutToShow()), this, SLOT(updateViewMenu()));
  updateViewMenu();

  helpMenu = new QMenu(tr("&Help"), this);
  QAction* action;
  connect(action = helpMenu->addAction(tr("&About...")), SIGNAL(triggered()), this, SLOT(about()));
  action->setStatusTip(tr("Show the application's About box"));
  connect(action = helpMenu->addAction(tr("About &Qt...")), SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  action->setStatusTip(tr("Show the Qt library's About box"));

  connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));

  closedDocument();
  for(int i = 1; i < argc; i++)
    if(*argv[i] != '-')
    {
      openFile(argv[i]);
      break;
    }
  if(!Document::document->isLayoutRestored())
    updateMenuAndToolBarAndTitle();
}

void MainWindow::updateMenuAndToolBarAndTitle()
{
  menuBar->clear();
  toolBar->clear();

  if(dockWidgetMenu)
  {
    delete dockWidgetMenu;
    dockWidgetMenu = 0;
  }

  menuBar->addMenu(fileMenu);
  menuBar->addMenu(viewMenu);
  menuBar->addMenu(simMenu);
  
  toolBar->addAction(fileOpenAct);
  toolBar->addAction(fileSaveAct);
  toolBar->addSeparator();
  toolBar->addAction(simStartAct);
  toolBar->addAction(simResetAct);
  toolBar->addAction(simStepAct);
  if(consoleWidget || sceneGraphWidget)
  {
    toolBar->addSeparator();
    if(consoleWidget)
      toolBar->addAction(consoleWidget->toggleViewAction());
    if(sceneGraphWidget)
      toolBar->addAction(sceneGraphWidget->toggleViewAction());
  }
  if(activeDockWidget)
  {
    toolBar->addSeparator();
    toolBar->addAction(fitWindowSizeAct);

    dockWidgetMenu = activeDockWidget->createPopupMenu();
    if(dockWidgetMenu)
    {      
      menuBar->addMenu(dockWidgetMenu);
      toolBar->addSeparator();

      const QList<QAction*>& actions(dockWidgetMenu->actions());
      bool addSeparator = false;
      foreach(QAction* action, actions)
      {
        if(!action->icon().isNull())
        {
          if(addSeparator)
            toolBar->addSeparator();
          toolBar->addAction(action);
          if(action->menu())
            qobject_cast<QToolButton*>(toolBar->widgetForAction(action))->setPopupMode(QToolButton::InstantPopup);
        }
        addSeparator = action->isSeparator();
      }
    }
  }

  
  menuBar->addMenu(helpMenu);

  updateWindowTitle();
}

void MainWindow::updateWindowTitle()
{
  if(activeDockWidget)
    setWindowTitle(tr("SimRobot - ") + activeDockWidget->windowTitle());
  else
    setWindowTitle(tr("SimRobot"));
}

void MainWindow::aboutToCloseDocument()
{
  activeDockWidget = 0;
  consoleWidget = 0;
  sceneGraphWidget = 0;
  updateMenuAndToolBarAndTitle();
}

void MainWindow::closedDocument()
{
  fileCloseAct->setEnabled(false);
  fileSaveAsAct->setEnabled(false);
  fileSaveAct->setEnabled(false);
  simStartAct->setEnabled(false);
  simResetAct->setEnabled(false);
  simStepAct->setEnabled(false);
}

void MainWindow::openedDocument()
{
  fileCloseAct->setEnabled(true);
  fileSaveAsAct->setEnabled(true);
  fileSaveAct->setEnabled(false); // yes, false!
  simStartAct->setEnabled(true);
  simResetAct->setEnabled(true);
  simStepAct->setEnabled(true);
}

void MainWindow::startedSim()
{
  simStartAct->setChecked(true);
}

void MainWindow::pausedSim()
{
  simStartAct->setChecked(false);
}

void MainWindow::newFile()
{
  Document::document->newFile();
}

void MainWindow::openFile(const QString& fileName)
{
  QString error;
  if(!Document::document->openFile(fileName, error))
  {
     QMessageBox::warning(this, tr("SimRobot"), error);
     return;
  }

  // add clean filepath to recent file list
  const QString& file(Document::document->getFile());
  recentFiles.removeAll(file);
  recentFiles.prepend(file);
  while(recentFiles.count() > 8)
    recentFiles.removeLast();
  settings.setValue("RecentFiles", recentFiles);
}

void MainWindow::open()
{
  QString fileName = QFileDialog::getOpenFileName(this, 
    tr("Open File"), settings.value("Opendir", "").toString(), tr("Robot Simulation Files (*.ros)"));

  if(fileName.isEmpty())
    return;
  settings.setValue("Opendir", QFileInfo(fileName).dir().path());

  openFile(fileName);
}

void MainWindow::save()
{
  if(Document::document->getFile().isEmpty())
  {
    saveAs();
    return;
  }

  QString error;
  if(!Document::document->saveFile(Document::document->getFile(), error))
  {
     QMessageBox::warning(this, tr("SimRobot"), error);
     return;
  }
}

void MainWindow::close()
{
  if(fileSaveAct->isEnabled()) // file content changed
  {
    switch(QMessageBox::question(this, tr("SimRobot"), tr("Save changes?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
    {
    case QMessageBox::Yes:
      save();
      break;
    case QMessageBox::Cancel:
      return;
    default:
      break;
    }
  }
  Document::document->close();
}

void MainWindow::saveAs()
{
  QString oldFile(Document::document->getFile());
  if(oldFile.isEmpty())
    oldFile = settings.value("Opendir", "").toString();

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As"), oldFile, tr("Robot Simulation Files (*.ros)"));
  if (fileName.isEmpty())
     return;

  QString error;
  if(!Document::document->saveFile(fileName, error))
  {
     QMessageBox::warning(this, tr("SimRobot"), error);
     return;
  }

  // add clean filepath to recent file list
  const QString& file(Document::document->getFile());
  recentFiles.removeAll(file);
  recentFiles.prepend(file);
  while(recentFiles.count() > 8)
    recentFiles.removeLast();
  settings.setValue("RecentFiles", recentFiles);
}

void MainWindow::openObject(const SceneObject& object)
{
  DockWidget* widget = 0;
  
  switch(object.type)
  {
  case SceneObject::Actuator: 
    Document::document->openObject(SceneObject(".Actuators", SceneObject::Actuators));
    if(!actuatorsWidget)
      openObject(SceneObject(".Actuators", SceneObject::Actuators));
    actuatorsWidget->addObject(object);
    return;
    break;
  case SceneObject::SceneGraph: 
    if(!sceneGraphWidget)
      widget = sceneGraphWidget = new SceneGraphWidget(object, this);
    break;
  case SceneObject::Console:
    if(!consoleWidget)
      widget = consoleWidget = new ConsoleWidget(object, this);
    break;
  case SceneObject::Object: 
    widget = new ObjectWidget(object, this);
    break;
  case SceneObject::Sensor:
    widget = new SRSensorWidget(object, this);
    break;
  case SceneObject::Actuators:
    if(!actuatorsWidget)
      widget = actuatorsWidget = new ActuatorWidget(object, this);
    break;
  case SceneObject::Editor:
    widget = new EditorWidget(object, this);
    connect(widget, SIGNAL(modificationChanged(bool)), this, SLOT(modificationChanged(bool)));
    break;
  default:
    break;
  }

  if(widget)
  {
    addDockWidget(Qt::BottomDockWidgetArea, widget);
    widget->setFloating(true);
    dockWidgets.append(widget);
    if(Document::document->isLayoutRestored())
      widget->activateWindow();
  }
}

void MainWindow::raiseObject(const SceneObject& object)
{
  if(object.type == SceneObject::Actuator) // sepcial handling for the gathered actuator window
  {
    Q_ASSERT(actuatorsWidget);
    Document::document->openObject(SceneObject(".Actuators", SceneObject::Actuators)); // raise the actuator window
    return;
  }

  for(QList<DockWidgetItem>::iterator i = dockWidgets.begin(), end = dockWidgets.end(); i != end; ++i)
    if(i->widget->getObject() == object)
    {
      DockWidget* widget = i->widget;
      widget->setVisible(true);
      widget->raise();
      widget->activateWindow();
      widget->setFocus();
      break;
    }
}

void MainWindow::closeObject(const SceneObject& object)
{
  if(object.type == SceneObject::Actuator) // sepcial handling for the gathered actuator window
  {
    if(actuatorsWidget)
    {
      actuatorsWidget->removeObject(object);
      if(actuatorsWidget->getObjectCount() == 0)
        Document::document->closeObject(SceneObject(".Actuators", SceneObject::Actuators));
    }
    return;
  }

  for(QList<DockWidgetItem>::iterator i = dockWidgets.begin(), end = dockWidgets.end(); i != end; ++i)
    if(i->widget->getObject() == object)
    {
      DockWidget* widget = i->widget;
      removeDockWidget(widget);
      if(widget == actuatorsWidget)
        actuatorsWidget = 0;
      if(widget == consoleWidget)
        consoleWidget = 0;
      if(widget == sceneGraphWidget)
        sceneGraphWidget = 0;
      if(widget == activeDockWidget)
      {
        activeDockWidget = 0;
        if(Document::document->isLayoutRestored())
        {
          if(hasFocus())
            updateMenuAndToolBarAndTitle();
          else
            setFocus();
        }
      }
      delete widget;
      dockWidgets.erase(i);
      return;
    }
}

void MainWindow::writeLayout()
{
  QSettings* settings = Document::document->getLayoutSettings();
  settings->beginGroup("MainWindow");

  settings->setValue("State", saveState()); // tool and dockwidgets stuff
  writeGeometry(*settings);

  settings->setValue("ShowMenu", !menuBar->hasAutoHide());
  settings->setValue("ShowStatus", statusBar->wasVisible());

  settings->endGroup();
}

void MainWindow::restoreLayout()
{
  QSettings* settings = Document::document->getLayoutSettings();
  settings->beginGroup("MainWindow");

  restoreGeometry(*settings);
  const QVariant& var(settings->value("State")); // tool and dockwidgets stuff
  if(var.isValid())
    restoreState(var.toByteArray());

  for(QList<DockWidgetItem>::iterator i = dockWidgets.begin(), end = dockWidgets.end(); i != end; ++i)
    if(i->widget->isFloating())
      i->widget->raise();
  if(dockWidgets.size() > 0)
  {
    QWidget* widget = dockWidgets.last().widget;
    widget->raise();
    widget->activateWindow();
    widget->setFocus();
  }

  bool showMenu = settings->value("ShowMenu", true).toBool();
  menuBar->setAutoHide(!showMenu );
  menuBar->setVisible(showMenu );

  statusBar->setVisible(settings->value("ShowStatus", true).toBool());

  settings->endGroup();
  updateMenuAndToolBarAndTitle();
}

void MainWindow::writeGeometry(QSettings& settings)
{
  settings.setValue("Geometry", normalGeometry);
  settings.setValue("WindowState", int(windowState()));
}

void MainWindow::restoreGeometry(QSettings& settings)
{
  const QVariant& var(settings.value("Geometry"));
  if(!var.isValid())
    return;
  const QRect& geometry(var.toRect());
  if(geometry.isEmpty())
    return;
  Qt::WindowStates newWindowState = Qt::WindowStates(settings.value("WindowState").toInt());

  Qt::WindowStates oldWindowState = windowState();
  if(oldWindowState & (Qt::WindowMinimized | Qt::WindowMaximized | Qt::WindowFullScreen))
  { // Make sure the window is not maximized or minimized. Otherwise we can't restore the window geometry.
#ifdef _WIN32
      WINDOWPLACEMENT wp;
      GetWindowPlacement(effectiveWinId(), &wp);
      wp.showCmd = SW_SHOWNORMAL;
      SetWindowPlacement(effectiveWinId(), &wp);
#endif
      setWindowState(oldWindowState & ~(Qt::WindowMinimized | Qt::WindowMaximized | Qt::WindowFullScreen));
  }

  Q_ASSERT(!(windowState() & (Qt::WindowMinimized | Qt::WindowMaximized | Qt::WindowFullScreen)));
  setGeometry(geometry);
#ifdef _WIN32
  if(newWindowState & Qt::WindowMaximized)
  {
    WINDOWPLACEMENT wp;
    GetWindowPlacement(effectiveWinId(), &wp);
    wp.showCmd = SW_SHOWMAXIMIZED;
    SetWindowPlacement(effectiveWinId(), &wp);
  }
#endif
  setWindowState(newWindowState);
  normalGeometry = geometry;
}


void MainWindow::moveEvent(QMoveEvent* event)
{
  if(!(windowState() & (Qt::WindowMinimized | Qt::WindowMaximized | Qt::WindowFullScreen)))
    normalGeometry = geometry();  
  QMainWindow::moveEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
  if(!(windowState() & (Qt::WindowMinimized | Qt::WindowMaximized | Qt::WindowFullScreen)))
    normalGeometry = geometry();
  QMainWindow::resizeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  if(fileSaveAct->isEnabled())
  {
    close();
    if(fileSaveAct->isEnabled())
    {
      event->ignore();
      return;
    }
  }

  setVisible(false); // optical close speedup
  Document::document->close();
  QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
  if((event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier)) == (Qt::ControlModifier | Qt::ShiftModifier) &&
    Document::document->handleKeyEvent(event->key(), true))
    event->accept();
  else
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
  if((event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier)) == (Qt::ControlModifier | Qt::ShiftModifier) &&
    Document::document->handleKeyEvent(event->key(), false))
      event->accept();
  else
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{  
  if(event->mimeData()->hasUrls())
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
  QList<QUrl> urls = event->mimeData()->urls();
  QString chdir;
  foreach(QUrl url, urls)
  {
    QString file(url.toLocalFile());
    if(!file.isEmpty())
    {
      openFile(file);
      break;
    }
  }
  event->acceptProposedAction();
}

void MainWindow::updateFileMenu()
{
  fileMenu->clear();
  fileMenu->addAction(fileNewAct);
  fileMenu->addAction(fileOpenAct);
  fileMenu->addAction(fileSaveAct);
  fileMenu->addAction(fileSaveAsAct);
  fileMenu->addAction(fileCloseAct);
  if(recentFiles.size() > 0)
  {
    fileMenu->addSeparator();
    char shortcut = '1';
    for(QStringList::const_iterator i = recentFiles.begin(), end = recentFiles.end(); i != end; ++i)
    {
      const QString& file(*i);
      QAction* action = fileMenu->addAction("&" + QString(shortcut++) + " " + QFileInfo(file).fileName());
      connect(action, SIGNAL(triggered()), &recentFileMapper, SLOT(map()));
      recentFileMapper.setMapping(action, file);
    }
  }
  fileMenu->addSeparator();
  fileMenu->addAction(fileExitAct);
}

void MainWindow::updateViewMenu()
{
  updateViewMenu(viewMenu);
}

void MainWindow::updateViewMenu(QMenu* menu)
{
  menu->clear();
  menu->addAction(menuBar->toggleViewAction());
  menu->addAction(toolBar->toggleViewAction());
  menu->addAction(statusBar->toggleViewAction());
  if(dockWidgets.size() > 0)
  {
    menu->addSeparator();
    qSort(dockWidgets.begin(), dockWidgets.end());    
    for(QList<DockWidgetItem>::iterator i = dockWidgets.begin(), end = dockWidgets.end(); i != end; ++i)
      menu->addAction(i->widget->toggleViewAction());
  }
}

QMenu* MainWindow::createPopupMenu()
{
  QMenu* menu = new QMenu();
  updateViewMenu(menu);
  return menu;
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About SimRobot"),
    tr("<b>SimRobot</b> 2009 for Qt4<br><table><tr><td>Authors:</td><td>Tim Laue</td></tr><tr><td></td><td>Thomas Röfer</td></tr><tr><td></td><td>Kai Spiess</td></tr><tr><td></td><td>Dennis Pachur</td></tr><tr><td></td><td>Armin Burchardt</td></tr><tr><td></td><td>Christian Büttner</td></tr><tr><td></td><td>Markus Küch</td></tr><tr><td></td><td>Tobias Schade</td></tr><tr><td></td><td>Colin Graf</td></tr><tr><td></td><td>Thijs Jeffry de Haas</td></tr></table><br><br>German Research Center for Artificial Intelligence (DFKI)<br>University of Bremen\n\n"));
}

void MainWindow::createActions()
{
  fileNewAct = new QAction(tr("&New"), this);
  fileNewAct->setShortcut(QKeySequence(QKeySequence::New));
  fileNewAct->setStatusTip(tr("Create a new simulation file"));
  connect(fileNewAct, SIGNAL(triggered()), this, SLOT(newFile()));

  fileOpenAct = new QAction(QIcon(":/icons/folder_page.png"), tr("&Open..."), this); 
  fileOpenAct->setShortcut(QKeySequence(QKeySequence::Open));
  fileOpenAct->setStatusTip(tr("Open an existing simulation file"));
  connect(fileOpenAct, SIGNAL(triggered()), this, SLOT(open()));

  fileSaveAct = new QAction(QIcon(":/icons/disk.png"), tr("&Save"), this); 
  fileSaveAct->setShortcut(QKeySequence(QKeySequence::Save));
  fileSaveAct->setStatusTip(tr("Save the document to disk"));
  connect(fileSaveAct, SIGNAL(triggered()), this, SLOT(save()));

  fileSaveAsAct = new QAction(tr("Save &As..."), this);
  fileSaveAsAct->setStatusTip(tr("Save the document under a new name"));
  connect(fileSaveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  fileCloseAct = new QAction(tr("&Close"), this);
  fileCloseAct->setStatusTip(tr("Close the document"));
  connect(fileCloseAct, SIGNAL(triggered()), this, SLOT(close()));

  fileExitAct = new QAction(/*QIcon(":/icons/Exit.png"), */tr("E&xit"), this);
#ifdef _WIN32
  fileExitAct->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4/*, Qt::CTRL + Qt::Key_Q*/));
#else
  fileExitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q, Qt::ALT + Qt::Key_F4));
#endif
  fileExitAct->setStatusTip(tr("Exit the application"));
  connect(fileExitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
  
  simResetAct = new QAction(QIcon(":/icons/control_start_blue.png"), tr("&Reset"), this);
  simResetAct->setStatusTip(tr("Reset the simulation to the beginning"));
  simResetAct->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F5));
  connect(simResetAct, SIGNAL(triggered()), this, SLOT(simReset()));

  simStartAct = new QAction(QIcon(":/icons/control_play_blue.png"), tr("&Start"), this);
  simStartAct->setStatusTip(tr("Start or stop the simulation"));
  simStartAct->setShortcut(QKeySequence(Qt::Key_F5));
  simStartAct->setCheckable(true);
  connect(simStartAct, SIGNAL(triggered()), this, SLOT(simStart()));

  simStepAct = new QAction(QIcon(":/icons/control_step_blue.png"), tr("&Step"), this);
  simStepAct->setStatusTip(tr("Execute a single simulation step"));
  simStepAct->setShortcut(QKeySequence(Qt::Key_F8));
  connect(simStepAct, SIGNAL(triggered()), this, SLOT(simStep()));

  fitWindowSizeAct = new QAction(QIcon(":/icons/application_fit.png"), tr("&Fit Size")/*tr("&Fit Window Size")*/, this);
  connect(fitWindowSizeAct, SIGNAL(triggered()), this, SLOT(fitWindowSize()));
}

bool MainWindow::DockWidgetItem::operator<(const DockWidgetItem& other) const
{
  if(widget->getObject().name < other.widget->getObject().name)
    return true;
  return widget->getObject().name == other.widget->getObject().name && widget->getObject().type < other.widget->getObject().type;
}

void MainWindow::focusChanged(QWidget *old, QWidget* now)
{
  QWidget* newActive = now;
  while(newActive)
  {
    if(newActive->inherits("QDockWidget"))
      break;
    newActive = newActive->parentWidget();
  }

  if(!newActive && activeDockWidget)
    if(activeDockWidget->isVisible())
      return;

  if(newActive != activeDockWidget)
  {
    if(activeDockWidget)
      ((DockWidget*)activeDockWidget)->setActive(false);
    if(newActive)
      ((DockWidget*)newActive)->setActive(true);
    activeDockWidget = (DockWidget*)newActive;
    if(Document::document->isLayoutRestored())
    {
      updateMenuAndToolBarAndTitle();
      if(activeDockWidget)
      {        
        Document::document->raiseObject(activeDockWidget->getObject());
        if(activeDockWidget->isFloating())
          setFocus();
      }
    }
  }
}

void MainWindow::fitWindowSize()
{
  if(activeDockWidget)
    activeDockWidget->fitWindowSize();
}

void MainWindow::modificationChanged(bool changed)
{
  fileSaveAct->setEnabled(changed);
  updateWindowTitle();
}

void MainWindow::simReset()
{
  if(fileSaveAct->isEnabled()) // file content changed and should be saved first
  {
    save();
    if(fileSaveAct->isEnabled())
      return;
  }
  Document::document->simReset();
}

void MainWindow::simStart()
{
  if(Document::document->isRunning())
  {
    Document::document->simStop();
    simStartAct->setChecked(false);
    return;
  }
  if(fileSaveAct->isEnabled()) // file content changed and should be saved first
  {
    simReset();
    if(fileSaveAct->isEnabled())
    {
      simStartAct->setChecked(false);
      return;
    }
  }
  Document::document->simStart();
  simStartAct->setChecked(Document::document->isRunning());
}

void MainWindow::simStep()
{
  if(fileSaveAct->isEnabled()) // file content changed and should be saved first
  {
    simReset();
    if(fileSaveAct->isEnabled())
      return;
  }
  Document::document->simStep();
}
