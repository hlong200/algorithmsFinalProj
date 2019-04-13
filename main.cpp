#include <iostream>

#include "SDL_Plotter.h"
#include <math.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>

using namespace std;

struct Color {
    int r, g, b;
    Color() { r = g = b = 0; }
    Color(int r, int g, int b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
};

const Color BLACK(0, 0, 0);
const Color RED(255, 0, 0);
const Color GREEN(0, 255, 0);
const Color BLUE(0, 0, 255);

struct Point {
    int x, y;
    Color color;
    Point() { x = y = 0; }
    Point(int x, int y, const Color& color = BLACK) {
        this->x = x;
        this->y = y;
        this->color = color;
    }
    double dist(const Point& other) const {
        return sqrt(pow(x - other.x, 2.0) + pow(y - other.y, 2.0));
    }
    void draw(SDL_Plotter& g) {
        if(x >= 0 && x < g.getCol() && y >= 0 && y < g.getRow()) {
            g.plotPixel(x, y, color.r, color.g, color.b);
        }
    }
    void erase(SDL_Plotter& g) {
        if(x >= 0 && x < g.getCol() && y >= 0 && y < g.getRow()) {
            g.plotPixel(x, y, 255, 255, 255);
        }
    }
};

struct Line {
    Point p1, p2;
    Color color;
    Line() { };
    Line(int x1, int y1, int x2, int y2, const Color& color = BLACK) {
        p1.x = x1;
        p1.y = y1;
        p2.x = x2;
        p2.y = y2;
        this->color = color;
    }
    Line(const Point& p1, const Point& p2, const Color& color = BLACK) :
         Line(p1.x, p1.y, p2.x, p2.y, color) { };

    void draw(SDL_Plotter& g) {
        int dx, dy, dx1, dy1, px, py, xe, ye, i;
        dx = p2.x - p1.x;
        dy = p2.y - p1.y;
        dx1 = fabs(dx);
        dy1 = fabs(dy);
        px = 2 * dy1 - dx1;
        py = 2 * dx1 - dy1;

        Point p;
        p.color = color;

        if(dy1 <= dx1) {
            if(dx >= 0) {
                p.x = p1.x;
                p.y = p1.y;
                xe = p2.x;
            } else {
                p.x = p2.x;
                p.y = p2.y;
                xe = p1.x;
            }
            p.draw(g);
            for(i = 0; p.x < xe; i++) {
                p.x = p.x + 1;
                if(px < 0) {
                    px += 2 * dy1;
                } else {
                    if((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                        p.y = p.y + 1;
                    } else {
                        p.y = p.y - 1;
                    }
                    px += 2 * (dy1 - dx1);
                }
                p.draw(g);
            }
        } else {
            if(dy >= 0) {
                p.x = p1.x;
                p.y = p1.y;
                ye = p2.y;
            } else {
                p.x = p2.x;
                p.y = p2.y;
                ye = p1.y;
            }
            p.draw(g);
            for(i = 0; p.y < ye; i++) {
                p.y = p.y + 1;
                if(py <= 0) {
                    py += 2 * dx1;
                } else {
                    if((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                        p.x = p.x + 1;
                    } else {
                        p.x = p.x - 1;
                    }
                    py += 2 * (dx1 - dy1);
                }
                p.draw(g);
            }
        }
    }
    void erase(SDL_Plotter& g) {
        Color old = color;
        color = Color(255, 255, 255);
        draw(g);
        color = old;
    }
};

struct Rectangle {
    Point p1, p2;
    Color color;
    Rectangle() { }
    Rectangle(const Point& p1, const Point& p2, const Color& color = BLACK) {
        this->p1 = p1;
        this->p2 = p2;
        this->color = color;
    }
    void draw(SDL_Plotter& g) {
        for(int x = p1.x; x <= p2.x; x++) {
            for(int y = p1.y; y <= p2.y; y++) {
                Point p(x, y, color);
                p.draw(g);
            }
        }
    }
    void erase(SDL_Plotter& g) {
        Color old = color;
        color = Color(255, 255, 255);
        draw(g);
        color = old;
    }
};

struct DistComp {
    bool operator() (const pair<Point, Point>& p1, const pair<Point, Point>& p2) {
        return p1.first.dist(p1.second) > p2.first.dist(p2.second);
    }
};

pair<Point, Point> closestPairBrute(const vector<Point>& points, SDL_Plotter& g) {
    pair<Point, Point> min = make_pair(Point(numeric_limits<int>::max(), numeric_limits<int>::max()), Point());
    Rectangle r;
    int count = 0;
    for(int i = 0; i < points.size(); i++) {
        for(int j = i + 1; j < points.size(); j++) {
            count++;
            Line l;
            r.erase(g);
            r = Rectangle(Point(0, 0), Point(int(g.getCol() * 1.0 / (points.size() * points.size() - count)), 10), BLUE);
            r.draw(g);
            l.erase(g);
            if(i != j) {
                if(min.first.dist(min.second) > points.at(i).dist(points.at(j))) {
                    min = make_pair(points.at(i), points.at(j));
                }
                g.update();
                l.p1.draw(g);
                l.p2.draw(g);
                l = Line(min.first, min.second, RED);
                l.draw(g);
                g.update();
                //g.Sleep(10);
            }
        }
    }
    cout << "DONE!" << endl;
    return min;
}

bool convexTest(Point p1, Point p2, Point p3) {
    return (p2.y - p1.y) * (p3.x - p2.x) - (p2.x - p1.x) * (p3.y - p2.y) > 2;
}

vector<Point> convexHullBrute(const vector<Point>& points, SDL_Plotter& g) {
    vector<Point> result;
    if(points.size() >= 3) {
        int index = 0;
        for(int i = 0; i < points.size(); i++) {
            if(points.at(i).x < points.at(index).x) {
                index = i;
            }
        }

        int x = index, y;
        Line l;
        do {
            result.push_back(points.at(x));

            y = (x + 1) % points.size();

            for(int i = 0; i < points.size(); i++) {
                if(convexTest(points.at(x), points.at(i), points.at(y))) {
                    y = i;
                    l = Line(points.at(x), points.at(y), GREEN);
                    l.draw(g);
                    g.update();
                }
            }

            x = y;
        } while(x != index);
    }

    for(int i = 0; i < result.size(); i++) {
        Line l(result.at(i), result.at((i + 1) % result.size()), RED);
        l.draw(g);
    }
    cout << "DONE!" << endl;
    return result;
}

int main(int argc, char ** argv)
{

    SDL_Plotter g(1000,1000);
    bool stopped = false;
    bool colored = false;
    int x = -1,y = -1, xd, yd;
    Color color;
    Point p;

    vector<Point> points;

    while (!g.getQuit()) {

        if(g.kbhit()){
            switch(g.getKey()) {
                case 'C':
                    g.clear();
                    cout << "Starting brute force convex hull..." << endl;
                    points.clear();
                    for(int i = 0; i < 1000; i++) {
                        points.push_back(Point(rand() % g.getCol(), rand() % g.getRow()));
                        points.at(i).draw(g);
                    }
                    g.update();
                    convexHullBrute(points, g);
                    break;
                case 'P':
                    g.clear();
                    cout << "Starting brute force closest pair..." << endl;
                    points.clear();
                    for(int i = 0; i < 1000; i++) {
                        points.push_back(Point(rand() % g.getCol(), rand() % g.getRow()));
                        points.at(i).draw(g);
                    }
                    g.update();
                    closestPairBrute(points, g);
                    break;
            }

        }

        if(g.getMouseClick(x,y)){
            stopped = !stopped;
        }

        g.update();
    }
}