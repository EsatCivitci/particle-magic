#version 430 core

layout (std140, binding = 0) uniform attractor_block {
    vec4 attractor[12]; // xyz positions, w mass
};

// Block size 128 along x
layout (local_size_x = 128) in;

// Particle's positions and velocities in a buffer
layout (rgba32f, binding = 0) uniform imageBuffer velocity_buffer;
layout (rgba32f, binding = 1) uniform imageBuffer position_buffer;

uniform float dt; // Delta time
uniform int attractorSize; // Total number of attractors
uniform vec3 origin; // Origin position

void main() {
    // Read the current position and velocity from the buffers
    vec4 velocity = imageLoad(velocity_buffer, int(gl_GlobalInvocationID.x));
    vec4 position = imageLoad(position_buffer, int(gl_GlobalInvocationID.x));

    // Update position via velocity
    position.xyz += velocity.xyz * dt;

    // Update the life time of a particle
    position.w -= 0.1 * dt;

    for (int i = 0; i < attractorSize; ++i) {
        // Calculate the force and update the velocity for the next time step
        vec3 dist = attractor[i].xyz - position.xyz;
        velocity.xyz += 5 * dt * attractor[i].w * normalize(dist) / (dot(dist,dist) + 1); 
    }

    if (position.w <= 0.0) {
        position.xyz = origin;
        position.w += 1.0;
    }

    // Store the new position and velocity back into the buffers
    imageStore(velocity_buffer, int(gl_GlobalInvocationID.x), velocity);
    imageStore(position_buffer, int(gl_GlobalInvocationID.x), position);
}


