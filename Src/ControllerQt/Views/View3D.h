/**
* @file ControllerQt/Views/View3D.h
*
* Declaration of class View3D
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef View3D_H
#define View3D_H

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Vector3.h>

#include "SimRobotCore/Controller/DirectView.h"

/**
* @class View3D 
* 
* The base class for all 3-D views.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/
class View3D : public DirectView
{
public:
  /**
  * Constructor.
  * @param background The background color.
  */
  View3D(const Vector3<float>& background);

protected:
  enum
  {
    cubeId = 1, /**< The id of the cube display list. */
    colorsId /**< The id of the color table display list. */
  };
  
  /** 
  * Update the display lists if required. 
  */
  virtual void updateDisplayLists() = 0;

  /** 
  * Need the display lists to be updated?
  * @return Yes or no? 
  */
  virtual bool needsUpdate() const {return true;}

  /**
  * The function returns the view distance.
  * @return The distance from which the scene is viewed.
  */
  virtual double getViewDistance() const {return 8.0;}
  
  const Vector3<float>& background; /**< The background color. */
  Vector3<float> lastBackground; /**< The last background color used. */
  Vector2<double> rotation; /**< The rotation of the view around two axes. */

private:
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

  /** 
  * Needs the view to be repainted?
  * @return Yes or no? 
  */
  bool needsRepaint() const;

  friend class View3DWidget;
};

#endif //View3D_H
