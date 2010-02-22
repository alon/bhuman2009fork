/**
* @file ControllerQt/Views/JointView.h
* Declaration of class for displaying the requested and measured joint angles.
* @author Colin Graf
*/

#ifndef JointView_H
#define JointView_H

#include "SimRobotCore/Controller/DirectView.h"

class RobotConsole;
class JointData;
class JointRequest;
class SensorData;
class QWidget;
class JointWidget;

/**
* @class JointView 
* A class implements a DirectView for displaying the requested and measured joint angles.
*/
class JointView : public DirectView
{
public:
  /**
  * Constructor.
  * @param robotConsole The robot console which owns \c jointData, \c sensorData and \c jointRequest.
  * @param jointData A reference to the jointData representation of the robot console.
  * @param sensorData A reference to the jointData representation of the robot console.
  * @param jointRequest A reference to the jointRequest representation of the robot console.
  */
  JointView(RobotConsole& robotConsole, const JointData& jointData, const SensorData& sensorData, const JointData& jointRequest);

private:
  RobotConsole& console; /**< A reference to the console object. */
  const JointData& jointData; /**< A reference to the jointData representation of the robot console. */
  const SensorData& sensorData; /**< A reference to the sensorData representation of the robot console. */
  const JointData& jointRequest; /**< A reference to the jointRequest representation of the robot console. */
  QWidget* jointWidget; /**< The Qt widget for displaying the angles. */
  unsigned int lastUpdateTimeStamp; /**< Timestamp of the last painted joint angles. */

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

  friend class JointWidget;
};

#endif //JointView_H
