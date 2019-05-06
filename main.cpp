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
#include <chrono>
#include <random>
#include <thread>
#include <mutex>

using namespace std;

//mutex mx;

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
const Color ORANGE(255, 127, 0);
const Color YELLOW(255, 255, 0);
const Color GREEN(0, 255, 0);
const Color BLUE(0, 0, 255);
const Color INDIGO(75, 0, 130);
const Color VIOLET(148, 0, 211);

struct Circle;

struct Point {
    int x, y;
    Color color;
    Point() { x = y = 0; }
    Point(int x, int y, const Color& color = BLACK);
    double dist(const Point& other) const;
    void draw(SDL_Plotter& g);
    void drawBig(SDL_Plotter& g);
    void erase(SDL_Plotter& g);
    bool operator==(const Point& other);
    bool operator!=(const Point& other);
};

ostream& operator<<(ostream& out, const Point& point);

struct Line {
    Point p1, p2;
    Color color;
    Line() = default;
    Line(int x1, int y1, int x2, int y2, const Color& color = BLACK);
    Line(const Point& p1, const Point& p2, const Color& color = BLACK) :
         Line(p1.x, p1.y, p2.x, p2.y, color) { };

    void draw(SDL_Plotter& g);
    void erase(SDL_Plotter& g);
};

struct Rectangle {
    Point p1, p2;
    Color color;
    Rectangle() { }
    Rectangle(const Point& p1, const Point& p2, const Color& color = Color()) : p1(p1), p2(p2), color(color) { }
    Rectangle(int x1, int y1, int x2, int y2, const Color& color = Color()) :
        Rectangle(Point(x1, y1), Point(x2, y2), color){ }

    void draw(SDL_Plotter& g);
    void erase(SDL_Plotter& g);
};

struct Circle {
    Point p;
    int r;
    Color color;
    Circle(Point p, int r = 0, Color color = Color()) : p(p), r(r), color(color) {}
    void draw(SDL_Plotter& g);
};



bool convexTest(Point p1, Point p2, Point p3);

vector<Point> convexHullBrute(const vector<Point>& points, SDL_Plotter& g, bool visualize = true);

void drawPoints(const vector<Point>& points, SDL_Plotter& g);

pair<Point, Point> closestPairBrute(const vector<Point>& points, SDL_Plotter& g, bool visualize = true);

pair<vector<Point>,double> closestPairRecursive(const vector <Point>& pX, const vector<Point>& pY, SDL_Plotter& g, int depth, bool visualize = true);

pair<vector<Point>,double> closestPairDC(const vector<Point>& points, SDL_Plotter& g, bool visualize = true);

void genPoints(vector<Point>& points, const Point& p1, const Point& p2, int sampleSize = 1000);

void compareAlgorithms(vector<Point>& points, SDL_Plotter& g);

int main(int argc, char ** argv)
{

    SDL_Plotter g(1080,1920);
    bool stopped = false;
    bool colored = false;
    int x = 0,y = 0, curX = 0, curY = 0;
    Color color;
    Point p;

    vector<Point> points;
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
                    drawPoints(points, g);
                    pair<Point, Point> closest = closestPairBrute(points, g);
                    g.update();
                    cout << "Closest pair: " << closest.first << " --> " << closest.second << endl;
                    break;
                }

                case '3': {
                    cout << "Starting divide and conquer convex hull..." << endl;
                    // TODO: Call divide and conquer convex hull
                    break;
                }

                case '4': {
                    cout << "Starting divide and conquer closest pair..." << endl;
                    drawPoints(points,g);
                    closestPairDC(points,g);
                    g.update();
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
                    genPoints(points, p1, p2, atoi(input.c_str()));
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

                case 'X': {
                    compareAlgorithms(points, g);
                    drawPoints(points, g);
                    g.update();
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
            bool found = false;
            for(int i = 0; i < points.size() && !found; i++) {
                if (points[i].x == x && points[i].y == y) {
                    found = true;
                }
            }
            if(!found) {
                points.push_back(Point(x, y));
                points.back().drawBig(g);
                g.update();
            }
        }

        //g.update();
    }

    return 0;
}

Point::Point(int x, int y, const Color& color) {
    this->x = x;
    this->y = y;
    this->color = color;
}
double Point::dist(const Point& other) const {
    return sqrt(pow(x - other.x, 2.0) + pow(y - other.y, 2.0));
}
void Point::draw(SDL_Plotter& g) {
    //mx.lock();
    if(x >= 0 && x < g.getCol() && y >= 0 && y < g.getRow()) {
        g.plotPixel(x, y, color.r, color.g, color.b);
    }
    //mx.unlock();
}
void Point::erase(SDL_Plotter& g) {
    if(x >= 0 && x < g.getCol() && y >= 0 && y < g.getRow()) {
        g.plotPixel(x, y, 255, 255, 255);
    }
}
bool Point::operator==(const Point& other) {
    return (this == &other) || (x == other.x && y == other.y);
}
bool Point::operator!=(const Point& other) {
    return (this != &other) || (x != other.x || y != other.y);
}
void Point::drawBig(SDL_Plotter &g) {
    Circle c(Point(x, y), 3, color);
    c.draw(g);
}

ostream& operator<<(ostream& out, const Point& point) {
    out << "(" << point.x << ", " << point.y << ")";
    return out;
}

Line::Line(int x1, int y1, int x2, int y2, const Color& color) {
    p1.x = x1;
    p1.y = y1;
    p2.x = x2;
    p2.y = y2;
    this->color = color;
}

void Line::draw(SDL_Plotter& g) {
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
void Line::erase(SDL_Plotter& g) {
    Color old = color;
    color = Color(255, 255, 255);
    draw(g);
    color = old;
}

void Rectangle::draw(SDL_Plotter& g) {
    for(int x = p1.x; x <= p2.x; x++) {
        for(int y = p1.y; y <= p2.y; y++) {
            Point p(x, y, color);
            p.draw(g);
        }
    }
}
void Rectangle::erase(SDL_Plotter& g) {
    Color old = color;
    color = Color(255, 255, 255);
    draw(g);
    color = old;
}

void Circle::draw(SDL_Plotter& g) {
    for(int x = -r; x < r; x++) {
        for(int y = -r; y < r; y++) {
            Point end(p.x + x, p.y + y, color);
            if(p.dist(end) <= r) {
                end.draw(g);
            }
        }
    }
}

bool convexTest(Point p1, Point p2, Point p3) {
    return (p2.y - p1.y) * (p3.x - p2.x) - (p2.x - p1.x) * (p3.y - p2.y) > 2;
}

void drawPoints(const vector<Point>& points, SDL_Plotter& g) {
    g.clear();
    for(Point p: points) {
        p.drawBig(g);
    }
    g.update();
}

vector<Point> convexHullBrute(const vector<Point>& points, SDL_Plotter& g, bool visualize) {
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
            if(result.size() > 1 && visualize) {
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

    if(result.size() > 1 && visualize) {
        Line l(result.at(result.size() - 1), result.at(0), RED);
        l.draw(g);
    }

    //cout << "DONE!" << endl;
    return result;
}

void compareAlgorithms(vector<Point>& points, SDL_Plotter& g){
    points.clear();
    vector<Point> points2;
    Point p1(0, 0);
    Point p2(1920, 1080);

    vector<double> CPB;
    vector<double> CPDC;
    vector<double> CHB;
    vector<double> CHDC;

    int inputCount = 300;

    for(int i = 2; i < inputCount + 2; i++){
        cout << "running for input size " << i << endl;

        genPoints(points2, p1, p2, i);
        vector<Point> points3 = points2;
        vector<Point> points4 = points3;
        vector<Point> points5 = points4;

        auto begin = chrono::system_clock::now();
        closestPairBrute(points2, g, false);
        auto end = chrono::system_clock::now();
        auto elapsed = chrono::duration_cast<chrono::microseconds>(end - begin).count();
        CPB.push_back(elapsed);

        begin = chrono::system_clock::now();
        closestPairDC(points3, g, false);
        end = chrono::system_clock::now();
        elapsed = chrono::duration_cast<chrono::microseconds>(end - begin).count();
        CPDC.push_back(elapsed);

        begin = chrono::system_clock::now();
        convexHullBrute(points4, g, false);
        end = chrono::system_clock::now();
        elapsed = chrono::duration_cast<chrono::microseconds>(end - begin).count();
        CHB.push_back(elapsed);

        begin = chrono::system_clock::now();
        //Enter DC Convex Hull method here will use points5
        end = chrono::system_clock::now();
        elapsed = chrono::duration_cast<chrono::microseconds>(end - begin).count();
        CHDC.push_back(elapsed);
    }

    double maxTime = 0.0;

    for(double theTime : CPB){
        maxTime = max(maxTime, theTime);
    }

    for(double theTime : CPDC){
        maxTime = max(maxTime, theTime);
    }

    for(double theTime : CHB){
        maxTime = max(maxTime, theTime);
    }

    for(double theTime : CHDC){
        maxTime = max(maxTime, theTime);
    }

    for(int i = 0; i < inputCount; i++){
        double value = CPB[i];

        int x = (1920.0 / inputCount) * i;
        int y = (value / maxTime) * 1080;
        y = 1080 - y;

        Point toPlace(x, y, GREEN);
        points.push_back(toPlace);
    }

    for(int i = 0; i < inputCount; i++){
        double value = CPDC[i];

        int x = (1920.0 / inputCount) * i;
        int y = (value / maxTime) * 1080;
        y = 1080 - y;

        Point toPlace(x, y, BLUE);
        points.push_back(toPlace);
    }

    for(int i = 0; i < inputCount; i++){
        double value = CHB[i];

        int x = (1920.0 / inputCount) * i;
        int y = (value / maxTime) * 1080;
        y = 1080 - y;

        Point toPlace(x, y, RED);
        points.push_back(toPlace);
    }

    for(int i = 0; i < inputCount; i++){
        double value = CHDC[i];

        int x = (1920.0 / inputCount) * i;
        int y = (value / maxTime) * 1080;
        y = 1080 - y;

        Point toPlace(x, y, YELLOW);
        points.push_back(toPlace);
    }

    cout << "Plotting points for comparision..." << endl;
    cout << "Max time: " << maxTime << endl;
    cout << "Max input size: " << inputCount << endl;
    cout << "Number of points plotted: " << points.size() << endl;

    drawPoints(points, g);
    cout << "Updating screen..." << endl;
    g.update();
}

pair<Point, Point> closestPairBrute(const vector<Point>& points, SDL_Plotter& g, bool visualize) {
    if(points.size() > 2) {
        double minDist = numeric_limits<double>::max();
        pair<Point, Point> closest;

        for (int i = 0; i < points.size(); i++) {
            for (int j = 0; j < points.size() && i != j; j++) {
                Line l;
                pair<Point, Point> cur = make_pair(points.at(i), points.at(j));
                l = Line(cur.first, cur.second, INDIGO);
                if (cur.first.dist(cur.second) < minDist) {
                    closest = cur;
                    minDist = cur.first.dist(cur.second);
                    //cout << "Closer one found!" << endl;
                    l.color = RED;
                }
                if(visualize) {
                    l.draw(g);
                    g.update();
                }
            }
            if(visualize) {
                g.clear();
                drawPoints(points, g);
            }
        }

        if(visualize) {
            g.update();
            Line l(closest.first, closest.second, RED);
            l.draw(g);
        }

        return closest;
    } else if(points.size() == 2) {
        return make_pair(points.at(0), points.at(1));
    } else if(points.size() == 1) {
        return make_pair(points.at(0), Point());
    } else {
        return make_pair(Point(), Point());
    }
}

pair<vector<Point>,double> closestPairRecursive(const vector <Point>& pX, const vector<Point>& pY, SDL_Plotter& g, int depth, bool visualize) {
    int n = pX.size();
    if(n <= 3) {
        double minDist;
        vector<Point> minPoints;
        for (int i = 0; i < n - 1; i++) {
            for (int j = i + 1; j < n; j++) {
                double dist = pX[i].dist(pX[j]);
                if (minPoints.size() == 0 || dist < minDist) {
                    minPoints.clear();
                    minPoints.push_back(pX[i]);
                    minPoints.push_back(pX[j]);
                    minDist = dist;
                }
            }
        }

        return make_pair(minPoints, minDist);
    } else {
        int midIndex = n / 2;
        auto itr = pX.begin();
        vector<Point> lX(itr,itr + midIndex);
        vector<Point> rX(itr + midIndex,itr + n);

        double midX = abs(static_cast<double>(lX[lX.size() - 1].x + rX[0].x)) / 2;
        if(visualize) {
            Line l(Point((int) midX, 0), Point((int) midX, 1920), RED);
            switch (depth % 7) {
                case 0:
                    l.color = RED;
                    break;
                case 1:
                    l.color = ORANGE;
                    break;
                case 2:
                    l.color = YELLOW;
                    break;
                case 3:
                    l.color = GREEN;
                    break;
                case 4:
                    l.color = BLUE;
                    break;
                case 5:
                    l.color = INDIGO;
                    break;
                default:
                    l.color = VIOLET;
                    break;
            }

            l.draw(g);
            g.update();
        }

        vector<Point> lY = lX;
        sort(lY.begin(),lY.end(),[](const Point& p1, const Point& p2) {
            return p1.y < p2.y;
        });
        vector<Point> rY = rX;
        sort(rY.begin(),rY.end(),[](const Point& p1, const Point& p2) {
            return p1.y < p2.y;
        });

        pair<vector<Point>,double> lRes = closestPairRecursive(lX,lY,g,depth+1,visualize);
        pair<vector<Point>,double> rRes = closestPairRecursive(rX,rY,g,depth+1,visualize);

        pair<vector<Point>,double>& minRes = ((lRes.second < rRes.second) ? lRes : rRes);

        vector<Point> withinBoundary;
        for(int i = 0; i < pY.size(); i++) {
            if(abs(midX - pY[i].x) < minRes.second) {
                withinBoundary.push_back(pY[i]);
            }
        }

        int pointsInBounds = withinBoundary.size();
        if(pointsInBounds <= 1) {
            return minRes;
        } else {
            Point* p = &(withinBoundary[0]);
            Point* q = &(withinBoundary[1]);
            double pqDist = p->dist(*q);
            for(int i = 0; i < pointsInBounds; i++) {
                for(int j = 1; j <= 7 && (i+j) < pointsInBounds; j++) {
                    if(withinBoundary[i].dist(withinBoundary[i+j]) < pqDist) {
                        p = &(withinBoundary[i]);
                        q = &(withinBoundary[i+j]);
                        pqDist = p->dist(*q);
                    }
                }
            }

            if(pqDist < minRes.second) {
                vector<Point> resultVector = {*p,*q};
                return make_pair(resultVector,pqDist);
            } else {
                return minRes;
            }
        }
    }
}

pair<vector<Point>,double> closestPairDC(const vector<Point>& points, SDL_Plotter& g, bool visualize) {
    vector<Point> xSorted(points);
    vector<Point> ySorted(points);

    sort(xSorted.begin(),xSorted.end(),[](const Point& p1, const Point& p2) {
        return p1.x < p2.x;
    });

    sort(ySorted.begin(),ySorted.end(),[](const Point& p1, const Point& p2) {
        return p1.y < p2.y;
    });

    pair<vector<Point>,double> closestP = closestPairRecursive(xSorted,ySorted,g,0,visualize);
    if(visualize) {
        cout << "CLOSEST PAIR FINAL RESULT: ";
        for (int i = 0; i < closestP.first.size(); i++) {
            closestP.first[i].color = RED;
            closestP.first[i].drawBig(g);
            cout << closestP.first[i] << ' ';
        }
        cout << endl;
    }

    return closestP;
}

void genPoints(vector<Point>& points, const Point& p1, const Point& p2, int sampleSize) {
    points.clear();
    random_device rd;
    mt19937_64 mt(rd());
    uniform_int_distribution<int> distX(min(p1.x, p2.x), max(p1.x, p2.x));
    uniform_int_distribution<int> distY(min(p1.y, p2.y), max(p1.y, p2.y));

    for(int i = 0; i < sampleSize; i++) {
        points.push_back(Point(distX(mt), distY(mt)));
    }
}