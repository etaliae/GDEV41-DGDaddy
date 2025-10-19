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

// Two structs referencing each other
// Source: https://stackoverflow.com/questions/4394079/structs-that-refer-to-each-other

struct Ball;
struct Node;

struct Ball {
    Vector2 position, velocity, acceleration;
    float radius, mass, inverse_mass;
    Color color;

    Node* node;
};

struct Node {
    // AABB bounds
    Vector2 position;                   // center
    float half_size;                    // aka radius
    
    // pointers to the parent and children
    Node* parent = nullptr;
    std::vector<Node*> children;

    std::vector<Ball*> containedBalls;

    ~Node()
    {
        for (int i = 0; i < children.size(); i++)
            delete children[i];
    }

    bool isLeafNode()
    {
        // if this node's half-size is equal to 10, it is the leaf node
        return FloatEquals(half_size, 10.0f);

        /**
         * 5 was not chosen because at best, the circles of radius 5 will coincide
         * with the borders of nodes of half-size 5, making the circles not part of
         * any of these nodes
         * 
         * 10 is the next smallest
         */
    }

    bool isRootNode()
    {
        // if this node's half-size is equal to 640, it is the root node
        return FloatEquals(half_size, 640.0f);
    }
};

std::vector<Ball> balls;
int spawnCounter = 0;

Node* root;
std::vector<Node*> activeNodes;

float get_random_float(float min, float max);
void generate_balls();
void create_quadtree(Node& node);
bool is_ball_in_node(Ball& ball, Node& node);
void get_node_of_ball(Ball& ball, Node& current);
void find_new_node(Ball& ball, Node& current);
void set_node_active(Node& node);
std::vector<Node*> get_ancestors_of_node(Node& node);
void handle_collisions_with_ancestors(Ball& ball);
void handle_collisions_with_descendants(Ball& ball, Node& current);

int main() 
{   
    srand(time(0));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Quadtrees");
    SetTargetFPS(FPS);

    root = new Node;
    root->position = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
    root->half_size = (WINDOW_WIDTH >= WINDOW_HEIGHT) ? WINDOW_WIDTH / 2 : WINDOW_HEIGHT / 2; 
    create_quadtree(*root); // create the quadtree from the root node

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
            // clearing lists of balls in nodes
            // NOTE: not the same with removing balls and reinserting them in the quadtree;
            //          balls still have reference to their node (via node member in Ball struct)
            for(int i = 0; i < activeNodes.size(); i++)
                activeNodes[i]->containedBalls.clear();

            // clearing list of nodes with balls in them
            activeNodes.clear();

            // adding the root to list of active nodes (since its always active if there is an active ball)
            activeNodes.push_back(root);

            for(int _ = 0; _ < balls.size(); _++){
                Ball& circleA = balls[_];

                // ------ SEMI-IMPLICIT EULER INTEGRATION -------
                // Computes for velocity using v(t + dt) = v(t) + (a(t) * dt)
                circleA.velocity = Vector2Add(circleA.velocity, Vector2Scale(circleA.acceleration, TIMESTEP));

                // Computes for change in position using x(t + dt) = x(t) + (v(t + dt) * dt)
                circleA.position = Vector2Add(circleA.position, Vector2Scale(circleA.velocity, TIMESTEP));

                // if the ball is enclosed by the node it belonged to, check the children of that node
                if ( is_ball_in_node(circleA, *(circleA.node)) )
                    get_node_of_ball(circleA, *(circleA.node));

                // else, look for the new node it belongs to (starting with the parent of the old node)
                else
                    find_new_node(circleA, *(circleA.node->parent));

                // Negates the velocity at x and y if the object hits a wall. (Basic Collision Detection)
                if(circleA.position.x + circleA.radius >= WINDOW_WIDTH || circleA.position.x - circleA.radius <= 0) {
                    circleA.velocity.x *= -1;
                }
                if(circleA.position.y + circleA.radius >= WINDOW_HEIGHT || circleA.position.y - circleA.radius <= 0) {
                    circleA.velocity.y *= -1;
                }
            }

            for(int _ = 0; _ < balls.size(); _++){
                Ball& circleA = balls[_];
                handle_collisions_with_ancestors(circleA);
                // handle_collisions_with_descendants(circleA, *(circleA.node));
            }

            accumulator -= TIMESTEP;
        }
        
        BeginDrawing();
        ClearBackground(Color {246, 214, 255, 255});

        for(int i = 0; i < activeNodes.size(); i++){
            DrawLineV(Vector2Add(activeNodes[i]->position, {-activeNodes[i]->half_size, -activeNodes[i]->half_size}),
                        Vector2Add(activeNodes[i]->position, {activeNodes[i]->half_size, -activeNodes[i]->half_size}),
                        GRAY);

            DrawLineV(Vector2Add(activeNodes[i]->position, {-activeNodes[i]->half_size, -activeNodes[i]->half_size}),
                        Vector2Add(activeNodes[i]->position, {-activeNodes[i]->half_size, activeNodes[i]->half_size}),
                        GRAY);

            DrawLineV(Vector2Add(activeNodes[i]->position, {activeNodes[i]->half_size, activeNodes[i]->half_size}),
                        Vector2Add(activeNodes[i]->position, {activeNodes[i]->half_size, -activeNodes[i]->half_size}),
                        GRAY);

            DrawLineV(Vector2Add(activeNodes[i]->position, {activeNodes[i]->half_size, activeNodes[i]->half_size}),
                        Vector2Add(activeNodes[i]->position, {-activeNodes[i]->half_size, activeNodes[i]->half_size}),
                        GRAY);
        }
        
        for(int _ = 0; _ < balls.size(); _++){
            DrawCircleV(balls[_].position, balls[_].radius, balls[_].color);
        }

        DrawText(TextFormat("Balls: %04i", balls.size()), 20, 17.5f, 20, BLACK);

        EndDrawing();
    }

    delete root;

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
        balls.back().radius = (spawnCounter == 10) ? 25.0f : get_random_float(5.0f, 10.0f);
        balls.back().mass = (spawnCounter == 10) ? 10.0f : 1.0f;
        balls.back().inverse_mass = 1.0f;
        unsigned char r = GetRandomValue(0, 255);
        unsigned char g = GetRandomValue(0, 255);
        unsigned char b = GetRandomValue(0, 255);
        balls.back().color = {r, g, b, 255};

        get_node_of_ball(balls.back(), *root);

        if (spawnCounter == 10){
            spawnCounter = -1;  // will become 0 after this function is called
            return;
        }
    }
}

void create_quadtree(Node& node)
{
    // top-left child
    Node* TL_child = new Node;
    TL_child->position = Vector2Add(node.position, {-node.half_size / 2, -node.half_size / 2});
    TL_child->half_size = node.half_size / 2;
    TL_child->parent = &node;
    node.children.push_back(TL_child);

    // top-right child
    Node* TR_child = new Node;
    TR_child->position = Vector2Add(node.position, {node.half_size / 2, -node.half_size / 2});
    TR_child->half_size = node.half_size / 2;
    TR_child->parent = &node;
    node.children.push_back(TR_child);

    // bottom-leftt child
    Node* BL_child = new Node;
    BL_child->position = Vector2Add(node.position, {-node.half_size / 2, node.half_size / 2});
    BL_child->half_size = node.half_size / 2;
    BL_child->parent = &node;
    node.children.push_back(BL_child);

    // bottom-right child
    Node* BR_child = new Node;
    BR_child->position = Vector2Add(node.position, {node.half_size / 2, node.half_size / 2});
    BR_child->half_size = node.half_size / 2;
    BR_child->parent = &node;
    node.children.push_back(BR_child);

    // if children are leaf nodes (the children can only be either all leaves or all non-leaves),
    // do not proceed
    if (TL_child->isLeafNode()) return;

    // create more children with each child as the "root"
    create_quadtree(*TL_child);
    create_quadtree(*TR_child);
    create_quadtree(*BL_child);
    create_quadtree(*BR_child);
}

bool is_ball_in_node(Ball& ball, Node& node)
{
    // if node is root node, ball is definitely inside it
    if (node.isRootNode()) return true;

    // top-left and bottom-right corners of node
    Vector2 nodeTL = Vector2Subtract(node.position, {node.half_size, node.half_size});
    Vector2 nodeBR = Vector2Add(node.position, {node.half_size, node.half_size});

    // top-left and bottom-right corners of the AABB of the ball
    Vector2 ballTL = Vector2Subtract(ball.position, {ball.radius, ball.radius});
    Vector2 ballBR = Vector2Add(ball.position, {ball.radius, ball.radius});

    return ballTL.x > nodeTL.x && ballTL.y > nodeTL.y &&
            ballBR.x < nodeBR.x && ballBR.y < nodeBR.y;
}

// checks if ball is contained in one of current's children (unless current node is a leaf node)
void get_node_of_ball(Ball& ball, Node& current)
{
    // ball is always in current node (either current is the root, or this fact was confirmed in last recursion step)

    // if the current node is not a leaf, and
    // if ball is smaller than the any of the children whose size is half of their parent, we can proceed
    // (otherwise, ball cannot be contained by the children)
    if ( !current.isLeafNode() && ball.radius < current.half_size / 2 ){
        
        // check if the ball is in any of the four children
        for (int i = 0; i < 4; i++){  
            // if ball is in a child,
            if ( is_ball_in_node(ball, *(current.children[i])) ){
                // set the node active
                set_node_active(*(current.children[i]));

                // if that child is a leaf,
                if (current.children[i]->isLeafNode()){
                    // it belongs to that leaf node
                    current.children[i]->containedBalls.push_back(&ball);
                    ball.node = current.children[i];
                    return;
                }

                // else, we need to check the child's children
                get_node_of_ball( ball, *(current.children[i]) );
                return;
            }
        }
    }

    // if it isn't in any of the children, it belongs to the current node
    current.containedBalls.push_back(&ball);
    ball.node = &current;
}

// looks for the deepest ancestor that contains the ball, and then
// lets "get_node_of_ball" function check that ancestor's other descendants
void find_new_node(Ball& ball, Node& current)
{
    // if the ball is in the current node,
    if (is_ball_in_node(ball, current)){
        // set this node to active
        set_node_active(current);

        // check this node's descendants
        get_node_of_ball(ball, current);
        return;
    }

    // else, traverse further upwards
    find_new_node(ball, *(current.parent));
}

void set_node_active(Node& node)
{
    // if this is the root node, no need to add it again
    if (node.isRootNode()) return;

    // if node is already among the active nodes, do not add it again
    for (int i = 0; i < activeNodes.size(); i++){
        if (FloatEquals(activeNodes[i]->position.x, node.position.x) &&
            FloatEquals(activeNodes[i]->position.y, node.position.y))
            return;
    }

    // else, add it in
    activeNodes.push_back(&node);
}

std::vector<Node*> get_ancestors_of_node(Node& node)
{
    std::vector<Node*> ancestors;

    Node* current = &node;

    while (current != nullptr)
    {
        ancestors.push_back(current);        
        if (current->parent == nullptr) break;
        current = current->parent;
    }

    return ancestors;
}

void handle_collisions_with_ancestors(Ball& ball)
{
    // first, handle collisions with ancestors, starting from the biggest node to the node that the ball is currently in
    std::vector<Node*> ancestors = get_ancestors_of_node(*(ball.node));

    // for each ancestor,
    for (int i = ancestors.size()-1; i >= 0; i--){
        // if this ancestor contains no balls, continue to next ancestor
        if(ancestors[i]->containedBalls.size() == 0) continue;

        // we check each ball that belongs to that ancestor
        for (int j = 0; j < ancestors[i]->containedBalls.size()-1; j++){
            Ball& circleB = *(ancestors[i]->containedBalls[j]);

            // if it belongs to the same node as the ball
            if (i == 0){
                // check if it is the same ball by checking their positions and radii
                // if they are the same, ignore this ball
                if (FloatEquals(ball.position.x, circleB.position.x) &&
                    FloatEquals(ball.position.y, circleB.position.y) &&
                    FloatEquals(ball.radius, circleB.radius))
                    return;
            }

            // else, we proceed to checking for collision

            Vector2 collisionVector = Vector2Subtract(ball.position, circleB.position);
            float cvMagnitude = Vector2Length(collisionVector);
            Vector2 velocityRel = Vector2Subtract(ball.velocity, circleB.velocity);   
            float dotProduct = Vector2DotProduct(collisionVector, velocityRel);

            // Collision response
            if(dotProduct < 0  && cvMagnitude <= (ball.radius + circleB.radius)){
                float iNum = (1 + ELASTICITY) * dotProduct;
                float iDenom = pow(cvMagnitude, 2)
                    * (ball.inverse_mass + circleB.inverse_mass);
                float impulse = -(iNum/iDenom);

                ball.velocity = Vector2Add(ball.velocity, 
                    Vector2Scale(collisionVector, impulse/ball.mass) );
                
                circleB.velocity = Vector2Subtract(circleB.velocity,
                    Vector2Scale(collisionVector, impulse/circleB.mass) );
            }
        }
    }
}

void handle_collisions_with_descendants(Ball& ball, Node& current)
{
    // if this is a leaf node, do not proceed since it has no descendants
    if (current.isLeafNode()) return;

    // check each child
    for (int i = 0; i < 4; i++){
        Node& child = *(current.children[i]);

        // check for intersection of AABB's

        // in any axis, if absolute value of difference of center coordinate values is
        // greater than the sum of half-dimensions (i.e., radius, half_size),
        // no intersection, so proceed to next child
        if (fabsf(ball.position.x - child.position.x) > ball.radius + child.half_size ||
            fabsf(ball.position.y - child.position.y) > ball.radius + child.half_size)
            continue;

        // else, there is an intersection, so
        // check for collision between this ball and all the balls contained in that child

        // if this child contains no balls, proceed to next child
        if (child.containedBalls.size() == 0) continue;

        for (int j = 0; j < child.containedBalls.size()-1; j++){
            Ball& circleB = *(child.containedBalls[j]);

            Vector2 collisionVector = Vector2Subtract(ball.position, circleB.position);
            float cvMagnitude = Vector2Length(collisionVector);
            Vector2 velocityRel = Vector2Subtract(ball.velocity, circleB.velocity);   
            float dotProduct = Vector2DotProduct(collisionVector, velocityRel);

            // Collision response
            if(dotProduct < 0  && cvMagnitude <= (ball.radius + circleB.radius)){
                float iNum = (1 + ELASTICITY) * dotProduct;
                float iDenom = pow(cvMagnitude, 2)
                    * (ball.inverse_mass + circleB.inverse_mass);
                float impulse = -(iNum/iDenom);

                ball.velocity = Vector2Add(ball.velocity, 
                    Vector2Scale(collisionVector, impulse/ball.mass) );
                
                circleB.velocity = Vector2Subtract(circleB.velocity,
                    Vector2Scale(collisionVector, impulse/circleB.mass) );
            }
        }

        // handle collision with the descendants of this child
        handle_collisions_with_descendants(ball, child);
    }
}