#include "quadtree.cpp"
#include <random>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <string>
#include <cstdio>
#include "boid.h"
#include <cmath>


int main() {

    //define our constant variables of the simulation.
    const int width = 1920;
    const int height = 1080;

    const int boidCount = 500;
    const int boidSize = 2;

    const float alignmentWeight = 1;
    const float cohesionWeight = 0.1;
    const float separationWeight = 10;

    const float alignmentAttention = 40;
    const float cohesionAttention = 40;
    const float separationAttention = 20;

    //can increase this value to "increase the speed" of the simulation, but not that this doesn't compute more frames,
    //it naively multiplies the change in position and velocity by the timeStep value.
    const float timeStep = 1;


    //initializing our pseudo-random number generator to randomize our boid's starting positions (and add jitter)
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> distx(0,width);
    std::uniform_real_distribution<float> disty(0,height);
    std::uniform_real_distribution<float> gen(-1,1);


    std::vector<std::shared_ptr<boid>> boids;

    for (size_t i = 0; i < boidCount; i++){
        std::vector<float> acc = {gen(mt)*10,gen(mt)*10};
        std::vector<float> vel = {gen(mt)*100,gen(mt)*100};

        auto b = std::make_shared<boid>(distx(mt), disty(mt), vel, acc);
        b->p.setOwner(b);
        boids.push_back(b);
        (*boids.back()).p.setOwner(boids.back());

    }


    sf::RenderWindow window(sf::VideoMode(width, height), "Flocking");
    sf::Clock clock;
    float lastTime = 0;
    int frameCount = 0;
    auto previousTime = std::chrono::high_resolution_clock::now();


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //fps calculations, put into the title of the window to keep it out of the way
        lastTime += clock.restart().asSeconds();
        frameCount++;
        if (lastTime >= 1.0f) {
            float fps = frameCount / lastTime;
            window.setTitle("FPS: " + std::to_string(static_cast<int>(fps)));

            lastTime = 0;
            frameCount = 0;
        }

        //start by clearing any previous drawing from last frame
        window.clear(sf::Color::Black);

        //calculate the delta time to ensure that physics aren't tied to frame rate.
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        //insert each boids position as a point into the quadtree to speed up localized interactions
        auto q = std::make_shared<quadtree>(quadtree(width/2, height/2, width/2, height/2));
        for(auto& boid : boids) {
            q->insert(boid->p);
        }

        //draw each boid, calculate their new velocity and acceleration
        for(auto& boid : boids) {
            sf::CircleShape circle(boidSize);
            circle.setPosition(boid->p.x, boid->p.y);
            circle.setFillColor(sf::Color::White);
            window.draw(circle);
            boid->acceleration = calculateSteeringForce(boid, q, alignmentAttention, alignmentWeight,
                                                        cohesionAttention, cohesionWeight, separationAttention,
                                                        separationWeight);
            boid->p.x += boid->velocity[0]*deltaTime.count()*timeStep;
            boid->p.y += boid->velocity[1]*deltaTime.count()*timeStep;
            boid->velocity[0] += boid->acceleration[0]*deltaTime.count()*timeStep;
            boid->velocity[1] += boid->acceleration[1]*deltaTime.count()*timeStep;


            //if the boid has gone off the screen, warp it to the other end Pacman-style
            if (boid->p.x > width) {
                boid->p.x = std::fmod(boid->p.x, static_cast<float>(width));
            } else if (boid->p.x < 0) {
                boid->p.x = width - boid->p.x;
            }
            if (boid->p.y > height) {
                boid->p.y = std::fmod(boid->p.y, static_cast<float>(height));
            } else if (boid->p.y < 0) {
                boid->p.y = height - boid->p.y;
            }

            //add a slight bit of randomness to each boid's velocity to make it feel more natural.
            boid->velocity[0] += gen(mt);
            boid->velocity[1] += gen(mt);

        }

        window.display();
    }
    return 0;
}