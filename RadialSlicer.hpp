////////////////////////////////////////////////////////////////////////////////
//Description:  Slice an axisymmetric model in radial direction and combine
//all the images into one.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <vector>
#include <array>
#include <memory>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>

using Array2D = std::array<double,2>;
using Array3D = std::array<double,3>;
using Array3I = std::array<int,3>;

struct BoundingBox
{
    Array2D minCorner;
    Array2D maxCorner;
};

struct Grid
{
    std::vector<Array3D> points;
    std::vector<Array3I> triangles;
};

using GridPtr = std::shared_ptr<Grid>;

class RadialSlicer
{
public:
    void setGrid( const std::string &fname) { 
         filename = fname; 
	 initialized = false;
    }

    GridPtr getSlice(double angle);
    void saveAs( const GridPtr &grid, const std::string &fname);
private:
    BoundingBox box;
    std::string filename;
    bool  initialized = false;
    Array3D translate;

    GridPtr genCube( const Array2D &left, const Array2D &right, double h);
    GridPtr readMesh( const std::string &fname);
    void xRotate( const GridPtr &grid, double angle);
    GridPtr extractFrontPlane(const GridPtr &g);
    GridPtr extractBoundary(const GridPtr &g);
    bool hasHole(const GridPtr &g);
    void initBox();
};

