#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <iostream>
#include <algorithm>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float FPS = 60;
const float TIMESTEP = 1 / FPS; // Sets the timestep to 1 / FPS. But timestep can be any very small value.
const float FRICTION = 0.5;
const int MAX_BALLS = 16;
const float BALL_RADIUS = 25.0f;

void init_balls();
bool no_motion();

struct Ball {
    Vector2 position;
    float radius;
    Color color;

    float mass;
    float inverse_mass; // A variable for 1 / mass. Used in the calculation for acceleration = sum of forces / mass
    Vector2 acceleration;
    Vector2 velocity;
    
    bool isActive = true;
};

Rectangle borders[4] = {
    {50, 0, 700, 50}, // Up
    {0, 50, 50, 500}, // Left
    {50, 550, 700, 50}, // Down
    {750, 50, 50, 500} // Right
};

Vector2 pockets[4] = {
    {50, 50}, // Upper Left
    {50, 550}, // Lower Left
    {750, 550}, // Lower Right
    {750, 50} // Upper Right
};

// These remaining rectangles are just for aesthetic
Rectangle underPockets[4] = {
    {0, 0, 50, 50}, // Upper Left
    {0, 550, 50, 50}, // Lower Left
    {750, 550, 50, 50}, // Lower Right
    {750, 0, 50, 50} // Upper Right
};

float xDiff = sqrt(pow(BALL_RADIUS*2, 2) - (pow(BALL_RADIUS, 2)));

// Initial positions for the balls
Vector2 initPos[15] = {
    {400, 300},

    {400 + xDiff, 275},
    {400 + xDiff, 325},

    {400 + 2*xDiff, 250},
    {400 + 2*xDiff, 300},
    {400 + 2*xDiff, 350},

    {400 + 3*xDiff, 225},
    {400 + 3*xDiff, 275},
    {400 + 3*xDiff, 325},
    {400 + 3*xDiff, 375},

    {400 + 4*xDiff, 200},
    {400 + 4*xDiff, 250},
    {400 + 4*xDiff, 300},
    {400 + 4*xDiff, 350},
    {400 + 4*xDiff, 400}
};

Ball* balls = new Ball[MAX_BALLS];
bool e = true;
Vector2 startClick = Vector2Zero(), endClick = Vector2Zero();
int pocketIndex;

int main() {
    
    init_balls();
 
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "311 Sports Lounge");

    SetTargetFPS(FPS);

    float accumulator = 0;

    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        Vector2 forces = Vector2Zero(); // every frame set the forces to a 0 vector

        // Player can't move the ball if there are still balls in motion
        if(no_motion()){
            if(IsMouseButtonPressed(0)) {
                startClick = GetMousePosition();
            }
            if(IsMouseButtonDown(0)) {
                endClick = GetMousePosition();
            }
            if(IsMouseButtonReleased(0)) {
                Vector2 initValue = Vector2ClampValue(Vector2Scale((startClick - endClick), 2500), 0, 50000);
                std::cout << Vector2Length((initValue)) << std::endl;
                forces = Vector2Add(forces, initValue);
                startClick = Vector2Zero();
                endClick = Vector2Zero();
            }
        }
        
        if(IsKeyDown(KEY_R)) {
            init_balls();
        }
        if(IsKeyPressed(KEY_SPACE)) {
            e = !e;
        }
    
        // Does Vector - Scalar multiplication with the sum of all forces and the inverse mass of the ball
        balls[0].acceleration = Vector2Scale(forces, balls[0].inverse_mass);

        // Physics step
        accumulator += delta_time;
        while(accumulator >= TIMESTEP) {
            for(int _ = 0; _ < MAX_BALLS; _++){
                Ball& circleA = balls[_];

                if (!circleA.isActive)
                {
                    if (_ == 0 && no_motion()) // if it is the cue ball and there is no motion among other balls
                    {
                        // set ball's velocity and acceleration to 0
                        circleA.acceleration = Vector2Zero();
                        circleA.velocity = Vector2Zero();

                        // set its position to be just outside the pocket it just got in
                        Vector2 offsetDirection = Vector2Normalize( Vector2Subtract({WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}, pockets[pocketIndex]) );
                        circleA.position = pockets[pocketIndex] + Vector2Scale(offsetDirection, (40.0f + circleA.radius) * 1.1f);

                        // set it back to active
                        circleA.isActive = true;
                    }

                    // no need to do physics and collision checks
                    continue;
                }

                // ------ SEMI-IMPLICIT EULER INTEGRATION -------
                // Computes for velocity using v(t + dt) = v(t) + (a(t) * dt)
                circleA.velocity = Vector2Add(circleA.velocity, Vector2Scale(circleA.acceleration, TIMESTEP));
                circleA.velocity = Vector2Subtract(circleA.velocity, Vector2Scale(circleA.velocity, FRICTION * circleA.inverse_mass * TIMESTEP));
                
                // Stop the car!!!
                if(Vector2Length(circleA.velocity) <= 5.0f){
                    circleA.velocity = Vector2Zero();
                }
                // Computes for change in position using x(t + dt) = x(t) + (v(t + dt) * dt)
                circleA.position = Vector2Add(circleA.position, Vector2Scale(circleA.velocity, TIMESTEP));

                // Collision Detection with other balls
                for(int x = 0; x < MAX_BALLS; x++){
                    Ball& circleB = balls[x];

                    // avoid checking if it is the same ball, or if ball is inactive
                    if (_ == x || !circleB.isActive) continue;                    

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

                // Collision Detection with pockets
                for (int i = 0; i < 4; i++){
                    float distance = Vector2Length( Vector2Subtract(circleA.position, pockets[i]) );

                    // pocket radius: 40
                    if (distance <= circleA.radius + 40.0f)
                    {
                        circleA.isActive = false;

                        // if it is cue ball, note which pocket it got in
                        if (_ == 0) pocketIndex = i;

                        continue;
                    }
                }

                // if ball just got pocketed, no need to check for collisions with borders
                if (!circleA.isActive) continue;

                // handle collision with borders
                for (int i = 0; i < 4; i++){
                    // get the point on the border that is closest to the ball
                    Vector2 closestPoint;
                    closestPoint.x = std::clamp(circleA.position.x, borders[i].x, borders[i].x + borders[i].width);
                    closestPoint.y = std::clamp(circleA.position.y, borders[i].y, borders[i].y + borders[i].height);

                    Vector2 collisionVector = Vector2Subtract(circleA.position, closestPoint);
                    float cvMagnitude = Vector2Length(collisionVector);

                    // if distance between closest point and the ball is greater than the ball's radius,
                    // no collision
                    if (cvMagnitude > circleA.radius) continue;

                    // border is static, so relative velocity is just the velocity of circle A
                    Vector2 velocityRel = circleA.velocity;   
                    float dotProduct = Vector2DotProduct(collisionVector, velocityRel);

                    // if collision normal and relative velocity are towards roughly the same direction, no collision
                    if (dotProduct >= 0) continue;

                    float iNum = (1 + e) * dotProduct;
                    float iDenom = pow(cvMagnitude, 2)
                        * (circleA.inverse_mass + 0.0f); // inverse_mass of border is approximated to 0 since it is static
                    float impulse = -(iNum/iDenom);

                    circleA.velocity = Vector2Add(circleA.velocity, 
                        Vector2Scale(collisionVector, impulse/circleA.mass) );
                }
            }

            accumulator -= TIMESTEP;
        }

        BeginDrawing();
        ClearBackground(GRAY);
        DrawText(TextFormat("Elasticity (Space to toggle): %01i", e), 400, 280, 20, WHITE);
        DrawText(TextFormat("Press 'R' to reset balls."), 400, 300, 20, WHITE);
        

        // Draw the table's borders
        for(int b = 0; b < 4; b++){
            DrawRectangleRec(borders[b], RED);
            DrawRectangleRec(underPockets[b], RED);
        }

        // Draw the pockets
        for(int p = 0; p < 4; p++){
            DrawCircleV(pockets[p], 40.0f, BLACK);
        }
        
        // Draw the circles
        for(int i = 0; i < MAX_BALLS; i++){
            if (balls[i].isActive)
                DrawCircleV(balls[i].position, balls[i].radius, balls[i].color);
        }

        // Draw the line for aiming
        DrawLineEx(startClick, 
            endClick,
            100.0f/Clamp(Vector2Length((startClick - endClick)), 10.0f, 100.0f),
            BLACK);
        
        // Tell the player if they can move
        if(no_motion()){
            DrawText(TextFormat("You may move the balls."), 300, 580, 20, WHITE);
        }
        
        EndDrawing();
    }
    CloseWindow();

    delete[] balls;
    return 0;
}

void init_balls(){
    // Cue ball
    Ball ball;
    ball.position = {200, 300};
    ball.radius = BALL_RADIUS;
    ball.color = WHITE;
    ball.mass = 1.0f;
    ball.inverse_mass = 1 / ball.mass;
    ball.acceleration = Vector2Zero();
    ball.velocity = Vector2Zero();
    balls[0] = ball;

    for(int _ = 1; _ < MAX_BALLS; _++){
        Ball ball;
        ball.radius = BALL_RADIUS;
        ball.position = initPos[_ - 1];
        unsigned char r = GetRandomValue(0, 255);
        unsigned char g = GetRandomValue(0, 255);
        unsigned char b = GetRandomValue(0, 255);
        ball.color = {r, g, b, 255};
        ball.mass = 1.0f;
        ball.inverse_mass = 1 / ball.mass;
        ball.acceleration = Vector2Zero();
        ball.velocity = Vector2Zero();
        balls[_] = ball;
    }
}

bool no_motion(){
    bool inMotion = false;
    for(int _ = 0; _ < MAX_BALLS; _++){
        if(balls[_].isActive && Vector2Length(balls[_].velocity) > 0.0f){
            inMotion = true;
            break;
        }
    }

    return !inMotion;
}
