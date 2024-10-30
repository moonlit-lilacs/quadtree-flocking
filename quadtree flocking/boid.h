#pragma once


struct boid {
    point p;
    std::vector<float> velocity;
    std::vector<float> acceleration;

    boid(point p_, std::vector<float> velocity_, std::vector<float> acceleration_) : p(p_), velocity(velocity_), acceleration(acceleration_) {
//        p.setOwner(shared_from_this());
    }

    boid(float x, float y, std::vector<float> velocity_, std::vector<float> acceleration_) : p(x,y), velocity(velocity_), acceleration(acceleration_) {

//        p.setOwner(shared_from_this());

    }

};

//this monster of a function calculates the steering force by calculating the alignment, cohesion, and separation
//forces and then combining them together weighted by the given parameters. if each boid had a different mass, then
//we would account for that, but as they don't we can simply sum the forces on it and apply to acceleration.
std::vector<float> calculateSteeringForce(std::shared_ptr<boid> b,
                                          std::shared_ptr<quadtree> q,
                                          float alignmentAttention,
                                          float alignmentWeight,
                                          float cohesionAttention,
                                          float cohesionWeight,
                                          float separationAttention,
                                          float separationWeight) {

    std::vector<float> steeringVector = {0,0};
    std::vector<float> alignmentVector = {0,0};
    std::vector<float> cohesionVector = {0,0};
    std::vector<float> separationVector = {0,0};
    float alignmentXAccumulator = 0;
    float alignmentYAccumulator = 0;
    float cohesionXAccumulator = 0;
    float cohesionYAccumulator = 0;


    //define our attention circles and query for all the points within that circle for each of our flocking constraints.
    circ alignmentCircle = circ(b->p.x, b->p.y, alignmentAttention);
    std::vector<point> boidsAlignment = query(q,alignmentCircle);

    circ cohesionCircle = circ(b->p.x, b->p.y, cohesionAttention);
    std::vector<point> boidsCohesion = query(q,cohesionCircle);

    circ separationCircle = circ(b->p.x, b->p.y, separationAttention);
    std::vector<point> boidsSeparation = query(q, separationCircle);


    for (size_t i = 0; i < boidsAlignment.size(); i++) {
        point boidPoint = boidsAlignment[i];
        if (boidPoint.owner != nullptr) {
            std::shared_ptr<boid> bi = std::static_pointer_cast<boid>(boidPoint.owner);
            alignmentXAccumulator += bi->velocity[0];
            alignmentYAccumulator += bi->velocity[1];

        }
    }
    for (size_t i = 0; i < boidsCohesion.size(); i++) {
        point boidPoint = boidsCohesion[i];
        if (boidPoint.owner != nullptr) {
            std::shared_ptr<boid> bi = std::static_pointer_cast<boid>(boidPoint.owner);
            cohesionXAccumulator += bi->p.x;
            cohesionYAccumulator += bi->p.y;
        }
    }

    for(size_t i = 0; i < boidsSeparation.size(); i++) {
        point boidPoint = boidsSeparation[i];
        if (boidPoint.owner != nullptr) {
            std::shared_ptr<boid> bi = std::static_pointer_cast<boid>(boidPoint.owner);
            if (bi == b) continue;

            float dx = b->p.x - bi->p.x;
            float dy = b->p.y - bi->p.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            float repulsion = separationWeight / distance;

            separationVector[0] += dx*repulsion;
            separationVector[1] += dy*repulsion;

        }
    }



    if (boidsAlignment.size() > 0) {
        alignmentVector[0] += (alignmentXAccumulator / boidsAlignment.size()) - b->velocity[0];
        alignmentVector[1] += (alignmentYAccumulator / boidsAlignment.size()) - b->velocity[1];
    }
    if (boidsCohesion.size() > 0){
        cohesionVector[0] += (cohesionXAccumulator / boidsCohesion.size()) - b->p.x;
        cohesionVector[1] += (cohesionYAccumulator / boidsCohesion.size()) - b->p.y;
    }
    if(boidsSeparation.size() > 0) {
        separationVector[0] /= boidsSeparation.size();
        separationVector[1] /= boidsSeparation.size();
    }


    steeringVector[0] = alignmentVector[0] * alignmentWeight + cohesionVector[0] * cohesionWeight + separationVector[0] * separationWeight;
    steeringVector[1] = alignmentVector[1] * alignmentWeight + cohesionVector[1] * cohesionWeight + separationVector[1] * separationWeight;

    return steeringVector;


}