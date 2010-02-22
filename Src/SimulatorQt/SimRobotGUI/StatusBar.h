
#ifndef StatusBar_H
#define StatusBar_H

#include <QStatusBar>

class QLabel;

class StatusBar : public QStatusBar
{
  Q_OBJECT

public:
  StatusBar(QWidget* parent);

  QAction* toggleViewAction();
  bool wasVisible() const;

public slots:
  void setVisible(bool visible);

private slots:
  void updateSlot();

private:
  QLabel* collisionsLabel;
  QLabel* frameRateLabel;
  QLabel* stepsLabel;
  QLabel* rendererLabel;
  QString lastStatusMessage;
  bool lastGotSimulation;
  int lastRenderingMethod;
   QAction* toggleViewAct;
  bool visible;

  void hideEvent(QHideEvent* event);
  void showEvent(QShowEvent* event);
};

#endif //StatusBar_H
