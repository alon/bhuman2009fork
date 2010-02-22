/**
* @file Controller/DirectView.h
*
* Declaration of class DirectView
*
* @author Colin Graf
*/

#ifndef __DIRECTVIEW_H__
#define __DIRECTVIEW_H__

#include "../Controller/View.h"

class QWidget;

/**
* The class is the base class for direct views, i.e. views that have their own widget.
*/
class DirectView : public View
{
public:

  /**
  * The method returns a new instance of a widget for this direct view.
  * The caller has to delete this instance. (Qt handles this)
  * @return The widget.
  */
  virtual QWidget* createWidget() { return 0; }

  /**
  * The function is called to let the direct view update the widget. Do so if necessary.
  * @param widget The widget
  */
  virtual void updateWidget(QWidget& widget) {}

  /**
  * The function is called to return a string that contains the configuration of the widget/view.
  * @param widget The widget
  * @return A text string containing all information to restore the internal state of the widget/view.
  */
  virtual std::string getConfiguration(QWidget& widget) const {return "";}

  /**
  * The function is called to restore the configuration of the widget/view.
  * @param widget The widget
  * @param A text string containing all information to restore the internal state of the widget/view.
  */
  virtual void setConfiguration(QWidget& widget, const std::string& configuration) {}
};

#endif
