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

class RadialSlice
{
   public:
      void setGrid( const std::string &filename);
      GridPtr getSlice( double angle);

   private:
       GridPtr genPlane( const Array2D &lowerLeft, const Array2D &upperRight, int maxGrid, double h)
       GridPtr genCube( const Array2D &lowerLeft, const Array2D &upperRight, double h);
       void xRotate( const GridPtr &grid, double angle);
       GridPtr readMesh( const string &fname);
       void saveAs( const GridPtr &grid, const string &fname);
};

///////////////////////////////////////////////////////////////////////////////

GridPtr genPlane( const Array2D &lowerLeft, const Array2D &upperRight, int maxGrid, double h)
{
    auto grid = std::make_shared<Grid>();

    double xlen = upperRight[0] - lowerLeft[0];
    double ylen = upperRight[1] - lowerLeft[1];

    double len  = max(xlen, ylen);
    double dl   = len/(double)(maxGrid-1);

    int Nx   = xlen/dl+1;
    int Ny   = ylen/dl+1;

    Nx  = max(2, Nx);
    Ny  = max(2, Ny);

    double dx   = xlen/(double)(Nx-1);
    double dy   = ylen/(double)(Ny-1);

    grid->points.resize( Nx*Ny);
    int index = 0;
    for( int j = 0; j < Ny; j++) {
        for( int i = 0; i < Nx; i++) {
            double  x = lowerLeft[0] + i*dx;
            double  y = lowerLeft[1] + j*dy;
            double  z = h;
            grid->points[index++] = {x,y,z};
        }
    }

    grid->triangles.resize( 2*(Nx-1)*(Ny-1));
    index = 0;
    for( int j = 0; j < Ny-1; j++) {
        for( int i = 0; i < Nx-1; i++) {
            int n0 = j*Nx + i;
            int n1 = n0+1;
            int n2 = n0+1+Nx;
            int n3 = n0+Nx;
            grid->triangles[index++] = {n0, n1, n2};
            grid->triangles[index++] = {n0, n2, n3};
        }
    }

    return grid;
}

///////////////////////////////////////////////////////////////////////////////

GridPtr genCube( const Array2D &lowerLeft, const Array2D &upperRight, double h)
{
    auto grid = std::make_shared<Grid>();

    double x0 = lowerLeft[0];
    double y0 = lowerLeft[1];

    double x1 = upperRight[0];
    double y1 = upperRight[1];

    grid->points.resize(8);

    grid->points[0] = {x0, y0, -h};
    grid->points[1] = {x1, y0, -h};
    grid->points[2] = {x1, y1, -h};
    grid->points[3] = {x0, y1, -h};

    grid->points[4] = {x0, y0, +h};
    grid->points[5] = {x1, y0, +h};
    grid->points[6] = {x1, y1, +h};
    grid->points[7] = {x0, y1, +h};

    grid->triangles.resize(12);
    grid->triangles[0] = {0, 7, 3};
    grid->triangles[1] = {0, 4, 7};

    grid->triangles[2] = {1, 2, 6};
    grid->triangles[3] = {1, 6, 5};

    grid->triangles[4] = {0, 1, 5};
    grid->triangles[5] = {0, 5, 4};

    grid->triangles[6] = {3, 7, 6};
    grid->triangles[7] = {3, 6, 2};

    grid->triangles[8] = {0, 3, 2};
    grid->triangles[9] = {0, 2, 1};

    grid->triangles[10] = {4, 5, 6};
    grid->triangles[11] = {4, 6, 7};

    return grid;
}

///////////////////////////////////////////////////////////////////////////////

void xRotate( const GridPtr &grid, double angle)
{
    double costheta = cos(M_PI*angle/180.0);
    double sintheta = sin(M_PI*angle/180.0);
    for( size_t i = 0; i < grid->points.size(); i++) {
        auto xyz      = grid->points[i];
        double y = xyz[1]*costheta - xyz[2]*sintheta;
        double z = xyz[1]*sintheta + xyz[2]*costheta;
        xyz[1]   = y;
        xyz[2]   = z;
        grid->points[i] = xyz;
    }
}

///////////////////////////////////////////////////////////////////////////////

GridPtr readMesh( const string &fname)
{
    ifstream ifile(fname.c_str(), ios::in);

    string str;
    ifile >> str;

    int numNodes, numFaces, numEdges;
    ifile >> numNodes >> numFaces >> numEdges;

    auto grid = std::make_shared<Grid>();

    grid->points.resize(numNodes);

    double x, y, z;
    for( int i = 0; i < numNodes; i++) {
        ifile >> x >> y >> z;
        grid->points[i] = {x,y,z};
    }

    grid->triangles.resize(numFaces);

    int nd, n0, n1, n2;
    for( int i = 0; i < numFaces; i++) {
        ifile >> nd >> n0 >> n1 >> n2;
        grid->triangles[i] = {n0, n1, n2};
    }
    return grid;
}

///////////////////////////////////////////////////////////////////////////////

void saveAs( const GridPtr &grid, const string &fname)
{
    ofstream ofile(fname.c_str(), ios::out);

    ofile << "OFF" << endl;

    ofile << grid->points.size() << " " << grid->triangles.size() << " 0 " << endl;

    for( auto p :  grid->points )
        ofile << p[0] << "  " << p[1] <<  " " << p[2] << endl;

    for( auto t :  grid->triangles )
        ofile << "3 " << t[0] << "  " << t[1] <<  " " << t[2] << endl;
}

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    if( argc != 2) {
        cout << "Usages: " << argv[0] << " filename " << endl;
        cout << "filename : A geometric model (off format) whose axis of rotation os along X-direction" << endl;
        return 1;
    }

    Array2D lowerLeft   = {-1.0, -7.0};
    Array2D upperRight =  {17.0, +7.0};

    for( int i = 0; i < 360; i++) {
        double angle = i;
        cout << "Angle " << angle << endl;
        auto cube =  genCube( lowerLeft, upperRight, 0.001);
        xRotate( cube, angle);
        saveAs( cube, "cube.off");
        string outfile = "rot" + to_string(i) + ".off";
        string cmd = "cork -isct " + string(argv[1]) + " cube.off " + outfile;
        system(cmd.c_str());

        auto slice = readMesh(outfile);
        xRotate( slice, -1.0*angle);
        saveAs( slice, outfile);
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

