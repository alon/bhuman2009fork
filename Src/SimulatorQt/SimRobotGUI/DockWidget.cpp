
#include <QContextMenuEvent>
#include <QMenu>
#include <QLayout>
#include <QApplication>
/*
#ifdef _WIN32
#include <windows.h>
#endif
*/
#include "DockWidget.h"
#include "MainWindow.h"


#include <QPainter>
//#include <QLinearGradient>
#include <QToolButton>
#include <QStyleOptionMenuItem>

class DockTitleWidget : public QWidget
{
public:
  DockTitleWidget(QWidget* parent) : QWidget(parent)
  {
    height = QApplication::fontMetrics().height()+ 3;
    floatButton = new QToolButton(this);    
    closeButton = new QToolButton(this);
    floatButton->setIcon(QIcon(":/icons/dock_restore.png"));
    closeButton->setIcon(QIcon(":/icons/dock_close.png"));
    floatButton->setAutoRaise(true);
    closeButton->setAutoRaise(true);
    floatButton->setCheckable(true);
    closeButton->setToolTip(tr("Close"));
    floatButton->setToolTip(tr("Dockable"));
  }

  QSize sizeHint() const { return QSize(height, height); }
  QSize minimumSizeHint() const { return QSize(height, height); }

  QToolButton* floatButton;
  QToolButton* closeButton;

private:
  int height;

  /*
#ifdef _WIN32
  inline QColor getSysColor(int index)
  {
    DWORD color = GetSysColor(index);
    return QColor(color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff);
  }
#endif
*/
  void paintEvent(QPaintEvent* event)
  {
    DockWidget* parent((DockWidget*)this->parent());
    bool vert = parent->features() & QDockWidget::DockWidgetVerticalTitleBar;
    bool active = parent->hasFocus();

    QRect rect(this->rect());
    QPainter painter(this);

    if(vert)
    {
      painter.rotate(-90);   
      painter.translate(-rect.height(), 0);
      rect = painter.transform().inverted().mapRect(rect);
    }

    QStyle* style(this->style());

    /*
    QStyleOptionTitleBar sotb;
    int fw = style->pixelMetric(QStyle::PM_MDIFrameWidth, &sotb, this);
    sotb.rect = rect.adjusted(-fw, -fw, fw, 0);
    sotb.state = active ? (QStyle::State_Enabled|QStyle::State_Active) : QStyle::State_None;
    sotb.titleBarState = active ? QStyle::State_Active : 0;
    style->drawComplexControl(QStyle::CC_TitleBar, &sotb, &painter, this);
    */
    QStyleOption so;
    so.rect = rect.adjusted(-1, 0, 1, 0);
    so.state = active ? (QStyle::State_Enabled|QStyle::State_Active) : QStyle::State_None;
    style->drawControl(QStyle::CE_MenuBarEmptyArea, &so, &painter, this);

    rect.setLeft(rect.left() + 4);
    rect.setRight(rect.right() - 4 - height - height);
    QPalette palette(this->palette());
    palette.setCurrentColorGroup(active ? QPalette::Active : QPalette::Disabled);
    style->drawItemText(&painter, rect, Qt::AlignVCenter | Qt::TextSingleLine, palette, true, parent->windowTitle(), active ? QPalette::ButtonText : QPalette::Text);

    /*
    const QPalette& pal(QApplication::palette());
#ifdef _WIN32
    const QColor& bg(getSysColor(active ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION));
    const QColor& text(getSysColor(active ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT));
#else    
    const QColor& bg(pal.color(active ? QPalette::Highlight : QPalette::Shadow));
    const QColor& text(pal.color(active ? QPalette::HighlightedText : QPalette::Window));
#endif

    QRect textRect(rect());
    int split = vert ? ((textRect.height() - height - height) / 2 + height + height) : ((textRect.width() - height - height) / 2);
    QLinearGradient gradient(
      vert ? 0 : split, 
      vert ? split : 0, 
      vert ? 0 : (textRect.width() - height - height), 
      vert ? (textRect.top() + height + height) : 0);
    gradient.setColorAt(0, bg);
    gradient.setColorAt(1, pal.color(QPalette::Window));
    if(vert)
    {
      painter.fillRect(0, split, textRect.width(), textRect.height() - split, bg);
      painter.fillRect(0, 0, textRect.width(), split, gradient);
    }
    else
    {
      painter.fillRect(0, textRect.top(), split, textRect.height(), bg);
      painter.fillRect(split, textRect.top(), textRect.width() - split, textRect.height(), gradient);
    }
    painter.setPen(text);
    int space = 3 + 1;
    if(vert)
    {
      textRect.setTop(textRect.top() + space + height + height);
      textRect.setBottom(textRect.bottom() - space);
      painter.rotate(-90);   
      painter.translate(-rect().height(), 0);
         
      textRect = painter.transform().inverted().mapRect(textRect);
    }
    else
    {
      textRect.setLeft(textRect.left() + space);
      textRect.setRight(textRect.right() - space - height - height);
    }
    painter.drawText(textRect, Qt::AlignVCenter | Qt::TextSingleLine, parent->windowTitle());
    */
  }

  void resizeEvent(QResizeEvent* event)
  {
    QWidget::resizeEvent(event);

    DockWidget* parent((DockWidget*)this->parent());
    bool vert = parent->features() & QDockWidget::DockWidgetVerticalTitleBar;
    QRect buttonPos(rect());
    if(vert)
      buttonPos.setBottom(buttonPos.top() + height);
    else
      buttonPos.setLeft(buttonPos.right() - height);
    closeButton->setGeometry(buttonPos);
    if(vert)
    {
      buttonPos.setTop(buttonPos.bottom());
      buttonPos.setBottom(buttonPos.top() + height);
    }
    else
    {
      buttonPos.setRight(buttonPos.left());
      buttonPos.setLeft(buttonPos.right() - height);
    }
    floatButton->setGeometry(buttonPos);
  }

};

DockWidget::DockWidget(const SceneObject& object, QWidget* parent) : QDockWidget(parent),
  object(object), visible(false), active(false)
{
  setObjectName(object.name);
  setFocusPolicy(Qt::StrongFocus);
  titleWidget = new DockTitleWidget(this);
  setTitleBarWidget(titleWidget);

  if(object.type == SceneObject::Object || object.type == SceneObject::Sensor || object.type == SceneObject::Actuator)
  {
    int nameStart = object.name.lastIndexOf(QChar('.'));
    if(nameStart >= 0)
    {
      QString name(object.name.mid(nameStart + 1) + " - " + object.name);
      *name.data() = name.data()->toUpper();
      setWindowTitle(name);
    }
    else
      setWindowTitle(object.name);
  }

  connect(((DockTitleWidget*)titleWidget)->closeButton, SIGNAL(released()), this, SLOT(close()));
  connect(((DockTitleWidget*)titleWidget)->floatButton, SIGNAL(released()), this, SLOT(makeFloating()));
  connect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(visibilityChanged(bool)));
}


void DockWidget::closeEvent(QCloseEvent* event)
{
  QDockWidget::closeEvent(event);
  if(object.type == SceneObject::Object || object.type == SceneObject::Sensor || object.type == SceneObject::Actuator)
    Document::document->closeObject(object);
}

void DockWidget::contextMenuEvent(QContextMenuEvent* event)
{
  QWidget* widget(this->widget());
  if(!widget)
  {
    QDockWidget::contextMenuEvent(event);
    return;
  }

  QRect content(widget->geometry());
  if(!content.contains(event->x(), event->y()))
  { // click on window frame
    //QDockWidget::contextMenuEvent(event);
    QMainWindow* parent((QMainWindow*)this->parent());
    QMenu* menu = parent->createPopupMenu();
    if(menu)
    {
      menu->exec(mapToGlobal(QPoint(event->x(), event->y())));
      delete menu;
    }
    return;
  };

  QMenu* menu = createPopupMenu();
  if(menu)
  {
    const QList<QAction*> actions(menu->actions());
    if(actions.size() == 0)
    {
      delete menu;
      menu = 0;
    }
    else
    {
      event->accept();
      MainWindow* mainWindow = (MainWindow*)parent();
      QAction* firstAction = actions.at(0);      
      menu->insertAction(firstAction, mainWindow->simStartAct);
      menu->insertAction(firstAction, mainWindow->simResetAct);
      menu->insertAction(firstAction, mainWindow->simStepAct);
      menu->insertSeparator(firstAction);
      menu->insertAction(firstAction, mainWindow->fitWindowSizeAct);
      menu->insertSeparator(firstAction);
      menu->exec(mapToGlobal(QPoint(event->x(), event->y())));
      delete menu;
    }
  }
  if(!menu)
  {
    //QDockWidget::contextMenuEvent(event);
    event->accept();
    QMenu menu;
    MainWindow* mainWindow = (MainWindow*)parent();
    menu.addAction(mainWindow->simStartAct);
    menu.addAction(mainWindow->simResetAct);
    menu.addAction(mainWindow->simStepAct);
    menu.addSeparator();
    menu.addAction(mainWindow->fitWindowSizeAct);
    menu.exec(mapToGlobal(QPoint(event->x(), event->y())));
  }
}

void DockWidget::fitWindowSize()
{
  QWidget* widget = this->widget();
  if(widget)
  {
    QSize size(widget->sizeHint());
    if(size.isEmpty() || size == QSize(1, 1))
       size = QSize(640, 480);
    QWidget* parent = (QWidget*)this->parent();
    parent->setUpdatesEnabled(false);
    widget->setFixedSize(size);
    parent->layout()->update();
    QApplication::processEvents();
    widget->setMinimumSize(QSize(0, 0));
    widget->setMaximumSize(QSize(16777215, 16777215));
    parent->layout()->update();
    QApplication::processEvents();
    parent->setUpdatesEnabled(true);
    parent->repaint();
  }
}

void DockWidget::visibilityChanged(bool visible)
{
  this->visible = visible;
  if(visible && Document::document->isLayoutRestored())
  {
    raise();
    activateWindow();
    setFocus();
  }
}

void DockWidget::setActive(bool active)
{
  this->active = active;
  setStyleSheet(active ? "QDockWidget { font-weight: bold; }" : "");
}

void DockWidget::makeFloating()
{
  bool floating = allowedAreas() == Qt::AllDockWidgetAreas;
  if(floating)
    setFloating(floating);
  setAllowedAreas(floating ? Qt::NoDockWidgetArea : Qt::AllDockWidgetAreas);
  ((DockTitleWidget*)titleWidget)->floatButton->setChecked(floating);
}

void DockWidget::paintEvent(QPaintEvent* event)
{
  if(isFloating())
  {
    QPainter painter(this);
    qDrawPlainRect(&painter, rect(), palette().color(QPalette::Mid));
    setContentsMargins(0, 0, 0, 0);
  }
}