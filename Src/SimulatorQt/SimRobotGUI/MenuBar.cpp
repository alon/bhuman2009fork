
#include <QPaintEvent>
#include <QStyle>
#include <QApplication>

#include "MenuBar.h"

MenuBar::MenuBar(QWidget* parent) : QMenuBar(parent), autoHide(false), altPressed(false), fullyVisible(false), toggleViewAct(0) {}

void MenuBar::paintEvent(QPaintEvent *event)
{
  QMenuBar::paintEvent(event);

  QAction* activeAct = activeAction();
  if(activeAct)
    fullyVisible = true;
  if(autoHide && !hasFocus() && activeAct == 0 && fullyVisible)
    setVisible(false);
}

bool MenuBar::eventFilter(QObject* object, QEvent* event)
{
  if(!isVisible() && style()->styleHint(QStyle::SH_MenuBar_AltKeyNavigation, 0, this))
  {
    switch (event->type())
    {
    case QEvent::KeyPress:
    {
      QKeyEvent *kev = static_cast<QKeyEvent*>(event);
      if(kev->modifiers() == Qt::AltModifier)
      {
        const QList<QAction*>& actionList(actions());
        QKeySequence keySeq(Qt::ALT + kev->key());
        for(int i = 0; i < actionList.count(); i++)
          if(keySeq.matches(QKeySequence::mnemonic(actionList.at(i)->text())))
          {
            fullyVisible = false;
            setVisible(true);
            setActiveAction(actionList.at(i));
            altPressed = false;
            qApp->removeEventFilter(this);;
            return true;
          }
      }
      break;
    }      
    default:
      break;
    }

    if(altPressed)
    {
      switch (event->type())
      {
      case QEvent::KeyPress:
      case QEvent::KeyRelease:
      {
        QKeyEvent *kev = static_cast<QKeyEvent*>(event);
        if (kev->key() == Qt::Key_Alt || kev->key() == Qt::Key_Meta)
        {
          if (event->type() == QEvent::KeyPress) // Alt-press does not interest us, we have the shortcut-override event
              break;
          
          fullyVisible = false;
          setVisible(true);
          QKeyEvent press(QEvent::ShortcutOverride, Qt::Key_Alt, Qt::NoModifier);
          QMenuBar::eventFilter(0, &press);
          QKeyEvent release(QEvent::KeyRelease, Qt::Key_Alt, Qt::NoModifier);
          QMenuBar::eventFilter(0, &release);
        }
      }
      // fall through
      case QEvent::MouseButtonPress:
      case QEvent::MouseButtonRelease:
      //case QEvent::MouseMove:
      case QEvent::FocusIn:
      case QEvent::FocusOut:
      case QEvent::ActivationChange:
        altPressed = false;
        qApp->removeEventFilter(this);
        break;

      default:
        break;
      }
    } 
    else if (event->type() == QEvent::ShortcutOverride)
    {
      QKeyEvent *kev = static_cast<QKeyEvent*>(event);
      if((kev->key() == Qt::Key_Alt || kev->key() == Qt::Key_Meta)
          && kev->modifiers() == Qt::AltModifier)
      {
        altPressed = true;
        qApp->installEventFilter(this);
      }
    }    
  }

  return QMenuBar::eventFilter(object, event);
}

void MenuBar::setAutoHide(bool enable)
{
  autoHide = enable;
  if(toggleViewAct)
    toggleViewAct->setChecked(!autoHide);
}

bool MenuBar::hasAutoHide()
{
  return autoHide;
}

QAction* MenuBar::toggleViewAction()
{
  if(toggleViewAct)
    return toggleViewAct;
  toggleViewAct = new QAction(tr("&Menu Bar"), this);
  toggleViewAct->setCheckable(true);
  toggleViewAct->setChecked(!autoHide);
  //toggleViewAct->setShortcut(QKeySequence(Qt::ALT + Qt::SHIFT + Qt::Key_M));
  connect(toggleViewAct, SIGNAL(triggered(bool)), this, SLOT(toggleView(bool)));
  return toggleViewAct;
}

void MenuBar::toggleView(bool enable)
{
  setAutoHide(!enable);
  setVisible(enable);
}
