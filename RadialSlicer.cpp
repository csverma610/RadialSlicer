#include "RadialSlicer.hpp"
#include <iostream>
#include <limits>
#include <map>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

GridPtr RadialSlicer :: genCube( const Array2D &lowerLeft, const Array2D &upperRight, double h)
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

void RadialSlicer:: xRotate( const GridPtr &grid, double angle)
{
    double costheta = cos(M_PI*angle/180.0);
    double sintheta = sin(M_PI*angle/180.0);
    for( size_t i = 0; i < grid->points.size(); i++) {
        auto xyz = grid->points[i];
        double y = xyz[1]*costheta - xyz[2]*sintheta;
        double z = xyz[1]*sintheta + xyz[2]*costheta;
        xyz[1]   = y;
        xyz[2]   = z;
        grid->points[i] = xyz;
    }
}


///////////////////////////////////////////////////////////////////////////////

GridPtr RadialSlicer :: readMesh( const string &fname)
{
    ifstream ifile(fname.c_str(), ios::in);

    if( ifile.fail()) return nullptr;

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

void RadialSlicer :: initBox()
{
    if( filename.empty() ) return;

    auto grid = readMesh(filename);

    double xmin =  std::numeric_limits<double>::max();
    double ymin =  std::numeric_limits<double>::max();
    double zmin =  std::numeric_limits<double>::max();
    double xmax = -std::numeric_limits<double>::max();
    double ymax = -std::numeric_limits<double>::max();
    double zmax = -std::numeric_limits<double>::max();

    for( auto xyz : grid->points) {
        xmin = min( xmin, xyz[0] );
        ymin = min( ymin, xyz[1] );
        zmin = min( zmin, xyz[2] );

        xmax = max( xmax, xyz[0] );
        ymax = max( ymax, xyz[1] );
        zmax = max( zmax, xyz[2] );
    }

    double xlen = xmax-xlen;
    double ylen = ymax-ylen;

    xmin -= 0.01*xlen;
    ymin -= 0.01*ylen;

    xmax += 0.01*xlen;
    ymax += 0.01*ylen;

    box.minCorner = {xmin, ymin};
    box.maxCorner = {xmax, ymax};

    initialized = true;

    translate[0] = 0.5*(xmin+xmax);
    translate[1] = 0.5*(ymin+ymax);
    translate[2] = 0.5*(zmin+zmax);
    for( size_t i = 0; i < grid->points.size(); i++) {
         grid->points[i][0] -= translate[0];
         grid->points[i][1] -= translate[1];
         grid->points[i][2] -= translate[2];
    }

    cout << "BoxCenter : " << 0.5*(xmin+xmax) << " " << 0.5*(ymin+ymax) << " " << 0.5*(zmin+zmax) << endl;
}

///////////////////////////////////////////////////////////////////////////////
GridPtr RadialSlicer :: extractFrontPlane(const GridPtr &inGrid)
{
    auto outGrid = std::make_shared<Grid>();

    int nNodes = inGrid->points.size();

    vector<int>  active(nNodes);

    std::map<int,int> newID;

    int index = 0;
    for( int i = 0; i < nNodes; i++) {
        active[i] = 0;
	auto xyz  = inGrid->points[i];
        if( xyz[2] > 0.0) {
            xyz[2] = 0.0;
            outGrid->points.push_back(xyz);
            newID[i]  = index++;
            active[i] = 1;
        }
    }

    int nTris = inGrid->triangles.size();

    for( int i = 0; i < nTris; i++) {
        int v0 = inGrid->triangles[i][0];
        int v1 = inGrid->triangles[i][1];
        int v2 = inGrid->triangles[i][2];

        if( active[v0] + active[v1] + active[v2] == 3 ) {
            v0 = newID[v0];
            v1 = newID[v1];
            v2 = newID[v2];
            outGrid->triangles.push_back({v0,v1,v2});
        }
    }
    return outGrid;
}

GridPtr RadialSlicer :: getSlice(double angle)
{
    if( !initialized ) initBox();

    if( !initialized ) return nullptr;

    // Generate a thin bounding box
    auto cube =  genCube( box.minCorner, box.maxCorner, 0.001);
    xRotate( cube, angle);
    saveAs( cube, "cube.off");

    // Execute "Cork" binary command
    string cmd = "cork -isct " + filename + " cube.off rot.off";
    system(cmd.c_str());

    // Read the intersecting mesh..
    auto slice = readMesh("rot.off");
    if( slice == nullptr) return nullptr;

    // Rotate back the intersecting mesh to the origin ...
    xRotate( slice, -1.0*angle);

    slice = extractFrontPlane(slice);


    return slice;
}

///////////////////////////////////////////////////////////////////////////////

void RadialSlicer :: saveAs( const GridPtr &grid, const string &fname)
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

    RadialSlicer rslicer;
    rslicer.setGrid( argv[1] );

    for( int i = 0; i < 360; i++) {
        cout << "Angle " << i << endl;
        auto slice = rslicer.getSlice(i);
        string outfile = "rot" + to_string(i) + ".off";
        rslicer.saveAs( slice, outfile);
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

