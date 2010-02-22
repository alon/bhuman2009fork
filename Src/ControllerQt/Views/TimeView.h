/**
* @file ControllerQt/Views/TimeView.h
*
* Declaration of class TimeView
*
* @author Colin Graf
*/

#ifndef __TIMEVIEW_H__
#define __TIMEVIEW_H__

#include "SimRobotCore/Controller/DirectView.h"

class RobotConsole;
class TimeInfo;
class QWidget;

/**
* @class TimeView 
* 
* A class to represent a view with information about the timing of modules.
*
* @author Colin Graf
*/
class TimeView : public DirectView
{
public:
  /**
  * Constructor.
  * @param console The console object.
  * @param info The timing info object to be visualized.
  */
  TimeView(RobotConsole& console, const TimeInfo& info);

private:
  RobotConsole& console; /**< A reference to the console object. */
  const TimeInfo& info; /**< The Time info structure. */
  QWidget* timeWidget; /**< The Xabsl widget that displays the info. */
  unsigned int lastTimeInfoTimeStamp; /**< Timestamp of the last painted info. */

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

};

#endif //__TIMEVIEW_H__
