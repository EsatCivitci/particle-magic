# Particle Magic — Real-Time GPU Compute Shader Simulation

<div style="position: relative; padding-bottom: 56.25%; height: 0; overflow: hidden; max-width: 100%; border-radius: 8px; margin: 1em 0;">
  <iframe 
    src="https://www.youtube-nocookie.com/embed/x6esdj_qDn8" 
    title="Particle Simulation in 3D Space using OpenGL"
    style="position: absolute; top: 0; left: 0; width: 100%; height: 100%; border: 0;" 
    allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" 
    allowfullscreen>
  </iframe>
</div>

Hello everyone, welcome to my blog. In this blog, I will explain my homework for Computer Graphics 2 class.

For this assignment, I had to build a particle simulation using compute shaders. To make things more interesting (and realistic), I added attractors that pull on the particles and influence their movement.

## 1. Implementation

### 1.1 Initialize Particles

For generating particles, I set up two buffers—one for particle positions and one for velocities. In the position buffer, the xyz channels store each particle’s coordinates, and the w channel keeps track of its age. When initializing, I just fill both buffers with random values. Once they’re ready, I upload those buffers as textures so the compute shader can use them.

### 1.2 Initialize Attractors 

For storing the  attractors, I have used uniform buffer objects. Again I used the xyz channels for storing the coordinates of the attractor and the w channel for the weight of the attractor. 


### 1.3 Adding Physics by Using Compute Shaders

Since I will render more than million particles in real-time, I have used compute shaders for the parallel execution. 

Algorithm

* Load current state by using the imageLoad() 
* Move the particle: newPos = oldPos + v * dt
* Calculate the new age of the particle: oldAge -= 0.1 * dt
* Calculate the applied force from each attractor and sum them up.
* Respawn the particles if they are dead
* Write back to the buffer using imageStore()

Compute Shader Code

```cpp
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
```

“glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT)” is used to make sure that all writes of my compute shader just did to those image‐bound buffers are fully finished and visible before any subsequent OpenGL calls try to read from them. Without it, the GPU might reorder or delay those writes, causing stale or undefined data when you later sample or render using those same textures. After that, I draw my points using “glDrawArrays()”.

## 2. Results and Observations
### 2.1 Performance Observation

Compute shaders are very powerful object for rendering complex objects. With the help of the parallel execution feature of compute shaders we can render more than 10 million particles real time. Here is the result of rendering the particles with different point sizes and different numbers. I have used NVDIA GTX 1650ti for this experiment.

| Particle Count / Point Size | Size = 1 | Size = 10 | Size = 25 | Size = 50 |
| :--- | :---: | :---: | :---: | :---: |
| **10** | 950 fps | 950 fps | 950 fps | 950 fps |
| **1000** | 950 fps | 900 fps | 800 fps | 600 fps |
| **100000** | 450 fps | 210 fps | 90 fps | 30 fps |
| **1000000** | 110 fps | 40 fps | 13 fps | 6 fps |
| **10000000** | 20 fps | 8 fps | 3 fps | 1 fps |

## 3. Added Functionalities

* “R”: Pause/Resume
* “W”: Speed Up
* “S”: Speed down
* “T”: Enable/Disable Text
* “G”: Attractor/Origin Mode
* “F”: Full Screen
* “Left Click”: Set New Origin or Attractor
* “Right Click”: Remove the Last Placed Attractor
* “Mouse Scroll Up”: Increase the Mass
* “Mouse Scroll Down”: Decrease the Mass

## 4. Final Notes

Building this simulation was both fun and rewarding. I learned how compute shaders can move millions of particles in parallel and how adjusting attractor forces, lifetimes, and respawn behavior affects both performance and realism. Watching frame rates change as I increased particle counts or point sizes really showed me the power of GPU workgroups.

If you’re curious about any part of this project, feel free to reach out.

**[Watch the full demo on YouTube](http://www.youtube.com/watch?v=x6esdj_qDn8)**

Thanks for reading!
