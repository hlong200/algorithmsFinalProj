/*
 * Author: Hunter Long
 * Course: CSI 3344
 * Assignment: Group Project - Spring 2019
 */

#include <iostream>

#include "SDL_Plotter.h"
#include <math.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <ctime>
#include <random>

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

struct Circle;

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
    void drawBig(SDL_Plotter& g);
    void erase(SDL_Plotter& g) {
        if(x >= 0 && x < g.getCol() && y >= 0 && y < g.getRow()) {
            g.plotPixel(x, y, 255, 255, 255);
        }
    }
};

ostream& operator<<(ostream& out, const Point& point) {
    out << "(" << point.x << ", " << point.y << ")";
}

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
    Rectangle(const Point& p1, const Point& p2, const Color& color = Color()) : p1(p1), p2(p2), color(color) { }
    Rectangle(int x1, int y1, int x2, int y2, const Color& color = Color()) :
        Rectangle(Point(x1, y1), Point(x2, y2), color){ }

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

struct Circle {
    Point p;
    int r;
    Color color;
    Circle(Point p, int r = 0, Color color = Color()) : p(p), r(r), color(color) {}
    void draw(SDL_Plotter& g) {
        for(int x = -r; x < r; x++) {
            for(int y = -r; y < r; y++) {
                Point end(p.x + x, p.y + y, color);
                if(p.dist(end) <= r) {
                    end.draw(g);
                }
            }
        }
    }
};

void Point::drawBig(SDL_Plotter &g) {
    Circle c(Point(x, y), 3, color);
    c.draw(g);
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
            if(result.size() > 1) {
                Line l(result.at(result.size() - 1), result.at(result.size() - 2), RED);
                l.draw(g);
                g.update();
                g.Sleep(100);
            }

            y = (x + 1) % points.size();

            for(int i = 0; i < points.size(); i++) {
                if(convexTest(points.at(x), points.at(i), points.at(y))) {
                    y = i;
                }
            }

            x = y;
        } while(x != index);
    }

    if(result.size() > 1) {
        Line l(result.at(result.size() - 1), result.at(0), RED);
        l.draw(g);
    }

    cout << "DONE!" << endl;
    return result;
}

enum Pattern { RANDOM };

void genPoints(vector<Point>& points, const Point& p1, const Point& p2, Pattern mode = RANDOM, int sampleSize = 1000) {
    points.clear();
    switch(mode) {
        case RANDOM:
            random_device rd;
            mt19937_64 mt(rd());
            uniform_int_distribution<int> distX(min(p1.x, p2.x), max(p1.x, p2.x));
            uniform_int_distribution<int> distY(min(p1.y, p2.y), max(p1.y, p2.y));

            for(int i = 0; i < sampleSize; i++) {
                points.push_back(Point(distX(mt), distY(mt)));
            }
            break;
    }
}

void drawPoints(const vector<Point>& points, SDL_Plotter& g) {
    g.clear();
    for(Point p: points) {
        p.drawBig(g);
    }
    g.update();
}

int main(int argc, char ** argv)
{

    SDL_Plotter g(1080,1920);
    bool stopped = false;
    bool colored = false;
    int x = 0,y = 0, curX = 0, curY = 0;
    Color color;
    Point p;

    vector<Point> points;
    Pattern m;
    int sampleSize = 1000;
    string input;

    bool drawing = false;

    while (!g.getQuit()) {

        if(g.kbhit()) {
            /*if(g.getKey() == 'C') {
                points.clear();
                g.clear();
                g.update();
            }
            else {
                genPoints(points, g.getCol(), g.getRow());
                g.clear();
                for (Point p : points) { p.draw(g); }
                g.update();
            }*/

            // Visualizations
            switch(g.getKey()) {
                case 'R': {
                    cout << "Clearing screen..." << endl;
                    points.clear();
                    g.clear();
                    g.update();
                    break;
                }

                case '1': {
                    cout << "Starting brute force convex hull..." << endl;
                    drawPoints(points, g);
                    convexHullBrute(points, g);
                    g.update();
                    break;
                }

                case '2': {
                    cout << "Starting brute force closest pair..." << endl;
                    // TODO: Call brute force closest pair
                    break;
                }

                case '3': {
                    cout << "Starting divide and conquer convex hull..." << endl;
                    // TODO: Call divide and conquer convex hull
                    break;
                }

                case '4': {
                    cout << "Starting divide and conquer closest pair..." << endl;
                    // TODO: Call divide and conquer closest pair
                    break;
                }

                case 'G': {
                    cout << "Enter sample size followed by the down arrow" << endl;
                    input = string();
                    while (g.getKey() != DOWN_ARROW) {
                        if (g.kbhit()) {
                            if (g.getKey() == LEFT_ARROW) {
                                input.pop_back();
                            } else {
                                input.push_back(g.getKey());
                            }

                            cout << "Input: " << input << endl;
                        }
                    }

                    cout << "Select endpoints..." << endl;
                    int count = 0;
                    Point p1, p2;
                    while(count < 2) {
                        if(g.getMouseClick(x, y)) {
                            if(count == 0) {
                                p1 = Point(x, y);
                                cout << p1 << " --> ";
                            } else {
                                p2 = Point(x, y);
                                cout << p2 << endl;
                            }
                            count++;
                        }
                    }

                    cout << "Generating random points..." << endl;
                    genPoints(points, p1, p2, RANDOM, atoi(input.c_str()));
                    drawPoints(points, g);
                    break;
                }

                case ' ': {
                    g.getMouseLocation(curX, curY);
                    Rectangle r(x, y, curX, curY, GREEN);
                    g.clear();
                    r.draw(g);
                    g.update();
                    break;
                }

                default: {
                    x = curX;
                    y = curY;
                    //cout << "X: " << x << " Y: " << y << endl;
                    break;
                }
            }

        }

        if(g.getMouseClick(x, y)) {
            points.push_back(Point(x, y));
            points.back().drawBig(g);
            g.update();
        }

        //g.update();
    }
}