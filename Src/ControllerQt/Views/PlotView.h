/**
* @file ControllerQt/Views/PlotView.h
*
* Declaration of class PlotView
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author Colin Graf
*/

#ifndef PlotView_H
#define PlotView_H

#include <SimRobotCore/Controller/DirectView.h>

class RobotConsole;
class QWidget;
class QPointF;
class QPainter;
class PlotWidget;

/**
* @class PlotView 
* A class to represent a map with drawings sent by the robot code.
*/
class PlotView : public DirectView
{
public:
  /**
  * Constructor.
  * @param console The console object.
  * @param name The name of the view.
  * @param plotSize The number of entries in a plot.
  * @param minValue The minimum value in a plot.
  * @param maxValue The maximum value in a plot.
  */
  PlotView(RobotConsole& console, const std::string& name, 
   unsigned int plotSize, const double& minValue, const double& maxValue,
   const std::string& yUnit = "", const std::string& xUnit = "", const double& xScale = 1.);

  /**
  * Destructor.
  */
  ~PlotView();

  /**
  * Paints the current plot on a QPainter.
  * @param painter The painter to draw to.
  * @param widget The plot widget.
  */
  void paintAll(QPainter& painter, PlotWidget* widget);

  /**
  * Determines automatically a minimum and maximum value for the plot.
  */
  void determineMinMaxValue();

  bool drawUnits;
  bool drawLegend;
  bool antialiasing;

private:
  RobotConsole& console; /**< A reference to the console object. */
  const std::string name; /**< The name of the view. */
  unsigned int plotSize; /**< The number of entries in a plot. */
  double plotSizeF;
  double minValue; /**< The minimum value in a plot. */
  double maxValue; /**< The maximum value in a plot. */
  double valueLength;
  const std::string yUnit;
  const std::string xUnit;
  const double xScale;
  QPointF* points; /**< A buffer for drawing points. */
  unsigned int lastTimeStamp; /**< Timestamp of the last plot drawing. */
 

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

  /** Needs the view to be repainted?
  * @return Yes or no? 
  */
  virtual bool needsRepaint() const;

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

#endif //PlotView_H
