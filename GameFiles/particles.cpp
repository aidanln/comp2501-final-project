#include <iostream>
#include <string>
#include <glm/gtc/type_ptr.hpp>

#include "particles.h"

namespace game {

Particles::Particles(void) : Geometry()
{
    // Initialize variables with default values
    vbo_ = 0;
    ebo_ = 0;
    size_ = 0;
}


// Auxiliary function for generating a random number between 0 and 1
float rand_num(void)
{
    return ((float) rand()) / ((float) RAND_MAX);
}


void Particles::CreateGeometry(int num_particles)
{

    // Each particle is a square with four vertices and two triangles

    // Number of attributes for vertices and faces
    const int vertex_attr = 7;  // 7 attributes per vertex: 2D (or 3D) position (2), direction (2), 2D texture coordinates (2), time (1)
                                //    const int face_att = 3; // Vertex indices (3)

    // Vertices
    GLfloat vertex[]  = {
        // Four vertices of a square
        // Position      Color                Texture coordinates
        -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Top-left
         0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Top-right
         0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f  // Bottom-left
    };

    // Two triangles referencing the vertices
    GLuint face[] = {
        0, 1, 2, // t1
        2, 3, 0  // t2
    };

    // Initialize all the particle vertices
    GLfloat *particles = new GLfloat[num_particles * vertex_attr];
    float theta, r, tmod;
    float pi = glm::pi<float>();
    float two_pi = 2.0f*pi;

    for (int i = 0; i < num_particles; i++){
        // Check if we are initializing a new particle
        //
        // A particle has four vertices, so every four vertices we need
        // to initialize new random values
        if (i % 4 == 0){
            // Get three random values
            // Se above for definition of rand_num()
            //
            // Opening of the stream of particles
            theta = (2.0*rand_num() -1.0f)*0.01f + pi;
            //theta = two_pi*rand_num();
            // Radius (length) of the stream
            r = 0.0f + 0.8*rand_num();
            // Time phase
            tmod = rand_num();
        }

        // Copy position from standard sprite
        particles[i*vertex_attr + 0] = vertex[(i % 4) * 7 + 0];
        particles[i*vertex_attr + 1] = vertex[(i % 4) * 7 + 1];

        // Set direction (velocity) based on random values
        particles[i*vertex_attr + 2] = sin(theta)*r;
        particles[i*vertex_attr + 3] = cos(theta)*r;

        // Set phase based on random values
        particles[i*vertex_attr + 4] = tmod;

        // Copy texture coordinates from standard sprite
        particles[i*vertex_attr + 5] = vertex[(i % 4) * 7 + 5];
        particles[i*vertex_attr + 6] = vertex[(i % 4) * 7 + 6];
    }

    // Initialize all the particle faces
    int num_face_elements = num_particles * 6;
    GLuint *manyfaces = new GLuint[num_face_elements];

    for (int i = 0; i < num_particles; i++) {
        for (int j = 0; j < 6; j++){
            manyfaces[i * 6 + j] = face[j] + i * 4;
        }
    }

    // Create buffer for vertices
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, num_particles * vertex_attr * sizeof(GLfloat), particles, GL_STATIC_DRAW);

    // Create buffer for faces (index buffer)
    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_face_elements * sizeof(GLuint), manyfaces, GL_STATIC_DRAW);

    // Set number of elements in array buffer
    size_ = num_face_elements;

    // Free memory used
    delete[] particles;
    delete[] manyfaces;
}


void Particles::SetGeometry(GLuint shader_program){

    // Set blending
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    // glBlendFunc(GL_ONE, GL_ONE);

    // Bind buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

    // Set attributes for shaders
    // Should be consistent with how we created the buffers for the particle elements
    GLint vertex_att = glGetAttribLocation(shader_program, "vertex");
    glVertexAttribPointer(vertex_att, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(vertex_att);

    // Direction
    GLint dir_att = glGetAttribLocation(shader_program, "dir");
    glVertexAttribPointer(dir_att, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(dir_att);

    // Phase 
    GLint time_att = glGetAttribLocation(shader_program, "t");
    glVertexAttribPointer(time_att, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(4 * sizeof(GLfloat)));
    glEnableVertexAttribArray(time_att);

    // Texture coordinates
    GLint tex_att = glGetAttribLocation(shader_program, "uv");
    glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(tex_att);
}

} // namespace game
