/**
* @file ControllerQt/Views/SensorView.h
*
* Declaration of class SensorView
*
* @author of original sensorview <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author Jeff
* @author Colin Graf
*/

#ifndef SensorView_H
#define SensorView_H

#include <SimRobotCore/Controller/DirectView.h>

class RobotConsole;
class SensorData;
class SensorWidget;

/**
* @class SensorView 
* A class to represent a view with information about the sensor values.
*/
class SensorView : public DirectView
{
public:
  /**
  * Constructor.
  * @param robotConsole The robot console which owns \c sensorData and \c filteredSensorData.
  * @param sensorData A reference to the jointData representation of the robot console.
  * @param filteredSensorData A reference to the jointData representation of the robot console.
  */
  SensorView(RobotConsole& robotConsole, const SensorData& sensorData, const SensorData& filteredSensorData);

private:

  RobotConsole& console; /**< A reference to the console object. */
  const SensorData&  sensorData; /**< The most current sensor data. */
  const SensorData& filteredSensorData; /**< A reference to the filtered sensor data of the robot console. */
  QWidget* sensorWidget; /**< The Qt widget for displaying the sensor data. */
  unsigned int lastUpdateTimeStamp; /**< Timestamp of the last painted sensor data. */

  /**
  * The method returns a new instance of a widget for this direct view.
  * The caller has to delete this instance. (Qt handles this)
  * @return The widget.
  */
  QWidget* createWidget();

  /**
  * The function is called to let the direct view update the widget. Do so if necessary.
  * @param widget The widget
  */
  void updateWidget(QWidget& widget);

  /**
  * The function is called to return a string that contains the configuration of the widget/view.
  * @param widget The widget
  * @return A text string containing all information to restore the internal state of the widget/view.
  */
  virtual std::string getConfiguration(QWidget& widget) const;

  /**
  * The function is called to restore the configuration of the widget/view.
  * @param widget The widget
  * @param A text string containing all information to restore the internal state of the widget/view.
  */
  virtual void setConfiguration(QWidget& widget, const std::string& configuration);

  friend class SensorWidget;
};

#endif //__SensorView_h_
