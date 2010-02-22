/**
* @file ControllerQt/Views/ImageView.h
*
* Declaration of class ImageView
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author Colin Graf
*/

#ifndef __IMAGEVIEW_H__
#define __IMAGEVIEW_H__

#include <SimRobotCore/Controller/DirectView.h>

class RobotConsole;

/**
* @class ImageView 
* 
* A class to represent a view displaying camera images and overlaid debug drawings.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/
class ImageView : public DirectView
{

public:
  /**
  * Constructor.
  * @param console The console object.
  * @param background The name of the background image.
  * @param name The name of the view.
  * @param segmented The image will be segmented.
  */
  ImageView(RobotConsole& console, const std::string& background, const std::string& name, bool segmented);

private:
  RobotConsole& console; /**< A reference to the console object. */
  const std::string background; /**< The name of the background image. */
  const std::string name; /**< The name of the view. */
  bool segmented;  /**< The image will be segmented. */

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

#endif //__IMAGEVIEW_H__
