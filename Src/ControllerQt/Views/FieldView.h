/**
* @file ControllerQt/Views/FieldView.h
*
* Declaration of class FieldView
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __FIELDVIEW_H__
#define __FIELDVIEW_H__

#include <SimRobotCore/Controller/DirectView.h>

class RobotConsole;

/**
* @class FieldView 
* 
* A class to represent a view displaying debug drawings in field coordinates.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/
class FieldView : public DirectView
{
public:
  /**
  * Constructor.
  * @param console The console object.
  * @param name The name of the view.
  */
  FieldView(RobotConsole& console, const std::string& name);

private:
  RobotConsole& console; /**< A reference to the console object. */
  const std::string name; /**< The name of the view. */

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

#endif //__FIELDVIEW_H__
