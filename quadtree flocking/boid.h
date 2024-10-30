#include <utility>

#pragma once


struct boid {
    point p;
    std::vector<float> velocity;
    std::vector<float> acceleration;

    boid(point p_, std::vector<float> velocity_, std::vector<float> acceleration_) : p(std::move(p_)), velocity(std::move(velocity_)), acceleration(std::move(acceleration_)) {
    }

    boid(float x, float y, std::vector<float> velocity_, std::vector<float> acceleration_) : p(x,y), velocity(std::move(velocity_)), acceleration(std::move(acceleration_)) {
    }

};

std::vector<float> calculateSteeringForce(const std::shared_ptr<boid>& b, const std::shared_ptr<quadtree>& q,
                                          float alignmentAttention, float alignmentWeight,
                                          float cohesionAttention, float cohesionWeight,
                                          float separationAttention, float separationWeight);