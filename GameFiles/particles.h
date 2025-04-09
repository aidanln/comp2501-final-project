// Particles Class Declarations (derives geometry class)

#ifndef PARTICLES_H_
#define PARTICLES_H_

#include <iostream>
#include <string>
#include <random>
#include <glm/gtc/type_ptr.hpp>

#include "geometry.h"

namespace game {

    // A set of particles that can be rendered
    class Particles : public Geometry {

    public:
        Particles(void);

        // Random num from 0-1
        float RandNum(void);

        // Create different geometries
        void CreateGeometry(int num_particles);
        void CreatePlayerGeometry(int num_particles);

        // Use the geometry
        void SetGeometry(GLuint shader_program);

    private:
        // Random Number Generation Helper
        std::random_device rd;
    };

}

#endif // PARTICLES_H_
