
#ifndef MenuBar_H
#define MenuBar_H

#include <QMenuBar>

class MenuBar : public QMenuBar
{
  Q_OBJECT

public:
  MenuBar(QWidget* parent = 0);

  QAction* toggleViewAction();

  void setAutoHide(bool enable);
  bool hasAutoHide();

private slots:
  void toggleView(bool enable);

private:
  bool autoHide;
  bool altPressed;
  bool fullyVisible;
  QAction* toggleViewAct;

  virtual void paintEvent(QPaintEvent *event);
  virtual bool eventFilter(QObject* object, QEvent* event);
};

#endif //!MenuBar_H
