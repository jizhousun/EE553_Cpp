//
// Created by Jizhou.Sun on 2017/11/15.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
using namespace std;

class Point
{
public:
    double x,y,z;
    Point(double x, double y, double z):x(x),y(y),z(z){}

    void movePoint(double size, double mx,double my,double mz)
    {
        if(size!=0 && size!=1) {
            x *= size;
            y *= size;
            z *= size;
        }
        x += mx;
        y += my;
        z += mz;
    }
};

class Vector
{
public:
    double x,y,z;
    Vector(double x, double y, double z):x(x),y(y),z(z){}
};


class Facet
{
public:
    Vector vNormal;
    Point p1,p2,p3;
    Facet(Point p1, Point p2, Point p3):p1(p1),p2(p2),p3(p3),vNormal(calculateNormal(p1,p2,p3)) {    }

    Vector calculateNormal(const Point& p1, const Point& p2, const Point& p3)
    {
        double a = ( (p2.y-p1.y)*(p3.z-p1.z)-(p2.z-p1.z)*(p3.y-p1.y) );
        double b = ( (p2.z-p1.z)*(p3.x-p1.x)-(p2.x-p1.x)*(p3.z-p1.z) );
        double c = ( (p2.x-p1.x)*(p3.y-p1.y)-(p2.y-p1.y)*(p3.x-p1.x) );
        return Vector(a,b,c);
    }

};

class Shape {
public:
    Point pInit;
    vector<Facet*> facets;

    Shape(Point pInit):pInit(pInit) {}
    ~Shape() {
        for (int i = 0; i < facets.size(); i++) {
            delete facets[i];
        }
    }

    void addFacets(const Point& p1, const Point& p2, const Point& p3)
    {
        addFacets(new Facet(p1,p2,p3));
    }

    void addFacets(Facet* pFacetN)
    {
        facets.push_back(pFacetN);
    }

    void produceSTL(string strPath) const
    {
        if(facets.empty())
        { return;}
        ofstream ofs;
        //append contents
        ofs.open(strPath,ios::app);
        if (ofs.is_open())
        {
            for (int i = 0; i < facets.size(); i++) {
                ofs << "facet normal "<<facets[i]->vNormal.x<<" "<<facets[i]->vNormal.y<<" "<<facets[i]->vNormal.z<<"\n";
                ofs << "  outer loop\n";
                ofs << "    vertex "<<facets[i]->p1.x<<" "<<facets[i]->p1.y<<" "<<facets[i]->p1.z<<"\n";
                ofs << "    vertex "<<facets[i]->p2.x<<" "<<facets[i]->p2.y<<" "<<facets[i]->p2.z<<"\n";
                ofs << "    vertex "<<facets[i]->p3.x<<" "<<facets[i]->p3.y<<" "<<facets[i]->p3.z<<"\n";
                ofs << "  endloop\n";
                ofs << "endfacet\n";
            }
            ofs << "\n";
            ofs.close();
        }
    }
};

class Cylinder : public Shape
{
private:
    double r, h;
    int facetsN;
    double pi=3.1415926;
public:
    Cylinder(Point pInit, double r, double h, int facetsN):Shape(pInit),r(r),h(h),facetsN(facetsN)
    {
        generateFacets();
    }
    Cylinder(double x,double y, double z, double r, double h, int facetsN):Shape(Point(x,y,z)),r(r),h(h),facetsN(facetsN)
    {
        generateFacets();
    }

    void generateFacets()
    {
        //bot
        //center 0 0 0
        //x1 = r*cos(360/facetsN*pi/180)
        //y1 = r*sin(ao*3.14/180)
        //points on bot circle
        for (int i = 0; i < facetsN; i++) {
            double a=2.0*i/facetsN*pi;
            Point pAB=Point(r*cos(a),r*sin(a),0);
            Point pAT=Point(r*cos(a),r*sin(a),h);
            double b=2.0*((i+1)%facetsN)/facetsN*pi;
            Point pBB=Point(r*cos(b),r*sin(b),0);
            Point pBT=Point(r*cos(b),r*sin(b),h);

            //bot and top circle
            //addFacets(Point(0,0,0),pAB,pBB);
            addFacets(Point(0,0,0),pBB,pAB);
            addFacets(Point(0,0,h),pAT,pBT);

            //the face between pAB,pBB,pAT,pBT
            addFacets(pAB,pBB,pAT);
            addFacets(pBB,pBT,pAT);
        }

        for (int i = 0; i < facets.size(); i++) {
            facets[i]->p1.movePoint(1,pInit.x,pInit.y,pInit.z);
            facets[i]->p2.movePoint(1,pInit.x,pInit.y,pInit.z);
            facets[i]->p3.movePoint(1,pInit.x,pInit.y,pInit.z);
        }
    }
};

class Cube: public Shape
{
private:
    double size;
public:
    Cube(Point pInit, double size=1) :Shape(pInit), size(size)
    {
        generateFacets();
    }

    Cube(double x,double y, double z, double size=1):Shape(Point(x,y,z)),size(size)
    {
        generateFacets();
    }

    void generateFacets()
    {
        addFacets(Point(0,1,1),Point(1,0,1),Point(1,1,1));
        addFacets(Point(1,0,1),Point(0,1,1),Point(0,0,1));

        addFacets(Point(0,0,0),Point(1,1,0),Point(1,0,0));
        addFacets(Point(1,1,0),Point(0,0,0),Point(0,1,0));

        addFacets(Point(0,0,0),Point(1,0,1),Point(0,0,1));
        addFacets(Point(1,0,1),Point(0,0,0),Point(1,0,0));

        addFacets(Point(1,0,1),Point(1,1,0),Point(1,1,1));
        addFacets(Point(1,1,0),Point(1,0,1),Point(1,0,0));


        addFacets(Point(1,1,0),Point(0,1,1),Point(1,1,1));
        addFacets(Point(0,1,1),Point(1,1,0),Point(0,1,0));

        addFacets(Point(0,0,0),Point(0,1,1),Point(0,1,0));
        addFacets(Point(0,1,1),Point(0,0,0),Point(0,0,1));

        for (int i = 0; i < facets.size(); i++) {
            facets[i]->p1.movePoint(size,pInit.x,pInit.y,pInit.z);
            facets[i]->p2.movePoint(size,pInit.x,pInit.y,pInit.z);
            facets[i]->p3.movePoint(size,pInit.x,pInit.y,pInit.z);
        }
    }


};

class CAD {
private:
    vector<Shape*> shapes;
public:
    CAD(){}
    ~CAD()
    {
        for (int i = 0; i < shapes.size(); i++) {
            delete shapes[i];
        }
    }
    void add(Shape* pShapeN)
    {
        shapes.push_back(pShapeN);
    }

    void write(string strFilename)
    {
        string strPath="D://"+strFilename;
        generateSTLFile(strPath);
    }
    void generateSTLFile(string strPath)
    {
        //add a hw.stl file
        ofstream ofs;
        ofs.open(strPath);
        if(ofs.is_open())
        {
            ofs << "solid OpenSCAD_Model\n";
            ofs.close();
        }
        else
        {
            cout<<"error-writing stl file-001"<< endl;
        }
        //add shapes' facets
        for (int i = 0; i < shapes.size(); i++) {
            shapes[i]->produceSTL(strPath);
        }
        //append contents
        ofs.open(strPath,ios::app);
        if(ofs.is_open())
        {
            ofs << "endsolid OpenSCAD_Model\n";
            ofs.close();
        }
        else
        {
            cout<<"error-writing stl file-003"<< endl;
        }
    }

};

//https://www.stratasysdirect.com/resources/how-to-prepare-stl-files/
//https://www.viewstl.com/
int main() {
    CAD c;
    c.add(new Cube(0,0,0,  5));
    //c.add(new Cube(5,5,0,  7));
    c.add(new Cylinder(100,0,0,    3, 10, 10));
    c.write("test.stl");
    return 0;
}
