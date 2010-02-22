
#ifndef SensorWidget_H
#define SensorWidget_H

#include "DockWidget.h"

class DirectView;
class QMimeData;

class SRSensorWidget : public DockWidget
{
  Q_OBJECT

public:
  SRSensorWidget(const SceneObject& object, QWidget* parent);

  QMenu* createPopupMenu();

private slots:
  void updateView();
  void updateObject();
  void restoreLayout();
  void writeLayout();
  void copy();

private:
  DirectView* directView;
  QWidget* widget;
  std::string conf; /**< A restored configuration (for delayed loaded direct views). */  
  SensorReading::SensorType sensorType;
  std::vector<int> sensorDimensions;  

  void setClipboardGraphics(QMimeData& mimeData);
  void setClipboardText(QMimeData& mimeData);

  friend class SRSensorViewWidget;
};

#endif // SensorWidget_H
