/** 
* @file Visualization/DebugDrawing3D.cpp
* Implementation of class DebugDrawing3D.
*
* @author Philippe Schober
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#include "../RobotConsole.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "DebugDrawing3D.h"
#include "Platform/GTAssert.h"
#include "Platform/SystemCall.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Configuration/RobotDimensions.h"


DebugDrawing3D::DebugDrawing3D()
{
  timeStamp = SystemCall::getCurrentSystemTime();
  drawn = false;
  scaleX  = scaleY  = scaleZ  = 1.0;
  rotateX = rotateY = rotateZ = 0.0;
  transX  = transY  = transZ  = 0.0;
}

const DebugDrawing3D& DebugDrawing3D::operator=(const DebugDrawing3D& other)
{
  reset();
  timeStamp = other.timeStamp;
  drawn = other.drawn;
  scaleX = other.scaleX;
  scaleY = other.scaleY;
  scaleZ = other.scaleZ;
  rotateX = other.rotateX;
  rotateY = other.rotateY;
  rotateZ = other.rotateZ;
  transX = other.transX;
  transY = other.transY;
  transZ = other.transZ;
  processIdentifier = other.processIdentifier;
  robotConsole  = other.robotConsole;
  return *this += other;
}

const DebugDrawing3D& DebugDrawing3D::operator+=(const DebugDrawing3D& other)
{
  lines.insert(lines.end(), other.lines.begin(), other.lines.end());
  dots.insert(dots.end(), other.dots.begin(), other.dots.end());
  polygons.insert(polygons.end(), other.polygons.begin(), other.polygons.end());
  quads.insert(quads.end(), other.quads.begin(), other.quads.end());
  spheres.insert(spheres.end(), other.spheres.begin(), other.spheres.end());
  cylinders.insert(cylinders.end(), other.cylinders.begin(), other.cylinders.end());
  images.insert(images.end(), other.images.begin(), other.images.end());
  return *this;
}

DebugDrawing3D::DebugDrawing3D(const DebugDrawing3D& other)
{
  *this = other;
}

DebugDrawing3D::DebugDrawing3D(const DebugDrawing3D* pDebugDrawing3D)
{
  *this = *pDebugDrawing3D;
}

DebugDrawing3D::~DebugDrawing3D()
{
}

void DebugDrawing3D::draw()
{
  SYNC_WITH(*robotConsole);

  // Convert mm to m.
  glScaled(0.001, 0.001, 0.001);

  // Custom scaling.
  glScaled(scaleX, scaleY, scaleZ);

  // Custom rotation.
  if (rotateX != 0) glRotated(toDegrees(rotateX), 1, 0, 0);
  if (rotateY != 0) glRotated(toDegrees(rotateY), 0, 1, 0);
  if (rotateZ != 0) glRotated(toDegrees(rotateZ), 0, 0, 1);

  // Custom translation.
  glTranslated(transX, transY, transZ);

  GLboolean lighting;
  glGetBooleanv(GL_LIGHTING, &lighting);
  glDisable(GL_LIGHTING);

  // Draw all quads.
  std::vector<Quad>::iterator q;
  for(q = quads.begin(); q != quads.end(); ++q)
  {
    glColor4ub(q->color.r, q->color.g, q->color.b, q->color.a);
    glBegin(GL_QUADS);
    glVertex3d(q->points[0].x, q->points[0].y, q->points[0].z);
    glVertex3d(q->points[1].x, q->points[1].y, q->points[1].z);
    glVertex3d(q->points[2].x, q->points[2].y, q->points[2].z);      
    glVertex3d(q->points[3].x, q->points[3].y, q->points[3].z);      
    glEnd();
  }

  // Draw all polygons/triangles.
  std::vector<Polygon>::iterator p;
  for(p = polygons.begin(); p != polygons.end(); ++p)
  {
    glColor4ub(p->color.r, p->color.g, p->color.b, p->color.a);
    glBegin(GL_TRIANGLES);
    glVertex3d(p->points[0].x, p->points[0].y, p->points[0].z);
    glVertex3d(p->points[1].x, p->points[1].y, p->points[1].z);
    glVertex3d(p->points[2].x, p->points[2].y, p->points[2].z);      
    glEnd();
  }

  // Draw all lines.
  // Get line width so we can restore it later on
  float lw[1];
  glGetFloatv(GL_LINE_WIDTH, lw);

  std::vector<Line>::iterator l;
  for(l = lines.begin(); l != lines.end(); ++l)
  {
    glLineWidth(l->width);
    glColor4ub(l->color.r, l->color.g, l->color.b, l->color.a);
    glBegin(GL_LINES);
    glVertex3d(l->points[0].x, l->points[0].y, l->points[0].z);
    glVertex3d(l->points[1].x, l->points[1].y, l->points[1].z);
    glEnd();    
  }
  // Restore line width
  glLineWidth(lw[0]);

  // Draw all points.
  // Get point size so we can restore it later on
  float ps[1];
  glGetFloatv(GL_POINT_SIZE, ps);

  std::vector<Dot>::iterator d;
  for(d = dots.begin(); d != dots.end(); ++d)
  {
    // Since each point may have a different size we can't handle all
    // points in a single glBegin(GL_POINTS). 
    // ( glPointSize is not allowed in a glBegin(...). )
    glPointSize(d->width);
    glColor4ub(d->color.r, d->color.g, d->color.b, d->color.a);
    glBegin(GL_POINTS);
    glVertex3d(d->point.x,d->point.y,d->point.z);
    glEnd();
  }

  std::vector<Sphere>::iterator s;
  for(s = spheres.begin(); s != spheres.end(); ++s)
  {
    glColor4ub(s->color.r, s->color.g, s->color.b, s->color.a);
    glPushMatrix();
    glTranslated(s->point.x, s->point.y, s->point.z);
    GLUquadric* q = gluNewQuadric();
    gluSphere(q, s->radius, 16, 16);
    gluDeleteQuadric(q);
    glPopMatrix();
  }

  std::vector<Cylinder>::iterator c;
  for(c = cylinders.begin(); c != cylinders.end(); ++c)
  {
    glColor4ub(c->color.r, c->color.g, c->color.b, c->color.a);
    glPushMatrix();
    glTranslated(c->point.x, c->point.y, c->point.z);
    if(c->rotation.x != 0)
      glRotated(toDegrees(c->rotation.x), 1, 0, 0);
    if(c->rotation.y != 0)
      glRotated(toDegrees(c->rotation.y), 0, 1, 0);
    if(c->rotation.z != 0)
      glRotated(toDegrees(c->rotation.z), 0, 0, 1);
    glTranslated(0,0,-c->height / 2);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, c->radius, c->radius, c->height, 16, 1);
    glRotated(180, 0, 1, 0);
    gluDisk(q, 0, c->radius, 16, 1);
    glRotated(180, 0, 1, 0);
    glTranslated(0, 0, c->height);
    gluDisk(q, 0, c->radius, 16, 1);
    gluDeleteQuadric(q);
    glPopMatrix();
  }

  GLboolean culling;
  glGetBooleanv(GL_CULL_FACE, &culling);
  glDisable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  std::vector<Image3D>::iterator i;
  for(i = images.begin(); i != images.end(); ++i)
  {
    GLuint t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);

    int width, height;
    char* imageData = copyImage(*i->image, width, height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
      width, height,
      0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    delete [] imageData;

    glPushMatrix();
    glTranslated(i->point.x, i->point.y, i->point.z);
    if(i->rotation.x != 0)
      glRotated(toDegrees(i->rotation.x), 1, 0, 0);
    if(i->rotation.y != 0)
      glRotated(toDegrees(i->rotation.y), 0, 1, 0);
    if(i->rotation.z != 0)
      glRotated(toDegrees(i->rotation.z), 0, 0, 1);
    glBegin(GL_QUADS);
    double right = (double) i->image->cameraInfo.resolutionWidth / width;
    double top = (double) i->image->cameraInfo.resolutionHeight / height;
    glTexCoord2d(right, top); glVertex3d(0, -i->width / 2, i->height / 2);
    glTexCoord2d(0, top); glVertex3d(0, i->width / 2, i->height / 2);
    glTexCoord2d(0, 0); glVertex3d(0, i->width / 2, -i->height / 2);
    glTexCoord2d(right, 0); glVertex3d(0, -i->width / 2, -i->height / 2);
    glEnd();
    glPopMatrix();
    glDeleteTextures(1, &t);
  }
  glDisable(GL_TEXTURE_2D);

  // Restore point size
  glPointSize(ps[0]);  
  if(lighting)
    glEnable(GL_LIGHTING);
  if(culling)
    glEnable(GL_CULL_FACE);
}


void DebugDrawing3D::reset()
{
  timeStamp = SystemCall::getCurrentSystemTime();
  lines.clear();
  dots.clear();
  polygons.clear();
  quads.clear();
  spheres.clear();
  cylinders.clear();
  images.clear();
}

void DebugDrawing3D::quad(const Vector3<double>* points, float width, ColorRGBA color)
{
  Quad element;
  element.points[0] = points[0];
  element.points[1] = points[1];
  element.points[2] = points[2];
  element.points[3] = points[3];
  element.color = color;
  element.width = width;
  quads.push_back(element);
}


void DebugDrawing3D::line
(
 double xStart, double yStart, double zStart,
 double xEnd,   double yEnd,   double zEnd,
 float width,   ColorRGBA color
 )
{
  Line element;
  element.points[0] = Vector3<double>(xStart, yStart, zStart);
  element.points[1] = Vector3<double>(xEnd,   yEnd,   zEnd  );
  element.color = color;
  element.width = width;
  lines.push_back(element);
}

void DebugDrawing3D::line(Vector3<double> *points, float width, ColorRGBA color)
{
  Line element;
  element.points[0] = points[0];
  element.points[1] = points[1];
  element.color = color;
  element.width = width;
  lines.push_back(element);
}

void DebugDrawing3D::line
(
 double xStart, 
 double yStart, 
 double zStart,
 double xEnd,
 double yEnd,
 double zEnd
 )
{
  line(xStart, yStart, zStart, xEnd, yEnd, zEnd, 1, ColorRGBA(0, 0, 0));
}

void DebugDrawing3D::polygon
(
 const Vector3<double>* points,
 float width,
 ColorRGBA color
 )
{
  Polygon element;
  for(int i = 0; i < 3; i++)
    element.points[i] = points[i];
  element.width = width;
  element.color = color;
  polygons.push_back(element);
}

void DebugDrawing3D::dot
(
 Vector3<double> v,
 float  w,
 ColorRGBA color
 )
{
  Dot element;
  element.point = v;
  element.color = color;
  element.width = w;
  dots.push_back(element);
}

void DebugDrawing3D::sphere
(
 Vector3<double> v,
 double r,
 ColorRGBA color
 )
{
  Sphere element;
  element.point = v;
  element.color = color;
  element.radius = r;
  spheres.push_back(element);
}

void DebugDrawing3D::cylinder
(
 Vector3<double> v,
 Vector3<double> rot,
 double r,
 double h,
 ColorRGBA color
 )
{
  Cylinder element;
  element.point = v;
  element.rotation = rot;
  element.color = color;
  element.radius = r;
  element.height = h;
  cylinders.push_back(element);
}

void DebugDrawing3D::image
(
 Vector3<double> v,
 Vector3<double> rot,
 double w,
 double h,
 Image* i
 )
{
  Image3D element;
  element.point = v;
  element.rotation = rot;
  element.image = i;
  element.width = w;
  element.height = h;
  images.push_back(element);
}

bool DebugDrawing3D::addShapeFromQueue
(
 InMessage& message,
 Drawings3D::ShapeType shapeType,
 char identifier
 )
{          
  processIdentifier = identifier;

  switch ((Drawings3D::ShapeType)shapeType)
  {
  case Drawings3D::translate:
    {
      message.bin >> transX;
      message.bin >> transY;
      message.bin >> transZ;
    }
    break;
  case Drawings3D::scale:
    {
      message.bin >> scaleX;
      message.bin >> scaleY;
      message.bin >> scaleZ;
    }
    break;
  case Drawings3D::rotate:
    {
      message.bin >> rotateX;
      message.bin >> rotateY;
      message.bin >> rotateZ;
    }
    break;
  case Drawings3D::coordinates:
    {
      float width;
      double length;
      message.bin >> length;
      message.bin >> width;
      this->line(0,0,0, length,0,0, width, ColorRGBA(255,0,0));
      this->line(0,0,0, 0,length,0, width, ColorRGBA(0,255,0));
      this->line(0,0,0, 0,0,length, width, ColorRGBA(0,0,255));
    }
    break;
  case Drawings3D::quad:
    {
      Vector3<double> points[4];
      ColorRGBA c;
      message.bin >> points[0];
      message.bin >> points[1];
      message.bin >> points[2];
      message.bin >> points[3];
      message.bin >> c;
      this->quad(points, 1.0, c);
    }
    break;
  case Drawings3D::polygon:
    {
      Vector3<double> points[3];
      ColorRGBA c;
      message.bin >> points[0];
      message.bin >> points[1];
      message.bin >> points[2];
      message.bin >> c;
      this->polygon(points, 1.0, c);
    }
    break;
  case Drawings3D::line:
    {
      Vector3<double> points[2];
      float width;
      ColorRGBA c;
      message.bin >> points[0];
      message.bin >> points[1];
      message.bin >> width;
      message.bin >> c;
      this->line(points, width, c);
    }
    break;
  case Drawings3D::cube:
    {
      Vector3<double> points[8];
      float width;
      ColorRGBA c;
      for (int i=0; i<8; i++)
      {
        message.bin >> points[i];
      }     
      message.bin >> width;
      message.bin >> c;
      this->line(points[0].x,points[0].y,points[0].z,points[1].x,points[1].y,points[1].z,width,c); //AB
      this->line(points[0].x,points[0].y,points[0].z,points[2].x,points[2].y,points[2].z,width,c); //AC      
      this->line(points[0].x,points[0].y,points[0].z,points[4].x,points[4].y,points[4].z,width,c); //AE      
      this->line(points[1].x,points[1].y,points[1].z,points[3].x,points[3].y,points[3].z,width,c); //BD      
      this->line(points[1].x,points[1].y,points[1].z,points[5].x,points[5].y,points[5].z,width,c); //BF 
      this->line(points[2].x,points[2].y,points[2].z,points[3].x,points[3].y,points[3].z,width,c); //CD      
      this->line(points[2].x,points[2].y,points[2].z,points[6].x,points[6].y,points[6].z,width,c); //CG
      this->line(points[3].x,points[3].y,points[3].z,points[7].x,points[7].y,points[7].z,width,c); //DH
      this->line(points[4].x,points[4].y,points[4].z,points[6].x,points[6].y,points[6].z,width,c); //EG
      this->line(points[4].x,points[4].y,points[4].z,points[5].x,points[5].y,points[5].z,width,c); //EF      
      this->line(points[5].x,points[5].y,points[5].z,points[7].x,points[7].y,points[7].z,width,c); //FH
      this->line(points[6].x,points[6].y,points[6].z,points[7].x,points[7].y,points[7].z,width,c); //GH
    }
    break;
  case Drawings3D::dot:
    {
      Vector3<double> v;
      float w;
      ColorRGBA c;
      bool withLines;
      message.bin >> v;
      message.bin >> w;
      message.bin >> c;
      message.bin >> withLines;
      this->dot(v, w, c);

      if (withLines)
      {
        this->line(  0,  0,  0,   v.x,  0,  0, 1.0, c);
        this->line(  0,  0,  0,     0,v.y,  0, 1.0, c);
        this->line(  0,  0,  0,     0,  0,v.z, 1.0, c);
        this->line(v.x,  0,  0,   v.x,v.y,  0, 1.0, c);
        this->line(v.x,  0,  0,   v.x,  0,v.z, 1.0, c);
        this->line(  0,v.y,  0,     0,v.y,v.z, 1.0, c);
        this->line(v.x,v.y,  0,     0,v.y,  0, 1.0, c);
        this->line(v.x,v.y,  0,   v.x,v.y,v.z, 1.0, c);
        this->line(v.x,  0,v.z,   v.x,v.y,v.z, 1.0, c);
        this->line(v.x,  0,v.z,     0,  0,v.z, 1.0, c);
        this->line(  0,v.y,v.z,     0,  0,v.z, 1.0, c);
        this->line(  0,v.y,v.z,   v.x,v.y,v.z, 1.0, c);
      }

    }
    break;
  case Drawings3D::sphere:
    {
      Vector3<double> v;
      double r;
      ColorRGBA c;
      message.bin >> v;
      message.bin >> r;
      message.bin >> c;
      this->sphere(v, r, c);
    }
    break;
  case Drawings3D::cylinder:
    {
      Vector3<double> v,
                      rot;
      double r,
             h;
      ColorRGBA c;
      message.bin >> v;
      message.bin >> rot;
      message.bin >> r;
      message.bin >> h;
      message.bin >> c;
      this->cylinder(v, rot, r, h, c);
    }
    break;
  case Drawings3D::image:
    {
      Vector3<double> v,
                      rot;
      double w,
             h;
      Image* i = new Image;
      message.bin >> v;
      message.bin >> rot;
      message.bin >> w;
      message.bin >> h;
      message.bin >> *i;
      this->image(v, rot, w, h, i);
    }
    break;
  }
  return true;
}

char* DebugDrawing3D::copyImage(const Image& srcImage, int& width, int& height) const
{
  width = 1; 
  while(width < srcImage.cameraInfo.resolutionWidth)
    width <<= 1;
  height = 1;
  while(height < srcImage.cameraInfo.resolutionHeight)
    height <<= 1;

  static const int factor1 = 29016;
  static const int factor2 = 5662;
  static const int factor3 = 22972;
  static const int factor4 = 11706;
  char* imageData = new char[width * height * 3];
  int r, g, b;
  int yImage, uImage, vImage;
  for(int y = srcImage.cameraInfo.resolutionHeight - 1; y >= 0; y--) 
  {
    char* p = imageData + width * 3 * (srcImage.cameraInfo.resolutionHeight - 1 - y);
    const Image::Pixel* cur = &srcImage.image[y][0];
    const Image::Pixel* end = cur + srcImage.cameraInfo.resolutionWidth;
    for(; cur < end; cur++)
    {        
      yImage = int(cur->y) << 14;
      uImage = int(cur->cr) - 128;
      vImage = int(cur->cb) - 128;

      r = (yImage + factor3 * uImage) >> 14;
      g = (yImage - factor2 * vImage - factor4 * uImage) >> 14;
      b = (yImage + factor1 * vImage) >> 14;

      *p++ = r < 0 ? 0 : (r > 255 ? 255 : r);
      *p++ = g < 0 ? 0 : (g > 255 ? 255 : g);
      *p++ = b < 0 ? 0 : (b > 255 ? 255 : b);
    }
  }
  return imageData;
}
