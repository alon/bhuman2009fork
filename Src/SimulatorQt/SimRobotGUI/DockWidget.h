
#ifndef DockWidget_H
#define DockWidget_H

#include <QDockWidget>

#include "Document.h"

class QMenu;

class DockWidget : public QDockWidget
{
  Q_OBJECT

public:
  DockWidget(const SceneObject& object, QWidget* parent);

  const SceneObject& getObject() const { return object; }

  virtual QMenu* createPopupMenu() { return 0; }

  virtual QAction* toggleViewAction() const { return QDockWidget::toggleViewAction(); }

  void fitWindowSize();

  bool isReallyVisible() const { return visible; };

  void setActive(bool active);
  bool isActive() const { return active; }

protected:
  SceneObject object;

private slots:
  void visibilityChanged(bool visible);
  void makeFloating();

private:
  bool visible;
  bool active;
  QWidget* titleWidget;

  void closeEvent(QCloseEvent* event);
  void contextMenuEvent(QContextMenuEvent* event);
  void paintEvent(QPaintEvent* event);
};

#endif