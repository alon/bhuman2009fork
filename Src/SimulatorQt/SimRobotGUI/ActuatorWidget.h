
#ifndef ActuatorWidget_H
#define ActuatorWidget_H

#include <QHash>

#include "DockWidget.h"

class ActuatorView;
class QSlider;
class QSpinBox;
class QPushButton;
class QHBoxLayout;
class QCheckBox;

class ActuatorWidget : public DockWidget
{
public:
  ActuatorWidget(const SceneObject& object, QWidget* parent);

  void addObject(const SceneObject& obj);
  void removeObject(const SceneObject& obj);
  int getObjectCount();

private:
  QHash<QString, ActuatorView*> actuators;
  QHBoxLayout *layout;  

  void closeEvent(QCloseEvent* event);
};

class ActuatorView : public QWidget
{
  Q_OBJECT

public:
  ActuatorView(const SceneObject& object, QWidget* parent);

  const SceneObject& getObject() { return object; }

public slots:
  void valueChangedSlot(int value);
  void closeSlot();
  void updateActuatorSlot();
  void updateObject();
  void writeLayout();
  void restoreLayout();

private:
  SceneObject object;
  bool isAngle;
  int actuatorPortId;
  QSlider* slider;
  QSpinBox* txbValue;
  QPushButton* btnExit;
  QCheckBox* cbxSet;
  int value;
  bool set;

  QSize sizeHint () const;
};

#endif //ActuatorWidget_H
