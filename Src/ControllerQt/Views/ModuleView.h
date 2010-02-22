/**
* @file ControllerQt/ModuleView.h
* Declaration of a class to represent a view displaying the module layout of the process.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef ModuleView_H
#define ModuleView_H

#include <SimRobotCore/Controller/DirectView.h>

class RobotConsole;
class QPixmap;

/**
* @class ModuleView 
* A class to represent a view displaying the module layout of the process.
*/
class ModuleView : public DirectView
{
public:
  /**
  * Constructor.
  * @param console The console object.
  * @param processIdentifier The identifier of the process the modules of which are displayed.
  * @param category The category of the modules of this view. If "", show all categories.
  */
  ModuleView(RobotConsole& console, char processIdentifier, const std::string& category);

  /**
  * Destructor.
  */
  ~ModuleView();

private:
  RobotConsole& console; /**< A reference to the console object. */
  char processIdentifier; /**< The name of the view. */
  std::string category; /**< The category of the modules of this view. If "", show all categories. */
  QPixmap* image; /**< The module graph image. */
  unsigned int lastModulInfoTimeStamp; /**< Module Info timestamp when the image was created. */

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
  * The method creates the dot file of the module graph.
  * @param fileName The name of the file created.
  * @return Were any nodes generated?
  */
  bool generateDotFile(const char* fileName);

  /**
  * The method replaces all ' ' by '_'.
  * @param s The input string.
  * @return The string in which spaces were replaced.
  */
  std::string compress(const std::string& s) const;

  /**
  * The method creates the image of the module graph.
  */
  void generateImage();

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

#endif //ModuleView_H
