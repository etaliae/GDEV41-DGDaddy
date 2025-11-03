#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <cstdlib>

#include "entt.hpp"

const int WINDOW_WIDTH = 1280; // Center: 640
const int WINDOW_HEIGHT = 720; // Center: 360
const float FPS = 60;
const float TIMESTEP = 1/FPS;
const float INTERVAL = 1.0f;
const float FRICTION = 0.5f;

struct CircleComponent {
    float radius;
};

struct PositionComponent {
    Vector2 position;
};

struct GachaComponent {
    float directionBehavior, hasTimer, thatsMother;
    // directionBehavior - normal, attracted, or repelled; 
    // hasTimer - whether to get destroyed in 2 to 5 secs;
    // thatsMother - whether it gives birth to asteroids;
};

struct NormalBehaviorComponent {
    Vector2 velocity;
};

struct FollowBehaviorComponent {
    float speed; 
    Vector2 direction;
};

struct RepelBehaviorComponent {
    float speed; 
    Vector2 direction;
};

struct DestroyTimerComponent {
    float timer;
};

struct MotherComponent {
    int babies;
};

struct ColorComponent {
    Color color = DARKBLUE;
};

entt::registry registry;

float spawntimer = 1.0f;

float get_random_float(float min, float max);
void generate_balls(Vector2 initpos = Vector2{WINDOW_WIDTH/2, WINDOW_HEIGHT/2});
void destroysteroid(entt::entity e);

int main() 
{   
    srand(time(0));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Can we pretend that airplanes in the night sky are like shooting stars");

    float accumulator = 0;
    
    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {   
        float delta_time = GetFrameTime();
    
        accumulator += delta_time;

        while(accumulator >= TIMESTEP) {

            spawntimer -= TIMESTEP;

            if (spawntimer <= 0.0f) {
                generate_balls();
                spawntimer = 1.0f;
            }

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
                Vector2 mousePos = GetMousePosition();

                auto all_circle = registry.view<PositionComponent>();
                for (auto entity : all_circle) {
                    PositionComponent& pos = registry.get<PositionComponent>(entity);

                    // early exit if mouse's distance is greater than the biggest radius
                    if (Vector2Distance(mousePos, pos.position) > 50.0f) continue;

                    CircleComponent& rad = registry.get<CircleComponent>(entity);

                    float mouseDist = Vector2Distance(mousePos, pos.position);

                    if (mouseDist <= rad.radius){
                        destroysteroid(entity);
                    }
                }
            }

            auto die = registry.view<DestroyTimerComponent>();
            for (auto entity : die) {
                DestroyTimerComponent& dt = registry.get<DestroyTimerComponent>(entity);
                dt.timer -= TIMESTEP;

                if (dt.timer <= 0.0f){
                    destroysteroid(entity);
                }
            }

            auto normal = registry.view<NormalBehaviorComponent>();
            for (auto entity : normal) {
                CircleComponent& rad = registry.get<CircleComponent>(entity);
                PositionComponent& pos = registry.get<PositionComponent>(entity);
                NormalBehaviorComponent& n = registry.get<NormalBehaviorComponent>(entity);

                pos.position = Vector2Add(pos.position, Vector2Scale(n.velocity, TIMESTEP));

                if (pos.position.x + rad.radius >= WINDOW_WIDTH || pos.position.x - rad.radius <= 0){
                    n.velocity.x *= -1;
                }

                if (pos.position.y + rad.radius >= WINDOW_HEIGHT || pos.position.y - rad.radius <= 0){
                    n.velocity.y *= -1;
                }
            }

            auto follow = registry.view<FollowBehaviorComponent>();
            for (auto entity : follow) {
                CircleComponent& rad = registry.get<CircleComponent>(entity);
                PositionComponent& pos = registry.get<PositionComponent>(entity);
                FollowBehaviorComponent& f = registry.get<FollowBehaviorComponent>(entity);

                f.direction = Vector2Normalize(GetMousePosition() - pos.position);

                Vector2 f_velocity = Vector2Scale(f.direction, f.speed);
                
                pos.position = Vector2Add(pos.position, Vector2Scale(f_velocity, TIMESTEP));

                /* if (pos.position.x + rad.radius >= WINDOW_WIDTH || pos.position.x - rad.radius <= 0){
                    f_velocity.x *= -1;
                }

                if (pos.position.y + rad.radius >= WINDOW_HEIGHT || pos.position.y - rad.radius <= 0){
                    f_velocity.y *= -1;
                } */
            }  
            
            auto repel = registry.view<RepelBehaviorComponent>();
            for (auto entity : repel) {
                CircleComponent& rad = registry.get<CircleComponent>(entity);
                PositionComponent& pos = registry.get<PositionComponent>(entity);
                RepelBehaviorComponent& r = registry.get<RepelBehaviorComponent>(entity);

                r.direction = Vector2Negate(Vector2Normalize(GetMousePosition() - pos.position));

                Vector2 r_velocity = Vector2Scale(r.direction, r.speed);
                
                if (pos.position.x + rad.radius >= WINDOW_WIDTH || pos.position.x - rad.radius <= 0){
                    r_velocity.x *= -1;
                }

                if (pos.position.y + rad.radius >= WINDOW_HEIGHT || pos.position.y - rad.radius <= 0){
                    r_velocity.y *= -1;
                }

                pos.position = Vector2Add(pos.position, Vector2Scale(r_velocity, TIMESTEP));
                
            }  

            accumulator -= TIMESTEP;
        }
        
        BeginDrawing();
        ClearBackground(Color {246, 214, 255, 255});
       
        int counter = 0;

        auto all_circle = registry.view<PositionComponent, CircleComponent, ColorComponent>();

        for (auto entity : all_circle) {
            PositionComponent& pos = registry.get<PositionComponent>(entity);
            CircleComponent& rad = registry.get<CircleComponent>(entity);
            ColorComponent& col = registry.get<ColorComponent>(entity);

            DrawCircleV(pos.position, rad.radius, col.color);

            counter++;
        }        
        DrawText(TextFormat("Balls: %04i",counter), 20, 20, 20, WHITE);

        EndDrawing();
    }
    CloseWindow();

    registry.clear();
    return 0;
}

float get_random_float(float min, float max){
    return min + static_cast <float> (rand() / (static_cast <float> (RAND_MAX/(max-min))));
}

void generate_balls(Vector2 initpos){
    entt::entity e = registry.create();

    registry.emplace<CircleComponent>(e, get_random_float(20.0f, 50.0f));

    PositionComponent& pos = registry.emplace<PositionComponent>(e, Vector2{
        get_random_float(initpos.x - 100.0f, initpos.x + 100.0f),
        get_random_float(initpos.y - 100.0f, initpos.y + 100.0f)});

    GachaComponent& rng = registry.emplace<GachaComponent>(e, 
        get_random_float(1.0f, 100.0f), get_random_float(1.0f, 100.0f), get_random_float(1.0f, 100.0f));

    
    ColorComponent& c = registry.emplace<ColorComponent>(e);

    if (rng.directionBehavior <= 50.0f){
        NormalBehaviorComponent& n = registry.emplace<NormalBehaviorComponent>(e);
        n.velocity = Vector2{get_random_float(-200.0f, 200.0f), get_random_float(-200.0f, 200.0f)};
    }
    else if (rng.directionBehavior <= 75.0f){ // attracted
        FollowBehaviorComponent& f = registry.emplace<FollowBehaviorComponent>(e);
        f.speed = get_random_float(10.0f, 300.0f);
        f.direction = Vector2Normalize(GetMousePosition() - pos.position);
        c.color = VIOLET;
    }
    else { // repelled
        RepelBehaviorComponent& a = registry.emplace<RepelBehaviorComponent>(e);
        a.speed = get_random_float(10.0f, 100.0f);
        a.direction = Vector2Negate(Vector2Normalize(GetMousePosition() - pos.position));
        c.color = MAGENTA;
    }

    if (rng.hasTimer <= 35.0f){
        registry.emplace<DestroyTimerComponent>(e, get_random_float(2.0f, 5.0f));
        c.color = RED;
    }

    if (rng.thatsMother <= 10.0f){
        registry.emplace<MotherComponent>(e, GetRandomValue(2, 5));
        c.color = ORANGE;
    }
}

void destroysteroid(entt::entity e){
    if (registry.all_of<MotherComponent>(e)){
        MotherComponent& m = registry.get<MotherComponent>(e);
        PositionComponent& p = registry.get<PositionComponent>(e);

        for (int i = 0; i < m.babies; i++) {
            generate_balls(p.position);
        }
    }

    registry.destroy(e);
}