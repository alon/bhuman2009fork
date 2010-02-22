/**
* @file ControllerQt/Views/XabslView.h
*
* Declaration of class XabslView
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __XabslView_h_
#define __XabslView_h_

#include "SimRobotCore/Controller/DirectView.h"

class RobotConsole;
class XabslInfo;
class QWidget;

/**
* @class XabslView 
* 
* A class to represent a view with information about a Xabsl behavior.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/
class XabslView : public DirectView
{
public:
  /**
  * Constructor.
  * @param console The console object.
  * @param info The Xabsl info object to be visualized.
  */
  XabslView(RobotConsole& console, const XabslInfo& info);

private:
  RobotConsole& console; /**< A reference to the console object. */
  const XabslInfo& info; /**< The Xabsl info structure. */
  QWidget* xabslWidget; /**< The Xabsl widget that displays the info. */
  unsigned int lastXabslInfoTimeStamp; /**< Timestamp of the last painted info. */

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

#endif //__XabslView_h_
