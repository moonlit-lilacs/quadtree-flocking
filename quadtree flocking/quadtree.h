#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include <cmath>


struct circ {
    float x,y,radius;

    circ(float x_, float y_, float radius_) : x(x_), y(y_), radius(radius_){}
};

struct rect {
    float x,y, width, height;

    rect(float x_, float y_, float width_, float height_) : x(x_), y(y_), width(width_), height(height_){}
};



/*
 * floats x,y are positions of the point in spaces
 * function `in` takes in the dimensions of a bounding box and returns a bool stating if the point is within the
 * bounding box, or takes in the x,y of a circle with a radius r and returns if the point is within that circle.
 */
struct point {
    float x;
    float y;
    std::shared_ptr<void> owner;

    point(float x_, float y_, std::shared_ptr<void> owner_ = nullptr) : x(x_), y(y_), owner(owner_){};

    bool in(float xmin, float xmax, float ymin, float ymax) const {
        return (x >= xmin) && (x <= xmax) && (y >= ymin) && (y <= ymax);
    }

    bool in(circ circle) const {
        return ((this->x - circle.x) * (this->x - circle.x) + (this->y - circle.y) * (this->y - circle.y)) <= (circle.radius*circle.radius);
    }

    void setOwner(std::shared_ptr<void> owner_) {
        owner = owner_;
    }
};




/*
 * floats x,y are positions of the center of the quadtree.
 * floats width and height describe how far the quadtree extends from the center to each side. note: this means that
 * int bucketSize determines how many points can be assigned to one quadtree before we must subdivide
 * vector points contains the points in the quadtree
 * array of pointers subtrees holds the pointers to the children subtrees of the quadtree
 * */
struct quadtree {
    float x, y, width, height;
    int bucketSize;
    std::vector<point> points;
    std::shared_ptr<quadtree> subtrees[4];

    quadtree(float x_, float y_, float width_, float height_, int bucketSize_)
            : x(x_), y(y_), width(width_), height(height_), bucketSize(bucketSize_)
    {
        points.reserve(bucketSize_);
    }

    quadtree(float x_, float y_, float width_, float height_) : x(x_), y(y_), width(width_), height(height_), bucketSize(4) {
        points.reserve(bucketSize);
    }

    std::shared_ptr<quadtree> createChild(int quadrant) {
        switch (quadrant) {
            case 0:
                return std::make_unique<quadtree>(x+width/2, y-height/2, width/2, height/2, bucketSize);
            case 1:
                return std::make_unique<quadtree>(x-width/2, y-height/2, width/2, height/2, bucketSize);
            case 2:
                return std::make_unique<quadtree>(x-width/2, y+height/2, width/2, height/2, bucketSize);
            case 3:
                return std::make_unique<quadtree>(x+width/2, y+height/2, width/2, height/2, bucketSize);
            default:
                throw std::invalid_argument("Invalid quadrant value. Must be 1,2,3, or 4.");
        }
    }


    void insert(point p) {
        if (points.size() < 4) {
            points.push_back(p);
            return;
        }
        else if (subtrees[0] == NULL) {
            for(int i = 0; i < 4; i++){
                subtrees[i] = createChild(i);
            }
        }

        //calculates the quadrant the point should be in by comparing it to the center of the quadtree.
        int quad;
        if (p.x > x) {
            if (p.y > y) {
                quad = 3;
            }
            else {
                quad = 0;
            }
        }
        else {
            if (p.y > y) {
                quad = 2;
            }
            else {
                quad = 1;
            }
        }
        subtrees[quad]->insert(p);
    }


    bool intersect(float xmin, float xmax, float ymin, float ymax) const {
        return !((xmin > x + width) || (xmax < x - width) || (ymin > y + height) || (ymax < y - height));
    }

    bool intersect(circ circle) const {
        float closestX = std::max(x-width, std::min(circle.x, x+width));
        float closestY = std::max(y-height, std::min(circle.y, y+width));
        float dist = sqrt((circle.x - closestX)*(circle.x - closestX) + (circle.y - closestY)*(circle.y - closestY));
        return dist <= circle.radius;

    }

    bool isSubdivided() const {
        return subtrees[0] != nullptr;
    }

};

std::vector<point> query(std::shared_ptr<quadtree> qt, rect* r);
std::vector<point> query(std::shared_ptr<quadtree> qt, circ circle);
void printQuadtree(std::shared_ptr<quadtree> qt, int padding);

