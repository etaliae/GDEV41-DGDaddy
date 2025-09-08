/*
References:
- Find, distance: https://www.geeksforgeeks.org/cpp/std-find-in-cpp/
- Float computations: https://stackoverflow.com/questions/686353/random-float-number-generation
*/

#include <raylib.h>
#include <raymath.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int MAX_PARTICLES = 1000;
const float FPS = 60;
const float TIMESTEP = 1/FPS;

struct Particle {
    bool isActive = false;

    Vector2 position;
    Vector2 velocity;
    Vector2 direction;

    float speed;
    float lifetime;
    float radius = 5.0f;
    float colorRate;
    Color color;
};

int controls[6] = {
    32,     //KEY_SPACE
    263,    //KEY_LEFT
    262,    //KEY_RIGHT
    0,      //MOUSE_BUTTON_LEFT
    264,    //KEY_DOWN
    265     //KEY_UP
};

/*
Note that the following control scheme will be used in the Control array:
    controls[0]: bottom-center emissions
    controls[1]: decrease bottom-center emissions rate
    controls[2]: increase bottom-center emissions rate
    controls[3]: "mouse button" emissions
    controls[4]: decrease "mouse button" emissions rate
    controls[5]: increase "mouse button" emissions rate
*/

int config_controls();
void find_inactive();
void activate_particle(int mode);
float get_random_float(float min, float max);
void update_particle(int particle);

Particle* particles = new Particle[MAX_PARTICLES];
bool* active = new bool[MAX_PARTICLES];

int active_particles = 0, first_inactive = 0;
int BCRate = 1, MBRate = 1; //BCRate: Bottom-center rate, MBRate: Mouse-button rate

int main() 
{
    srand(time(0));
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Homework 2");

    for(int i = 0; i < MAX_PARTICLES; i++){
        active[i] = false;
    }

    if(config_controls() != 0){
        std::cout << "Invalid values in config.ini. Aborting." << std::endl;
        return 0;
    }

    float BCaccumulator = 0, MBaccumulator = 0, accumulator = 0;
    
    SetTargetFPS(FPS);
    while (!WindowShouldClose())
    {   
        float delta_time = GetFrameTime();
        
        // Decrease emission rate for bottom-center 
        if(controls[1] < 7){
            if(IsMouseButtonPressed(controls[1])){
                BCRate -=1;
                if (BCRate == 0){
                    BCRate = 1;
                }
            }
        }    
        else{
            if(IsKeyPressed(controls[1])){
                BCRate -=1;
                if (BCRate == 0){
                    BCRate = 1;
                }
            }
        }

        // Increase emission rate for bottom-center
        if(controls[2] < 7){
            if(IsMouseButtonPressed(controls[2])){
                BCRate +=1;
                if (BCRate == 51){
                    BCRate = 50;
                }
            }
        }    
        else{
            if(IsKeyPressed(controls[2])){
                BCRate +=1;
                if (BCRate == 51){
                    BCRate = 50;
                }
            }
        }

        // Decrease emission rate for mouse-button 
        if(controls[4] < 7){
            if(IsMouseButtonPressed(controls[4])){
                MBRate -=1;
                if (MBRate == 0){
                    MBRate = 1;
                }
            }
        }    
        else{
            if(IsKeyPressed(controls[4])){
                MBRate -=1;
                if (MBRate == 0){
                    MBRate = 1;
                }
            }
        }

        // Increase emission rate for mouse-button 
        if(controls[5] < 7){
            if(IsMouseButtonPressed(controls[5])){
                MBRate +=1;
                if (MBRate == 51){
                    MBRate = 50;
                }
            }
        }    
        else{
            if(IsKeyPressed(controls[5])){
                MBRate +=1;
                if (MBRate == 51){
                    MBRate = 50;
                }
            }
        }
        
        // Center-bottom emissions
        if(active_particles < MAX_PARTICLES){
            if(controls[0] < 7){
                if(IsMouseButtonReleased(controls[0])){
                    BCaccumulator = 0;
                }
                if(IsMouseButtonDown(controls[0])){
                    BCaccumulator += delta_time;

                    if(BCaccumulator >= 1/BCRate){
                        find_inactive();
                        activate_particle(0);
                        BCaccumulator -= 1/BCRate;
                    }
                }
            }    
            else{
                if(IsKeyReleased(controls[0])){
                    BCaccumulator = 0;
                }
                if(IsKeyDown(controls[0])){
                    BCaccumulator += delta_time;

                    if(BCaccumulator >= 1/BCRate){
                        find_inactive();
                        activate_particle(0);
                        BCaccumulator -= 1/BCRate;
                    }
                }
            }

            // "Mouse-cursor" emissions
            if(controls[3] < 7){
                if(IsMouseButtonReleased(controls[3])){
                    MBaccumulator = 0;
                }
                if(IsMouseButtonDown(controls[3])){
                    MBaccumulator += delta_time;

                    if(MBaccumulator >= 1/MBRate){
                        find_inactive();
                        activate_particle(1);
                        MBaccumulator -= 1/MBRate;
                    }
                }
            }    
            else{
                if(IsKeyReleased(controls[3])){
                    MBaccumulator = 0;
                }
                if(IsKeyDown(controls[3])){
                    MBaccumulator += delta_time;

                    if(MBaccumulator >= 1/MBRate){
                        find_inactive();
                        activate_particle(1);
                        MBaccumulator -= 1/MBRate;
                    }
                }
            }  
        }
    
        accumulator += delta_time;
        while(accumulator >= TIMESTEP){
            for (int a = 0; a < MAX_PARTICLES; a++){
                if(particles[a].isActive){
                    update_particle(a);
                }   
            }
            accumulator -= TIMESTEP;
        }
        
        BeginDrawing();
        ClearBackground(Color {246, 214, 255, 255});
        DrawText(TextFormat("Particles: %04i", active_particles), 20, 20, 20, WHITE);
        DrawText(TextFormat("Bottom-center rate: %02i", BCRate), 20, 40, 20, WHITE);
        DrawText(TextFormat("Mouse-button rate: %02i", MBRate), 20, 60, 20, WHITE);
        
        for(int _ = 0; _ < MAX_PARTICLES; _++){
            if(particles[_].isActive){
                //std::cout << _ << " pleek print!" << std::endl;
                DrawCircleV(particles[_].position, particles[_].radius, particles[_].color);
            }
        }
        EndDrawing();
    }
    CloseWindow();

    delete[] particles;
    return 0;
}

int config_controls(){
    std::ifstream file("config.ini");
    std::string temp;
    int counter = 0;

    // Process config.ini, making sure the input is valid
    while(std::getline(file, temp)){
        int numtemp;

        if(temp[0] == ';' || temp[0] == '['){
            continue;
        }
        try{
            int index = temp.find('=') + 1;
            numtemp = std::stoi(temp.substr(index));

            if (numtemp > 348){
                throw 1;
            }
        }
        catch(int n){
            std::cout << "Input is not a valid KeyboardKey/MouseButton int" << std::endl;
            return 1;
        }
        catch(...){
            std::cout << "Non-integer input." << std::endl;
            return 1;
        }

        controls[counter] = numtemp;
        std::cout << "Processed " << numtemp << std::endl;

        ++counter;
    }

    return 0;
}

void find_inactive(){
    bool* first = std::find(active, active + MAX_PARTICLES, false);
    std::cout << std::distance(active, first) << std::endl;
    first_inactive = std::distance(active, first);
}

void activate_particle(int mode){
    int ind = first_inactive;

    if (mode == 0){ // Center-bottom
        particles[ind].position = {WINDOW_WIDTH/2, WINDOW_HEIGHT};
        particles[ind].direction = Vector2Normalize({get_random_float(-1.0f, 1.0f), -1.0f});
        particles[ind].lifetime = get_random_float(2.0f, 5.0f);
    }
    else{ // Mouse-button
        particles[ind].position = {(float) GetMouseX(), (float) GetMouseY()};
        particles[ind].direction = Vector2Normalize({get_random_float(-1.0f, 1.0f), get_random_float(-1.0f, 1.0f)});
        particles[ind].lifetime = get_random_float(0.5f, 2.0f);
    }

    particles[ind].isActive = true;
    particles[ind].speed = get_random_float(50.0f, 100.0f);
    particles[ind].velocity = Vector2Scale(particles[ind].direction, particles[ind].speed);
    unsigned char r = GetRandomValue(0, 255);
    unsigned char g = GetRandomValue(0, 255);
    unsigned char b = GetRandomValue(0, 255);
    Color col = {r, g, b, 255};
    particles[ind].color = col;
    particles[ind].colorRate = particles[ind].lifetime/255;

    active[ind] = true;
  
    ++active_particles;
}

float get_random_float(float min, float max){
    return min + static_cast <float> (rand() / (static_cast <float> (RAND_MAX/(max-min))));
}

void update_particle(int particle){
    if(particles[particle].lifetime <= 0){
        particles[particle].isActive = false;
        active[particle] = false;
        --active_particles;
    }
    else{
        particles[particle].position = Vector2Add(particles[particle].position, particles[particle].velocity * TIMESTEP);
        particles[particle].color.a -= particles[particle].colorRate;
        std::cout << static_cast <int> (particles[particle].color.a) << " help" << std::endl;
        particles[particle].lifetime -= TIMESTEP;
    }
    
}
