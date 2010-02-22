#ifndef _CONTROLLER_3D_DRAWING_H_
#define _CONTROLLER_3D_DRAWING_H_


/**
 * This is an abstract base class for drawings, which can be implemented
 * inside the controller and executed while drawing the scene.
 */
class Controller3DDrawing
{
public:
  /** Empty virtual destructor */
  virtual ~Controller3DDrawing() {};

  /** Virtual function for drawing commands. Derived classes have to
      override this function.*/
  virtual void draw() = 0;
};


#endif
