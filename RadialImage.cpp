#include <fstream>
#include <QGLViewer/manipulatedCameraFrame.h>
#include <GL/glu.h>

#include "RadialImage.hpp"

using namespace std;
using namespace qglviewer;

using namespace std;

///////////////////////////////////////////////////////////////////////////////

RadialImage :: RadialImage(QWidget *w) : QGLViewer(w)
{
    canvas.minCorner = {-1.0, -1.0};
    canvas.maxCorner = {+1.0, +1.0};
}

///////////////////////////////////////////////////////////////////////////////
void RadialImage::init()
{
    camera()->setType(Camera::ORTHOGRAPHIC);

    AxisPlaneConstraint *worldConstraint = new WorldConstraint();
    worldConstraint->setRotationConstraintType(AxisPlaneConstraint::FORBIDDEN);
    this->camera()->frame()->setConstraint(worldConstraint);

    glDisable(GL_LIGHTING);
    setVisibleRegion();
    camera()->showEntireScene();
}

///////////////////////////////////////////////////////////////////////////////
Array2D RadialImage:: getPixelPosition( const Array2I &pixelPos) const
{
    qglviewer::Vec src = {1.0*pixelPos[0], 1.0*pixelPos[1], 0.0};
    qglviewer::Vec dst = camera()->unprojectedCoordinatesOf(src);
    Array2D xy = {dst[0], dst[1]};
    return xy;
}

///////////////////////////////////////////////////////////////////////////////
/*
void RadialImage :: loadContour()
{
    QString name = QFileDialog::getOpenFileName(this, "Select Contours file", ".",
                   "Format (*.dat *.txt)");
    if (name.isEmpty()) return;

    string filename = name.toStdString();

    ifstream ifile( filename.c_str(), ios::in);

    int np;
    int nholes;
    Array2D xy;

    VContour newC;

    ifile >> np >> nholes;
    for( int j = 0; j < np; j++) {
        ifile >> xy[0] >> xy[1];
        newC.points.push_back(xy);
    }
    newC.setBox();
    contours.push_back(newC);
    canvas.add(newC.box);
    camera()->setSceneRadius( newC.box.getRadius());

    Array2D c = canvas.getCenter();
    qglviewer::Vec pos;
    pos[0] = c[0];
    pos[1] = c[1];
    camera()->lookAt( pos );

    camera()->showEntireScene();

    int mholes;
    for( int i = 0; i < nholes; i++) {
        newC.points.clear();
        ifile >> np >> mholes;
        assert( mholes == 0);
        for( int j = 0; j < np; j++) {
            ifile >> xy[0] >> xy[1];
            newC.points.push_back(xy);
        }
        newC.setBox();
        contours.push_back(newC);
    }
    update();
}
*/

////////////////////////////////////////////////////////////////////////////////

void
RadialImage::keyPressEvent( QKeyEvent *e)
{
	/*
    if( e->key() == Qt::Key_A) {
        displayAxes = !displayAxes;
        update();
        return;
    }

    if( e->key() == Qt::Key_S) {
        saveContours();
        return;
    }

    if( e->key() == Qt::Key_O) {
        currContour.setBox();
        currContour.partition();
        contours.push_back(currContour);
        currContour.points.clear();
        pickPoints = false;
    }

    if( e->key() == Qt::Key_B) displayBoxes = !displayBoxes;
    if( e->key() == Qt::Key_C) loadContour();
    if( e->key() == Qt::Key_I) loadImage();
    if( e->key() == Qt::Key_Z) if( !currContour.points.empty() ) currContour.points.pop_back();
    if( e->key() == Qt::Key_V) {
        for( size_t i = 0; i < contours.size(); i++)
            contours[i].visible = true;
    }

    if( e->key() == Qt::Key_P) {
        pickPoints = !pickPoints;
        setSelectedName(-1);
    }

    QGLViewer::keyPressEvent(e);
    this->update();
*/
}

void RadialImage::drawContours()
{

	/*
    glLineWidth(1.0);
    glColor3f( 0.0, 1.0, 0.0);

    int selected = selectedName();

    glBegin(GL_LINES);
    for( int i = 0; i < contours.size(); i++) {
        if( contours[i].visible) {
            glColor3f( 0.0, 1.0, 0.0);
            if( i == selected ) glColor3f( 1.0, 0.0, 0.0);
            size_t nPoints = contours[i].points.size();
            for( int j = 0; j < nPoints; j++) {
                glVertex2dv( &contours[i].points[j][0] );
                glVertex2dv( &contours[i].points[(j+1)%nPoints][0]);
            }
        }
    }
    glEnd();

    if( fillContour) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        for( int i = 0; i < contours.size(); i++) {
            if( contours[i].visible) {
                glColor3f( 0.0, 1.0, 0.0);
                size_t npoly = contours[i].polygons.size();
                for( int j = 0; j < npoly; j++) {
                    int np = contours[i].polygons[j].size();
                    glBegin(GL_POLYGON);
                    for( int k = 0; k < np; k++) {
                        int id =  contours[i].polygons[j][k];
                        glVertex2dv( &contours[i].points[id][0] );
                    }
                    glEnd();
                }
            }
        }
    }

    if( displayBoxes) {
        glColor3f( 0.1, 0.1, 0.1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for( int i = 0; i < contours.size(); i++) {
            if( contours[i].visible ) drawBox( contours[i].box);
        }
    }

    if( !currContour.points.empty() ) {
        glLineWidth(2.0);
        glColor3f( 1.0, 0.0, 0.0);

        glBegin(GL_LINES);
        size_t nPoints = currContour.points.size();
        for( int j = 0; j < nPoints-1; j++) {
            glVertex2dv( &currContour.points[j][0] );
            glVertex2dv( &currContour.points[(j+1)%nPoints][0]);
        }

        glEnd();
    }

    if( displayPoints)
    {
        glPointSize(5);
        glColor3f( 1.0, 0.0, 0.0);

        glBegin(GL_POINTS);
        for( int i = 0; i < contours.size(); i++) {
            if( contours[i].visible) {
                size_t nPoints = contours[i].points.size();
                for( int j = 0; j < nPoints; j++) {
                    glVertex3d( contours[i].points[j][0], contours[i].points[j][1], 0.00001 );
                }
            }
        }
        glEnd();

        glColor3f( 1.0, 1.0, 0.0);

        glBegin(GL_POINTS);
        size_t nPoints = currContour.points.size();
        for( int j = 0; j < nPoints; j++) {
            glVertex3d( currContour.points[j][0], currContour.points[j][1], 0.00001);
        }
        glEnd();
    }
*/
}
//////////////////////////////////////////////////////////////////////////////////////

/*
void RadialImage::drawAxes()
{
    double maxval = camera()->sceneRadius();

    Array2D p1;

    glColor3f( 1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    p1 = {+maxval, 0.0}; glVertex2dv( &p1[0] );
    p1 = {-maxval, 0.0}; glVertex2dv( &p1[0] );
    glEnd();

    glColor3f( 0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    p1 = {0.0, +maxval}; glVertex2dv( &p1[0] );
    p1 = {0.0, -maxval}; glVertex2dv( &p1[0] );
    glEnd();
}
*/

//////////////////////////////////////////////////////////////////////////////////////

/*
void RadialImage::drawBox(const BoundingBox &b)
{
    glColor3f( 0.2, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2d( b.minCorner[0],  b.minCorner[1] );
    glVertex2d( b.maxCorner[0],  b.minCorner[1] );
    glVertex2d( b.maxCorner[0],  b.maxCorner[1] );
    glVertex2d( b.minCorner[0],  b.maxCorner[1] );
    glEnd();
}

//////////////////////////////////////////////////////////////////////////////////////
*/

void RadialImage::draw()
{
    glClearColor( 0.0, 0.0, 0.0, 1.0);

//  drawContours();
    if( displayAxes) drawAxes();
}

/////////////////////////////////////////////////////////////////////////////////////

/*
void RadialImage::drawWithNames()
{
    for( int i = 0; i < contours.size(); i++) {
        if( contours[i].visible) {
            glPushName(i);
            glBegin(GL_LINES);
            size_t nPoints = contours[i].points.size();
            for( int j = 0; j < nPoints; j++) {
                glVertex2dv( &contours[i].points[j][0] );
                glVertex2dv( &contours[i].points[(j+1)%nPoints][0]);
            }
            glEnd();
            glPopName();
        }
    }
}
*/

/////////////////////////////////////////////////////////////////////////////////////

void RadialImage:: mousePressEvent( QMouseEvent *e)
{
	/*
    if( pickPoints)  {
        Array2D xy = getPixelPosition( {e->x(), e->y()});
        currContour.points.push_back(xy);
        update();
    }
    QGLViewer::mousePressEvent(e);
    */
}

/////////////////////////////////////////////////////////////////////////////////////

void RadialImage:: mouseReleaseEvent( QMouseEvent *e)
{
}

/////////////////////////////////////////////////////////////////////////////////////

void RadialImage:: mouseMoveEvent( QMouseEvent *e)
{
    QGLViewer::mouseMoveEvent(e);
}
/////////////////////////////////////////////////////////////////////////////////////
void RadialImage:: setVisibleRegion()
{
    auto p0 = getPixelPosition( {0,0});
    auto p1 = getPixelPosition( {this->width(),this->height()});
    double dx = p1[0] - p0[0];
    double dy = p1[1] - p0[1];
    double r  = sqrt(dx*dx + dy*dy);

    camera()->setSceneRadius(r);
    update();
}

void RadialImage:: wheelEvent( QWheelEvent *e)
{
    setVisibleRegion();
    QGLViewer::wheelEvent(e);
}
/////////////////////////////////////////////////////////////////////////////////////

/*
void RadialImage:: saveContours()
{
    QString name = QFileDialog::getSaveFileName(this, "Select Contours file", ".",
                   "Format (*.dat *.txt)");
    if (name.isEmpty()) return;

    string filename = name.toStdString();

    ofstream ofile( filename.c_str(), ios::out);

    for( size_t i = 0; i < contours.size(); i++) {
        ofile << contours[i].points.size() << " 0 " << endl;
        for( auto p : contours[i].points)
            ofile << p[0] << " " << p[1] << endl;
    }
}
*/

/////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{

    QApplication application(argc, argv);

    RadialImage viewer;

    viewer.setWindowTitle("RadialImage");

    viewer.show();

    return application.exec();

}


