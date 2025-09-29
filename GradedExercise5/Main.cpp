#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <cstdlib>
#include <vector>

const int WINDOW_WIDTH = 1280; // 128
const int WINDOW_HEIGHT = 720; // 72
const float FPS = 60;
const float TIMESTEP = 1/FPS;
const float FRICTION = 0.5f;
const int BALLS_PER_PRESS = 25;

float e = 1.0f;


struct Ball {
    Vector2 position, velocity, acceleration;
    float radius, mass, inverse_mass;
    Color color;
};

struct GridCell {
    Vector2 position;
    float width, height;
};

std::vector<Ball> balls;

float get_random_float(float min, float max);
void generate_balls();

int main() 
{   
    srand(time(0));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Uniform Grid");

    float accumulator = 0;
    
    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {   
        float delta_time = GetFrameTime();
        
        if(IsKeyPressed(KEY_SPACE)){
            generate_balls();
        }
    
        accumulator += delta_time;
        while(accumulator >= TIMESTEP) {
            for(int _ = 0; _ < balls.size(); _++){
                Ball& circleA = balls[_];

                // ------ SEMI-IMPLICIT EULER INTEGRATION -------
                // Computes for velocity using v(t + dt) = v(t) + (a(t) * dt)
                circleA.velocity = Vector2Add(circleA.velocity, Vector2Scale(circleA.acceleration, TIMESTEP));
                circleA.velocity = Vector2Subtract(circleA.velocity, Vector2Scale(circleA.velocity, FRICTION * circleA.inverse_mass * TIMESTEP));

                // Computes for change in position using x(t + dt) = x(t) + (v(t + dt) * dt)
                circleA.position = Vector2Add(circleA.position, Vector2Scale(circleA.velocity, TIMESTEP));

                // Collision Detection with other balls
                for(int x = 0; x < balls.size(); x++){
                    Ball& circleB = balls[x];

                    // avoid checking if it is the same ball, or if ball is inactive
                    if (_ == x) continue;                    

                    Vector2 collisionVector = Vector2Subtract(circleA.position, circleB.position);
                    float cvMagnitude = Vector2Length(collisionVector);
                    Vector2 velocityRel = Vector2Subtract(circleA.velocity, circleB.velocity);   
                    float dotProduct = Vector2DotProduct(collisionVector, velocityRel);

                    // Collision response
                    if(dotProduct < 0  && cvMagnitude <= (circleA.radius + circleB.radius)){
                        float iNum = (1 + e) * dotProduct;
                        float iDenom = pow(cvMagnitude, 2)
                            * (circleA.inverse_mass + circleB.inverse_mass);
                        float impulse = -(iNum/iDenom);

                        circleA.velocity = Vector2Add(circleA.velocity, 
                            Vector2Scale(collisionVector, impulse/circleA.mass) );
                        
                        circleB.velocity = Vector2Subtract(circleB.velocity,
                            Vector2Scale(collisionVector, impulse/circleB.mass) );
                    }

                }

                // Negates the velocity at x and y if the object hits a wall. (Basic Collision Detection)
                if(circleA.position.x + circleA.radius >= WINDOW_WIDTH || circleA.position.x - circleA.radius <= 0) {
                    circleA.velocity.x *= -1;
                }
                if(circleA.position.y + circleA.radius >= WINDOW_HEIGHT || circleA.position.y - circleA.radius <= 0) {
                    circleA.velocity.y *= -1;
                }
            }

            accumulator -= TIMESTEP;
        }
        
        BeginDrawing();
        ClearBackground(Color {246, 214, 255, 255});
        DrawText(TextFormat("Balls: %04i", balls.size()), 20, 20, 20, WHITE);
        
        for(int _ = 0; _ < balls.size(); _++){
            DrawCircleV(balls[_].position, balls[_].radius, balls[_].color);
        }
        EndDrawing();
    }
    CloseWindow();

    return 0;
}

float get_random_float(float min, float max){
    return min + static_cast <float> (rand() / (static_cast <float> (RAND_MAX/(max-min))));
}

void generate_balls(){
    for(int i = 0; i < BALLS_PER_PRESS; i++){
        balls.emplace_back();
        balls.back().position = {get_random_float(0.0f, WINDOW_WIDTH), get_random_float(0.0f, WINDOW_HEIGHT)};
        balls.back().velocity = {get_random_float(-250.0f, 250.0f), get_random_float(-250.0f, 250.0f)};
        balls.back().acceleration = Vector2Zero();
        // Spawn big balls every BALLS_PER_PRESS*11 (25 * 11) balls
        balls.back().radius = (balls.size() % 275 == 0) ? 25.0f : get_random_float(5.0f, 10.0f);
        balls.back().mass = (balls.size() % 275 == 0) ? 10.0f : 1.0f;
        balls.back().inverse_mass = 1.0f;
        unsigned char r = GetRandomValue(0, 255);
        unsigned char g = GetRandomValue(0, 255);
        unsigned char b = GetRandomValue(0, 255);
        balls.back().color = {r, g, b, 255};
    }
}