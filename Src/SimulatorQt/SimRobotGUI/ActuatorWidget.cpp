
#include <QLabel>
#include <QVBoxLayout>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QSettings>

#include <Simulation/Simulation.h>

#include "Document.h"
#include "ActuatorWidget.h"

ActuatorView::ActuatorView(const SceneObject& object, QWidget* parent) : QWidget(parent),
  object(object), actuatorPortId(-1), value(0)
{
  setMaximumWidth(80);
  QStringList nameList = object.name.split(".");
  isAngle = nameList[nameList.size() - 1] == "angle";

  slider = new QSlider(Qt::Vertical);
  btnExit = new QPushButton(tr("Close"), this);
  txbValue = new QSpinBox(this);
  QLabel* label = new QLabel(nameList[nameList.size() - 2], this);

  cbxSet = new QCheckBox(tr("set"), this);

  QVBoxLayout *layout = new QVBoxLayout(this);

  layout->addWidget(label);
  layout->addWidget(slider, 0, Qt::AlignHCenter);
  layout->addWidget(txbValue);
  layout->addWidget(cbxSet);
  layout->addWidget(btnExit);

  setLayout(layout);

  connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
  connect(txbValue, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
  connect(btnExit, SIGNAL(released()), this, SLOT(closeSlot()));

  connect(Document::document, SIGNAL(updateActuatorsSignal()), this, SLOT(updateActuatorSlot()));
  connect(Document::document, SIGNAL(updatedSceneGraph()), this, SLOT(updateObject()));
  connect(Document::document, SIGNAL(restoreLayoutSignal()), this, SLOT(restoreLayout()));
  connect(Document::document, SIGNAL(writeLayoutSignal()), this, SLOT(writeLayout()));
  updateObject();
}

QSize ActuatorView::sizeHint () const { return QSize(80, 200); }

void ActuatorView::updateObject()
{
  Simulation *sim = Document::document->getSimulation();
  actuatorPortId = -1;
  if(sim)
  {
    actuatorPortId = sim->getActuatorportId(std::vector<std::string>(1,object.name.toAscii().constData()));
    if(actuatorPortId >= 0)
    {
      double minVal = sim->getActuatorportMinValue(actuatorPortId);
      double maxVal = sim->getActuatorportMaxValue(actuatorPortId);

      if(isAngle)
      {
        minVal = Functions::toDeg(minVal);
        maxVal = Functions::toDeg(maxVal);
      }

      slider->setRange((int)minVal, (int)maxVal);
      slider->setValue(value);

      txbValue->setRange((int)minVal, (int)maxVal);
      txbValue->setValue(value);
    }
  }
}

void ActuatorView::valueChangedSlot(int value)
{
  txbValue->setValue(value);
  slider->setValue(value);
  this->value = value;
}

void ActuatorView::closeSlot()
{
  Document::document->closeObject(object);
}

void ActuatorView::updateActuatorSlot()
{
  if(cbxSet->checkState() == Qt::Checked)
  {
    Simulation *sim = Document::document->getSimulation();
    double value = (double)this->value;
    if(isAngle)
      value = Functions::toRad(value);
    sim->setActuatorport(actuatorPortId, value);
  }
}


void ActuatorView::restoreLayout()
{
  QSettings* settings = Document::document->getLayoutSettings();
  settings->beginGroup(object.name);
  valueChangedSlot(settings->value("Value", int(0)).toInt());
  cbxSet->setChecked(settings->value("Set", false).toBool());
  settings->endGroup();
}

void ActuatorView::writeLayout()
{
  QSettings* settings = Document::document->getLayoutSettings();
  settings->beginGroup(object.name);
  settings->setValue("Set", cbxSet->checkState() == Qt::Checked);
  settings->setValue("Value", value);
  settings->endGroup();
}



class ActuatorSubWidget : public QWidget
{
public:
  ActuatorSubWidget(QWidget* parent) : QWidget(parent) { setFocusPolicy(Qt::StrongFocus); }
private:
  QSize sizeHint () const { return QSize(80, 200); }
};

ActuatorWidget::ActuatorWidget(const SceneObject& object, QWidget* parent) : DockWidget(object, parent)
{
  setWindowTitle(tr("Actuators"));
  QWidget* widget = new ActuatorSubWidget(this);
  QHBoxLayout* outerlayout = new QHBoxLayout(widget);
  layout = new QHBoxLayout();
  outerlayout->setContentsMargins(0,0,0,0);
  outerlayout->addLayout(layout);
  widget->setLayout(outerlayout);
  setWidget(widget);
  setFocusProxy(widget);
}

void ActuatorWidget::addObject(const SceneObject& object)
{
  if(actuators.contains(object.name))
    return;

  ActuatorView *widget = new ActuatorView(object, this);
  layout->addWidget(widget);
  actuators[object.name] = widget;
}

void ActuatorWidget::removeObject(const SceneObject& object)
{
  if(!actuators.contains(object.name))
    return;

  QWidget* widget = actuators[object.name];
  layout->removeWidget(widget);
  actuators.remove(object.name);
  widget->setAttribute(Qt::WA_DeleteOnClose);
  widget->close();
}

int ActuatorWidget::getObjectCount()
{
  return actuators.size();
}

void ActuatorWidget::closeEvent(QCloseEvent* event)
{
  QDockWidget::closeEvent(event);

  for(QHash<QString, ActuatorView*>::iterator i = actuators.begin(), next, end = actuators.end(); i != end; i = next)
  {
    next = i;
    ++next;
    SceneObject obj(i.value()->getObject());
    actuators.remove(obj.name);
    Document::document->closeObject(obj);
  }
}
