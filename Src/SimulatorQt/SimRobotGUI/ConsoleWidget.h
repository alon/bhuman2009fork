
#ifndef ConsoleWidget_H
#define ConsoleWidget_H

#include "DockWidget.h"

class ConsoleTextEdit;

class ConsoleWidget : public DockWidget
{
  Q_OBJECT

public:
  ConsoleWidget(const SceneObject& object, QWidget* parent);

  QAction* toggleViewAction() const;

private:
  ConsoleTextEdit* textEdit;

private slots:
  void clear();
  void print(const QString& text);
};

#endif //ConsoleWidget_H

