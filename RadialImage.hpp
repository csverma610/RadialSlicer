#pragma once

#include <QGLViewer/qglviewer.h>
#include <QtWidgets>
#include <QString>
#include <QGLViewer/qglviewer.h>
#include <qapplication.h>

#include <stdio.h>
#include <vector>
#include <array>
#include <memory>
#include <math.h>

using namespace std;

using Array2D = std::array<double,2>;
using Array3D = std::array<double,3>;
using Array3I = std::array<int,3>;

struct BoundingBox
{
   Array3D minCorner;
   Array3D maxCorner;
};

struct Grid
{
    std::vector<Array3D> points;
    std::vector<Array3I> triangles;
};

using GridPtr = std::shared_ptr<Grid>;

class RadialImage : public QGLViewer
{
public:
    RadialImage(QWidget *parent = nullptr);

protected:
    virtual void draw();
    virtual void init();
    virtual void keyPressEvent( QKeyEvent *e);
    virtual void mousePressEvent( QMouseEvent *e);
    virtual void mouseReleaseEvent( QMouseEvent *e);
    virtual void mouseMoveEvent( QMouseEvent *e);
    virtual void wheelEvent( QWheelEvent *e);
private:
    Array2D getPixelPosition( const Array2I &pixelPos) const;
};
//////////////////////////////////////////////////////////////////////
