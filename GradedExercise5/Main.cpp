#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <set>

const int WINDOW_WIDTH = 1280; // 128
const int WINDOW_HEIGHT = 720; // 72
const float FPS = 60;
const float TIMESTEP = 1/FPS;
const int BALLS_PER_PRESS = 25;
const float ELASTICITY = 1.0f;

const float GRID_SIZE = 50.0f;

struct Ball {
    Vector2 position, velocity, acceleration;
    float radius, mass, inverse_mass;
    Color color;

    int id;
    std::set<int> collidedBalls;
};

struct GridCell {
    Vector2 position;
    float size = GRID_SIZE;
    std::vector<Ball*> containedBalls;
};

std::vector<Ball> balls;
int spawnCounter = 0;

std::vector<GridCell*> activeCells;

float get_random_float(float min, float max);
void generate_balls();
void set_cell_active(GridCell& cell);
bool hasBallACollidedWithBallB(Ball& ballA, Ball& ballB);

int main() 
{   
    srand(time(0));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Uniform Grid");
    SetTargetFPS(FPS);

    int GRID_WIDTH = (WINDOW_WIDTH % (int) GRID_SIZE == 0) ? WINDOW_WIDTH / GRID_SIZE : WINDOW_WIDTH / GRID_SIZE + 1;
    int GRID_HEIGHT = (WINDOW_HEIGHT % (int) GRID_SIZE == 0) ? WINDOW_HEIGHT / GRID_SIZE : WINDOW_HEIGHT / GRID_SIZE + 1;
    
    GridCell grid[GRID_HEIGHT][GRID_WIDTH];
    for (int i = 0; i < GRID_HEIGHT; i++){
        for (int j = 0; j < GRID_WIDTH; j++){
            grid[i][j].position = {j * GRID_SIZE, i * GRID_SIZE};
        }
    }

    float accumulator = 0;
    
    while (!WindowShouldClose())
    {   
        float delta_time = GetFrameTime();
        
        if(IsKeyPressed(KEY_SPACE)){
            generate_balls();
            spawnCounter++;
        }

    
        accumulator += delta_time;
        while(accumulator >= TIMESTEP) {
            // CLEARING ARRAYS
            // clearing lists of balls in cells
            for(int i = 0; i < GRID_HEIGHT; i++){
                for(int j = 0; j < GRID_WIDTH;j++){
                    grid[i][j].containedBalls.clear();
                }
            }

            // clearing list of cells with balls in them
            activeCells.clear();

            // clearing list of balls a ball has collided with already
            for (int i = 0; i < balls.size(); i++)
                balls[i].collidedBalls.clear();

            for(int _ = 0; _ < balls.size(); _++){
                Ball& circleA = balls[_];

                // ------ SEMI-IMPLICIT EULER INTEGRATION -------
                // Computes for velocity using v(t + dt) = v(t) + (a(t) * dt)
                circleA.velocity = Vector2Add(circleA.velocity, Vector2Scale(circleA.acceleration, TIMESTEP));

                // Computes for change in position using x(t + dt) = x(t) + (v(t + dt) * dt)
                circleA.position = Vector2Add(circleA.position, Vector2Scale(circleA.velocity, TIMESTEP));

                // determine which cells the ball is inside of
                // check top-left corner
                Vector2 TL_indices = Vector2Subtract(circleA.position, {circleA.radius, circleA.radius});
                TL_indices.x = (int) Clamp(TL_indices.x / GRID_SIZE, 0.0f, GRID_WIDTH);
                TL_indices.y = (int) Clamp(TL_indices.y / GRID_SIZE, 0.0f, GRID_HEIGHT);

                GridCell& TL_cell = grid[(int) TL_indices.y][(int) TL_indices.x];
                TL_cell.containedBalls.push_back(&circleA);
                set_cell_active(TL_cell);

                // check bottom-right corner
                Vector2 BR_indices = Vector2Add(circleA.position, {circleA.radius, circleA.radius});
                BR_indices.x = (int) Clamp(BR_indices.x / GRID_SIZE, 0.0f, GRID_WIDTH);
                BR_indices.y = (int) Clamp(BR_indices.y / GRID_SIZE, 0.0f, GRID_HEIGHT);

                // if this is the same cell as the top-left corner's, no need to look for more
                if (FloatEquals(TL_indices.x, BR_indices.x) && FloatEquals(TL_indices.y, BR_indices.y))
                    continue;

                GridCell& BR_cell = grid[(int) BR_indices.y][(int) BR_indices.x];
                BR_cell.containedBalls.push_back(&circleA);
                set_cell_active(BR_cell);

                // to determine other possible cells, check if both the x and y coordinates of the cells are different
                if (!FloatEquals(TL_indices.x, BR_indices.x) && !FloatEquals(TL_indices.y, BR_indices.y))
                {
                    GridCell& TR_cell = grid[(int) TL_indices.y][(int) BR_indices.x];
                    TR_cell.containedBalls.push_back(&circleA);
                    set_cell_active(TR_cell);

                    GridCell& BL_cell = grid[(int) BR_indices.y][(int) TL_indices.x];
                    BL_cell.containedBalls.push_back(&circleA);
                    set_cell_active(BL_cell);
                }
            }

            for(int i = 0; i < activeCells.size(); i++){
                // if cell only contains one ball, no need to check for collisions
                if (activeCells[i]->containedBalls.size() == 1)
                    continue;

                for(int _ = 0; _ < activeCells[i]->containedBalls.size(); _++){
                    Ball& circleA = *activeCells[i]->containedBalls[_];

                    // Collision Detection with other balls
                    for(int x = 0; x < activeCells[i]->containedBalls.size(); x++){
                        Ball& circleB = *activeCells[i]->containedBalls[x];

                        // avoid checking if it is the same ball, or if circleA has collided with circleB (in that order) already
                        if (_ == x || hasBallACollidedWithBallB(circleA, circleB)) continue;

                        Vector2 collisionVector = Vector2Subtract(circleA.position, circleB.position);
                        float cvMagnitude = Vector2Length(collisionVector);
                        Vector2 velocityRel = Vector2Subtract(circleA.velocity, circleB.velocity);   
                        float dotProduct = Vector2DotProduct(collisionVector, velocityRel);

                        // Collision response
                        if(dotProduct < 0  && cvMagnitude <= (circleA.radius + circleB.radius)){
                            float iNum = (1 + ELASTICITY) * dotProduct;
                            float iDenom = pow(cvMagnitude, 2)
                                * (circleA.inverse_mass + circleB.inverse_mass);
                            float impulse = -(iNum/iDenom);

                            circleA.velocity = Vector2Add(circleA.velocity, 
                                Vector2Scale(collisionVector, impulse/circleA.mass) );
                            
                            circleB.velocity = Vector2Subtract(circleB.velocity,
                                Vector2Scale(collisionVector, impulse/circleB.mass) );

                            circleA.collidedBalls.insert(circleB.id);
                        }
                    }
                }
            }

            for(int _ = 0; _ < balls.size(); _++){
                Ball& circleA = balls[_];

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
        
        // draw grid and put number of balls in each cell
        for(int i = 0; i < GRID_HEIGHT; i++){
            for(int j = 0; j < GRID_WIDTH; j++){
                DrawLineV(grid[i][j].position, Vector2Add(grid[i][j].position, {GRID_SIZE, 0.0f}), WHITE);
                DrawLineV(grid[i][j].position, Vector2Add(grid[i][j].position, {0.0f, GRID_SIZE}), WHITE);
                DrawLineV(Vector2Add(grid[i][j].position, {GRID_SIZE, GRID_SIZE}), Vector2Add(grid[i][j].position, {GRID_SIZE, 0.0f}), WHITE);
                DrawLineV(Vector2Add(grid[i][j].position, {GRID_SIZE, GRID_SIZE}), Vector2Add(grid[i][j].position, {0.0f, GRID_SIZE}), WHITE);

                DrawText(TextFormat("%01i", grid[i][j].containedBalls.size()), 
                            grid[i][j].position.x + 5.0f, grid[i][j].position.y + 5.0f, 14, BLACK);

                DrawText(TextFormat("%01i, %0i", i, j), 
                            grid[i][j].position.x + 5.0f, grid[i][j].position.y + GRID_SIZE - 15.0f, 14, WHITE);
            }
        }

        for(int _ = 0; _ < balls.size(); _++){
            DrawCircleV(balls[_].position, balls[_].radius, balls[_].color);
        }

        DrawText(TextFormat("Balls: %04i", balls.size()), 20, 17.5f, 20, GRAY);

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
        balls.back().id = balls.size();
        balls.back().position = {get_random_float(0.0f, WINDOW_WIDTH), get_random_float(0.0f, WINDOW_HEIGHT)};
        balls.back().velocity = {get_random_float(-250.0f, 250.0f), get_random_float(-250.0f, 250.0f)};
        balls.back().acceleration = Vector2Zero();
        // Spawn big balls every BALLS_PER_PRESS*11 (25 * 11) balls
        balls.back().radius = (spawnCounter == 10) ? 25.0f : get_random_float(5.0f, 10.0f);
        balls.back().mass = (spawnCounter == 10) ? 10.0f : 1.0f;
        balls.back().inverse_mass = 1.0f;
        unsigned char r = GetRandomValue(0, 255);
        unsigned char g = GetRandomValue(0, 255);
        unsigned char b = GetRandomValue(0, 255);
        balls.back().color = {r, g, b, 255};

        if (spawnCounter == 10){
            spawnCounter = -1;  // will become 0 in the line after this function is called
            return;
        }
    }
}

void set_cell_active(GridCell& cell){
    // if cell is already among the active cells, do not add it again
    for (int i = 0; i < activeCells.size(); i++){
        if (FloatEquals(activeCells[i]->position.x, cell.position.x) &&
            FloatEquals(activeCells[i]->position.y, cell.position.y))
            return;
    }

    // else, add it in
    activeCells.push_back(&cell);
}

bool hasBallACollidedWithBallB(Ball& ballA, Ball& ballB)
{
    // if Ball B's id was registered among the balls Ball A has collided with already,
    //      Ball A has collided with Ball B already
    return ballA.collidedBalls.count(ballB.id);
}