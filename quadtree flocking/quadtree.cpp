#pragma once
#include "quadtree.h"
#include <iostream>
#include <string>
#include <stack>
#include <SFML/Graphics.hpp>


//implementation of query for a rectangular bounding box, returns a vector of points whose center is contained within
// the bounding box
std::vector<point> query(std::shared_ptr<quadtree> qt, rect* r) {
    std::vector<point> newPoints;

    //managing our own stack here to avoid stack overflows with recursion -- probably overkill for this, but an ounce
    //of prevention...
    std::stack<std::shared_ptr<quadtree>> stack;
    stack.push(qt);


    while(!stack.empty()) {
        auto current = stack.top();
        stack.pop();

        if(current->intersect(r->x - r->width, r->x + r->width, r->y - r->height, r->y + r->height)) {
            for(const auto& pt : current->points){
                if (pt.in(r->x - r->width, r->x + r->width, r->y - r->height, r->y + r->height)) {
                    newPoints.push_back(pt);
                }
            }

            for(const auto& subtree : current->subtrees) {
                if(subtree) {
                    stack.push(subtree);
                }
            }

        }

    }

    return newPoints;
}

//implementation of query for a circular bound, returns a vector of points whose center is contained within the bounding
//circle.
std::vector<point> query(std::shared_ptr<quadtree> qt, circ circle) {
    std::vector<point> newPoints;

    //managing our own stack here to avoid stack overflows with recursion -- probably overkill for this, but an ounce
    //of prevention...
    std::stack<std::shared_ptr<quadtree>> stack;
    stack.push(qt);

    while(!stack.empty()) {
        auto current = stack.top();
        stack.pop();

        if(current->intersect(circle)) {
            for(const auto& pt : current->points){
                if (pt.in(circle)) {
                    newPoints.push_back(pt);
                }
            }

            for(const auto& subtree : current->subtrees) {
                if(subtree) {
                    stack.push(subtree);
                }
            }

        }

    }

    return newPoints;
}


void printQuadtree(std::shared_ptr<quadtree> qt, int padding) {
    if (!qt) return;

    std::string pad(padding, '\t');
    std::cout << "\n\n" << pad << "bounding box: " << (qt->x - qt->width) << " - "
    << (qt->x + qt->width) << ", " << (qt->y - qt->height) << " - " << (qt->y + qt->height) << "\n";

    std::cout << pad << "points: ";
    if (qt->points.empty()) {
        std::cout << "None";
    } else {
        for (size_t ind = 0; ind < qt->points.size(); ++ind) {
            const auto& ele = qt->points[ind];
            std::cout << "\n" << pad << "\tpoint" << ind << ": { x: " << ele.x << ", y: " << ele.y << "}";

        }
    }
    std::cout << "\n";


    std::cout << "\n" << pad << "subtrees:\n";
    for(const auto& subtree : qt->subtrees) {
        if (subtree) {
            printQuadtree(subtree, padding+2);
        }
    }

}
void showQuadtree(std::shared_ptr<quadtree> q, sf::RenderWindow& window) {

    std::stack<std::shared_ptr<quadtree>> stack;
    stack.push(q);

    while(!stack.empty()) {
        auto current = stack.top();
        stack.pop();

        sf::RectangleShape boundingBox(sf::Vector2f((current->width)*2, (current->height)*2));
        boundingBox.setPosition(current->x-current->width, current->y-current->height);
        boundingBox.setFillColor(sf::Color::Transparent);
        boundingBox.setOutlineColor(sf::Color::White);
        boundingBox.setOutlineThickness(0.5);
        window.draw(boundingBox);


        for(const auto& subtree : current->subtrees) {
            if(subtree){
                stack.push(subtree);
            }

        }

    }

    return;
}

void showQuery(const std::vector<point>& points, sf::RenderWindow& window, const rect* r) {

    sf::RectangleShape boundingBox(sf::Vector2f((r->width)*2, (r->height)*2));
    boundingBox.setPosition(r->x-r->width, r->y-r->height);
    boundingBox.setFillColor(sf::Color::Transparent);
    boundingBox.setOutlineColor(sf::Color::Green);
    boundingBox.setOutlineThickness(1);
    window.draw(boundingBox);

    for(const auto& point : points) {
        sf::CircleShape circle(3);
        circle.setPosition(point.x, point.y);
        circle.setFillColor(sf::Color::Green);
        //points.push_back(point);
        window.draw(circle);
    }

    return;
}


