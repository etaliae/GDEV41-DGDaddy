#include <raymath.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "entt.hpp"
#include "components.hpp"

const float FPS = 60;
const float TIMESTEP = 1/FPS;
const float FRICTION = 1.5f;
const float e = 0.25f;

const float GRID_SIZE = 50;
const float radius = 16.0f;
const float item_radius = 12.0f;
const float interact_range = GRID_SIZE * 1.5f;

float brew_time = 15.0f;
float eat_time = 15.0f;

float score = 0;

// TEXTURES
Texture bean;
Texture hot_coffee;
Texture coffee_tools;
Texture iced_coffee;


entt::registry registry;
entt::entity player;

std::map< std::pair<std::string, std::string>, std::string > combine =
{
    { std::make_pair("empty", "water"),         "water"},
    { std::make_pair("espresso", "hot water"),  "americano"},
    { std::make_pair("espresso", "milk"),       "cappuccino"}
};

std::map<std::string, int> price =
{
    {"water", 5},
    {"espresso", 8},
    {"americano", 10},
    {"cappucino", 12},
};

std::string drinks[] = {"water", "espresso"};

void init_textures()
{
    bean = ResourceManager::GetInstance()->GetTexture("bean.png");
    hot_coffee = ResourceManager::GetInstance()->GetTexture("hot_coffee.png");
    iced_coffee = ResourceManager::GetInstance()->GetTexture("iced_coffe.png");
    coffee_tools = ResourceManager::GetInstance()->GetTexture("coffee_tools.png");
}

void init_entities()
{
    // player
    player = registry.create();
    registry.emplace<CircleComponent>(player, radius);
    registry.emplace<PositionComponent>(player, Vector2{8.5f * GRID_SIZE, 7.5f * GRID_SIZE});
    registry.emplace<MoveComponent>(player, Vector2Zero());
    registry.emplace<AccelerationComponent>(player, Vector2Zero());
    registry.emplace<PhysicsComponent>(player, 1.0f, 1 / 1.0f);
    registry.emplace<DirectionComponent>(player, Vector2{0.0f, 1.0f});
    registry.emplace<InteractorComponent>(player, entt::null);
    registry.emplace<HolderComponent>(player, entt::null);


//FOR TESTING
    // customer
    entt::entity customer = registry.create();
    registry.emplace<CircleComponent>(customer, radius);
    registry.emplace<PositionComponent>(customer, Vector2{7.5f * GRID_SIZE, 2.5f * GRID_SIZE});
    registry.emplace<MoveComponent>(customer, Vector2Zero());
    registry.emplace<DirectionComponent>(customer, Vector2{0.0f, 1.0f});
    registry.emplace<InteractableComponent>(customer, true, false);
    registry.emplace<TimerComponent>(customer, 0.0f);

    int index = GetRandomValue(0, 1);
    registry.emplace<CustomerComponent>(customer, 100.0f, "Ordering", drinks[index], entt::null, entt::null);

    // counters
    entt::entity counter1 = registry.create();
    registry.emplace<SquareComponent>(counter1, GRID_SIZE / 2.0f);
    registry.emplace<PositionComponent>(counter1, Vector2{4.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<PhysicsComponent>(counter1, 1.0f, 0.0f);
    registry.emplace<InteractableComponent>(counter1, false, false);
    registry.emplace<TableComponent>(counter1, true);

    entt::entity counter2 = registry.create();
    registry.emplace<SquareComponent>(counter2, GRID_SIZE / 2.0f);
    registry.emplace<PositionComponent>(counter2, Vector2{5.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<PhysicsComponent>(counter2, 1.0f, 0.0f);
    registry.emplace<InteractableComponent>(counter2, false, false);
    registry.emplace<TableComponent>(counter2, true);

    entt::entity counter3 = registry.create();
    registry.emplace<SquareComponent>(counter3, GRID_SIZE / 2.0f);
    registry.emplace<PositionComponent>(counter3, Vector2{6.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<PhysicsComponent>(counter3, 1.0f, 0.0f);
    registry.emplace<InteractableComponent>(counter3, false, false);
    registry.emplace<TableComponent>(counter3, true);

    entt::entity counter4 = registry.create();
    registry.emplace<SquareComponent>(counter4, GRID_SIZE / 2.0f);
    registry.emplace<PositionComponent>(counter4, Vector2{7.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<PhysicsComponent>(counter4, 1.0f, 0.0f);
    registry.emplace<InteractableComponent>(counter4, false, false);
    registry.emplace<TableComponent>(counter4, true);

    entt::entity counter5 = registry.create();
    registry.emplace<SquareComponent>(counter5, GRID_SIZE / 2.0f);
    registry.emplace<PositionComponent>(counter5, Vector2{9.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<PhysicsComponent>(counter5, 1.0f, 0.0f);
    registry.emplace<InteractableComponent>(counter5, true, false);
    registry.emplace<TableComponent>(counter5, true);

    entt::entity counter6 = registry.create();
    registry.emplace<SquareComponent>(counter6, GRID_SIZE / 2.0f);
    registry.emplace<PositionComponent>(counter6, Vector2{10.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<PhysicsComponent>(counter6, 1.0f, 0.0f);
    registry.emplace<InteractableComponent>(counter6, false, false);
    registry.emplace<TableComponent>(counter6, true);

    entt::entity counter7 = registry.create();
    registry.emplace<SquareComponent>(counter7, GRID_SIZE / 2.0f);
    registry.emplace<PositionComponent>(counter7, Vector2{11.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<PhysicsComponent>(counter7, 1.0f, 0.0f);
    registry.emplace<InteractableComponent>(counter7, false, false);
    registry.emplace<TableComponent>(counter7, true);

    // customer-side obstacles
    entt::entity chair1 = registry.create();
    registry.emplace<SquareComponent>(chair1, GRID_SIZE / 4.0f);
    registry.emplace<PositionComponent>(chair1, Vector2{7.5f * GRID_SIZE, 2.5f * GRID_SIZE});
    registry.emplace<PhysicsComponent>(chair1, 1.0f, 0.0f);
    registry.emplace<ChairComponent>(chair1, customer);

    entt::entity dining_table = registry.create();
    registry.emplace<SquareComponent>(dining_table, GRID_SIZE / 2.0f);
    registry.emplace<PositionComponent>(dining_table, Vector2{8.5f * GRID_SIZE, 2.5f * GRID_SIZE});
    registry.emplace<PhysicsComponent>(dining_table, 1.0f, 0.0f);
    registry.emplace<InteractableComponent>(dining_table, true, false);
    registry.emplace<TableComponent>(dining_table, false);
    registry.emplace<DiningTableComponent>(dining_table, chair1);

    CustomerComponent& c = registry.get<CustomerComponent>(customer);
    c.table = dining_table;

    // item
    entt::entity stack_of_cups = registry.create();
    registry.emplace<PositionComponent>(stack_of_cups, Vector2{4.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<InteractableComponent>(stack_of_cups, true, false);
    registry.emplace<StackComponent>(stack_of_cups, "cup");

    entt::entity coffee_machine = registry.create();
    registry.emplace<PositionComponent>(coffee_machine, Vector2{5.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<InteractableComponent>(coffee_machine, true, false);
    registry.emplace<TableComponent>(coffee_machine, false);
    registry.emplace<CoffeeMachineComponent>(coffee_machine, false, false, entt::null);
    registry.emplace<TimerComponent>(coffee_machine, 0.0f);

    entt::entity coffee_bean_container = registry.create();
    registry.emplace<PositionComponent>(coffee_bean_container, Vector2{6.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<InteractableComponent>(coffee_bean_container, true, false);
    registry.emplace<StackComponent>(coffee_bean_container, "ingredient");
    registry.emplace<IngredientComponent>(coffee_bean_container, "coffee bean", false);

    entt::entity water_pitcher = registry.create();
    registry.emplace<PositionComponent>(water_pitcher, Vector2{7.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<InteractableComponent>(water_pitcher, true, false);
    registry.emplace<HoldableComponent>(water_pitcher, false);
    registry.emplace<PlaceableComponent>(water_pitcher, counter4);
    registry.emplace<IngredientComponent>(water_pitcher, "water", true);

    entt::entity kettle = registry.create();
    registry.emplace<PositionComponent>(kettle, Vector2{10.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<InteractableComponent>(kettle, true, false);
    registry.emplace<HoldableComponent>(kettle, false);
    registry.emplace<PlaceableComponent>(kettle, counter6);
    registry.emplace<IngredientComponent>(kettle, "hot water", true);

    entt::entity milk_jug = registry.create();
    registry.emplace<PositionComponent>(milk_jug, Vector2{11.5f * GRID_SIZE, 6.5f * GRID_SIZE});
    registry.emplace<InteractableComponent>(milk_jug, true, false);
    registry.emplace<HoldableComponent>(milk_jug, false);
    registry.emplace<PlaceableComponent>(milk_jug, counter7);
    registry.emplace<IngredientComponent>(milk_jug, "milk", true);
}

void read_player_input()
{
    //MOVEMENT
    Vector2 forces = Vector2Zero(); // every frame set the forces to a 0 vector

    // Adds forces with the magnitude of 100 in the direction given by WASD inputs
    if(IsKeyDown(KEY_W)) {
        forces = Vector2Add(forces, {0, -100});
    }
    if(IsKeyDown(KEY_A)) {
        forces = Vector2Add(forces, {-100, 0});
    }
    if(IsKeyDown(KEY_S)) {
        forces = Vector2Add(forces, {0, 100});
    }
    if(IsKeyDown(KEY_D)) {
        forces = Vector2Add(forces, {100, 0});
    }

    AccelerationComponent& a = registry.get<AccelerationComponent>(player);
    PhysicsComponent& p1_phy = registry.get<PhysicsComponent>(player);
    // Does Vector - Scalar multiplication with the sum of all forces and the inverse mass of the ball
    a.acceleration = Vector2Scale(forces, p1_phy.inverse_mass);

    if (Vector2Length(forces) > 0)
    {
        DirectionComponent& dir = registry.get<DirectionComponent>(player);
        dir.forward = Vector2Normalize(forces);
    }

    //INTERACT
    InteractorComponent& interactor = registry.get<InteractorComponent>(player);

    if(IsKeyPressed(KEY_X) && interactor.hot_item != entt::null)
    {
        MoneyComponent* payment = registry.try_get<MoneyComponent>(interactor.hot_item);
        if (payment)
        {
            // add payment to score
            score += payment->amount;

            // update table's status
            PlaceableComponent& placeable = registry.get<PlaceableComponent>(interactor.hot_item);
            TableComponent& table = registry.get<TableComponent>(placeable.table);
            table.hasItemOnTop = false;

            InteractableComponent& i = registry.get<InteractableComponent>(placeable.table);
            i.isEnabled = true;

            // update placeable's "table" to null
            placeable.table = entt::null;

            // destroy money object
            registry.destroy(interactor.hot_item);
            
            // set hot item to null
            interactor.hot_item = entt::null;
            
            return;
        }

        HolderComponent& holder = registry.get<HolderComponent>(player);

        // if there is no held item
        if (holder.held_item == entt::null)
        {
            HoldableComponent* holdable = registry.try_get<HoldableComponent>(interactor.hot_item);

            // if hot item is holdable
            if (holdable)
            {
                // set held item to hot item
                holder.held_item = interactor.hot_item;
                holdable->isHeld = true;

                // make held item not interactable and not hot
                InteractableComponent& item = registry.get<InteractableComponent>(interactor.hot_item);
                item.isEnabled = false;
                item.isHot = false;

                // update table's status
                PlaceableComponent& placeable = registry.get<PlaceableComponent>(interactor.hot_item);
                TableComponent& table = registry.get<TableComponent>(placeable.table);
                table.hasItemOnTop = false;

                InteractableComponent& i = registry.get<InteractableComponent>(placeable.table);
                i.isEnabled = true;

                // update placeable's "table" to null
                placeable.table = entt::null;

                // set hot item to null
                interactor.hot_item = entt::null;

                DrinkComponent* drink = registry.try_get<DrinkComponent>(holder.held_item);
                if (drink)
                    std::cout << "Got " << drink->name << "\n";
                else
                {
                    IngredientComponent* ingredient = registry.try_get<IngredientComponent>(holder.held_item);
                    if (ingredient)
                        std::cout << "Got " << ingredient->name << "\n";
                }
                
                return;
            }

            StackComponent* stack = registry.try_get<StackComponent>(interactor.hot_item);

            // else if hot item is stack
            if (stack)
            {
                // create a new entity (an object from the stack)
                entt::entity new_entity = registry.create();

                registry.emplace<PositionComponent>(new_entity, Vector2Zero());     // position doesnt matter if held
                registry.emplace<InteractableComponent>(new_entity, false, false);  // not enabled, not hot
                registry.emplace<HoldableComponent>(new_entity, true);              // is held
                registry.emplace<PlaceableComponent>(new_entity, entt::null);       // not placed on anything

                if (stack->type == "cup")
                {
                    registry.emplace<DrinkComponent>(new_entity, "empty");

                    registry.emplace<SpriteComponent>(new_entity, hot_coffee,
                                                        std::vector<Rectangle>{
                                                            {112,0,128,16}
                                                        }, 0);

                    std::cout << "Got empty cup\n"; 
                }
                else if (stack->type == "ingredient")
                {
                    IngredientComponent& ingredient = registry.get<IngredientComponent>(interactor.hot_item);
                    registry.emplace<IngredientComponent>(new_entity, ingredient.name);

                    registry.emplace<SpriteComponent>(new_entity, bean,
                                                        std::vector<Rectangle>{
                                                            {0,0,16,16}
                                                        }, 0);

                    std::cout << "Got " << ingredient.name << "\n";
                }

                // set held item to new entity
                holder.held_item = new_entity;

                // set hot item to null
                interactor.hot_item = entt::null;
                
                return;
            }
        }

        // else if there is a held item
        else
        {
            HolderComponent& holder = registry.get<HolderComponent>(player);
            CoffeeMachineComponent* machine = registry.try_get<CoffeeMachineComponent>(interactor.hot_item);

            // if hot item is a coffee machine
            if (machine)
            {
                IngredientComponent* ingredient = registry.try_get<IngredientComponent>(holder.held_item);

                // if holding an ingredient
                if (ingredient)
                {
                    // if holding coffee bean / grounds and machine has no coffee yet
                    if (ingredient->name == "coffee bean" && !machine->hasCoffeeGrounds)
                    {
                        // fill machine with coffee
                        machine->hasCoffeeGrounds = true;

                        // destroy entity
                        registry.destroy(holder.held_item);

                        // remove it from the hands of holder
                        holder.held_item = entt::null;

                        std::cout << "Filled machine with coffee grounds\n";
                    }

                    // else if holding water pitcher and machine has no water yet
                    else if (ingredient->name == "water" && !machine->hasWater)
                    {
                        // fill machine with water
                        machine->hasWater = true;

                        std::cout << "Filled machine with water\n";
                    }
                }
                else
                {
                    DrinkComponent* drink = registry.try_get<DrinkComponent>(holder.held_item);

                    // if held item is an empty cup, and the machine has no cup yet
                    if (drink && drink->name == "empty" && machine->drink == entt::null)
                    {
                        // set cup on coffee machine
                        PositionComponent& machine_pos = registry.get<PositionComponent>(interactor.hot_item);
                        PositionComponent& cup_pos = registry.get<PositionComponent>(holder.held_item);
                        cup_pos.position = Vector2Add(machine_pos.position, {0.0f, GRID_SIZE * 0.15f});

                        PlaceableComponent& placeable = registry.get<PlaceableComponent>(holder.held_item);
                        placeable.table = interactor.hot_item;

                        machine->drink = holder.held_item;

                        // keep cup not interactable, coffee machine interactable

                        // remove cup from hands of holder
                        HoldableComponent& holdable = registry.get<HoldableComponent>(holder.held_item);
                        holdable.isHeld = false;

                        holder.held_item = entt::null;

                        std::cout << "Placed cup in machine\n";
                    }
                }

                TimerComponent& timer = registry.get<TimerComponent>(interactor.hot_item);

                // if timer has not been set, and coffee machine is all set up
                if (FloatEquals(timer.time, 0.0f) &&
                    machine->hasCoffeeGrounds && machine->hasWater && machine->drink != entt::null)
                {
                    // disable interactions with machine
                    InteractableComponent& i = registry.get<InteractableComponent>(interactor.hot_item);
                    i.isEnabled = false;
                    i.isHot = false;

                    // remove coffee grounds and water
                    machine->hasCoffeeGrounds = false;
                    machine->hasWater = false;

                    // set timer
                    timer.time = brew_time;

                    std::cout << "Activated coffee machine for " << timer.time << " seconds\n";
                }

                // set hot item to null
                interactor.hot_item = entt::null;

                return;
            }

            CustomerComponent* customer = registry.try_get<CustomerComponent>(interactor.hot_item);

            // else if the hot item is a customer
            if (customer)
            {
                DrinkComponent* drink = registry.try_get<DrinkComponent>(holder.held_item);

                // if holding drink and drink is the customer's order
                if (drink && drink->name == customer->order)
                {
                    // make customer not interactable
                    InteractableComponent& i = registry.get<InteractableComponent>(interactor.hot_item);
                    i.isEnabled = false;
                    i.isHot = false;

                    // put drink on customer
                    PositionComponent& drink_pos = registry.get<PositionComponent>(holder.held_item);
                    PositionComponent& customer_pos = registry.get<PositionComponent>(interactor.hot_item);
                    drink_pos.position = Vector2Add(customer_pos.position, {radius / 1.5f, radius / 1.5f});

                    customer->drink = holder.held_item;

                    // let customer eat
                    customer->state = "Eating";

                    TimerComponent& timer = registry.get<TimerComponent>(interactor.hot_item);
                    timer.time = eat_time;

                    // remove item from hands of holder
                    HoldableComponent& holdable = registry.get<HoldableComponent>(holder.held_item);
                    holdable.isHeld = false;

                    holder.held_item = entt::null;

                    // set hot item to null
                    interactor.hot_item = entt::null;

                    return;
                }
            }

            TableComponent* table = registry.try_get<TableComponent>(interactor.hot_item);

            // else if the hot item is a table,
            if (table)
            {
                table->hasItemOnTop = true;

                // make table not interactable
                InteractableComponent& i = registry.get<InteractableComponent>(interactor.hot_item);
                i.isEnabled = false;
                i.isHot = false;

                // set held item on top of table
                PositionComponent& table_pos = registry.get<PositionComponent>(interactor.hot_item);
                PositionComponent& item_pos = registry.get<PositionComponent>(holder.held_item);
                item_pos.position = table_pos.position;

                PlaceableComponent& placeable = registry.get<PlaceableComponent>(holder.held_item);
                placeable.table = interactor.hot_item;

                // make item interactable
                InteractableComponent& item = registry.get<InteractableComponent>(holder.held_item);
                item.isEnabled = true;

                HoldableComponent& holdable = registry.get<HoldableComponent>(holder.held_item);
                holdable.isHeld = false;

                // remove item from hands of holder
                holder.held_item = entt::null;

                // set hot item to null
                interactor.hot_item = entt::null;

                return;
            }

            DrinkComponent* drink = registry.try_get<DrinkComponent>(interactor.hot_item);

            // else if hot item is a drink
            if (drink)
            {
                IngredientComponent* ingredient = registry.try_get<IngredientComponent>(holder.held_item);
                
                // if holding an ingredient (inside a pitcher), and
                // if the combination of the drink and ingredient is valid / is in the map data structure
                if ( ingredient && ingredient->isPitcher && combine.find( std::make_pair(drink->name, ingredient->name) ) != combine.end() )
                {
                    std::cout << "Combined " << drink->name << " and " << ingredient->name;

                    // combine ingredient with drink
                    drink->name = combine[std::make_pair(drink->name, ingredient->name)];

                    std::cout << " into " << drink->name << "\n";

                    // set hot item to null
                    interactor.hot_item = entt::null;

                    return;
                }
            }
        }
    }
}

void update_customers()
{
    auto customers = registry.view<CustomerComponent>();
    for (auto entity : customers)
    {
        CustomerComponent& customer = registry.get<CustomerComponent>(entity);

        if (customer.state == "Ordering")
        {
            customer.patience -= TIMESTEP;
        }
    }
}

void affect_velocities()
{
    // make acceleration and friction affect velocity
    auto affect_velocity = registry.view<AccelerationComponent, PhysicsComponent>();
    for (auto entity : affect_velocity)
    {
        AccelerationComponent& a = registry.get<AccelerationComponent>(entity);
        PhysicsComponent& phy = registry.get<PhysicsComponent>(entity);
        MoveComponent& m = registry.get<MoveComponent>(entity);
        
        m.velocity = Vector2Add(m.velocity, Vector2Scale(a.acceleration, TIMESTEP));
        m.velocity = Vector2Subtract(m.velocity, Vector2Scale(m.velocity, FRICTION * phy.inverse_mass * TIMESTEP));
    }
}

void move_entities()
{
    auto move = registry.view<MoveComponent>();
    for (auto entity : move)
    {
        MoveComponent& m = registry.get<MoveComponent>(entity);
        PositionComponent& pos = registry.get<PositionComponent>(entity);

        pos.position = Vector2Add(pos.position, Vector2Scale(m.velocity, TIMESTEP));
    }
}

void circle_rectangle_collision(entt::entity& circle, entt::entity& rectangle)
{
    // circle components
    PhysicsComponent& c_phy = registry.get<PhysicsComponent>(circle);
    PositionComponent& c_pos = registry.get<PositionComponent>(circle);
    CircleComponent& c_rad = registry.get<CircleComponent>(circle);

    // rectangle components
    PhysicsComponent& r_phy = registry.get<PhysicsComponent>(rectangle);
    PositionComponent& r_pos = registry.get<PositionComponent>(rectangle);
    SquareComponent& r_size = registry.get<SquareComponent>(rectangle);


    // collision proper
    
    // get the point on the border that is closest to the ball
    Vector2 closestPoint = {
        Clamp(c_pos.position.x, r_pos.position.x - r_size.half_size, r_pos.position.x + r_size.half_size),
        Clamp(c_pos.position.y, r_pos.position.y - r_size.half_size, r_pos.position.y + r_size.half_size)
    };

    Vector2 collisionVector = Vector2Subtract(c_pos.position, closestPoint);
    float cvMagnitude = Vector2Length(collisionVector);

    // if distance between closest point and the ball is greater than the ball's radius,
    // no collision
    if (cvMagnitude > c_rad.radius) return;

    MoveComponent* c_m = registry.try_get<MoveComponent>(circle);
    MoveComponent* r_m = registry.try_get<MoveComponent>(rectangle);

    Vector2 velocityRel = Vector2Zero();

    // guaranteed that at least one of circle and rectangle is moving
    // (assuming it is not the circle moving)

    // if circle is moving, temporarily set relative velocity to circle's velocity
    if (c_m)
        velocityRel = c_m->velocity;

    // if rectangle is moving, subtract its velocity from vector stored in relative velocity
    if (r_m)
        velocityRel = Vector2Subtract(velocityRel, r_m->velocity);

    float dotProduct = Vector2DotProduct(collisionVector, velocityRel);

    // if collision normal and relative velocity are towards roughly the same direction, no collision
    if (dotProduct >= 0) return;

    float iNum = (1 + e) * dotProduct;
    float iDenom = pow(cvMagnitude, 2)
        * (c_phy.inverse_mass + r_phy.inverse_mass);
    float impulse = -(iNum/iDenom);

    if (c_m)
    {
        c_m->velocity = Vector2Add(c_m->velocity, 
            Vector2Scale(collisionVector, impulse/c_phy.mass) );
    }

    if (r_m)
    {
        r_m->velocity = Vector2Subtract(r_m->velocity, 
            Vector2Scale(collisionVector, impulse/r_phy.mass) );
    }
}

void handle_collisions()
{
    // moving circle colliding with squares
    auto moving_physics = registry.view<PhysicsComponent, MoveComponent>();
    auto physics = registry.view<PhysicsComponent>();

    for (auto e1 : moving_physics)
    {
        // check for collisions with other physics objects
        for (auto e2 : physics)
        {
            // ignore if it is the same physics object
            if (e1 == e2)
                continue;

            circle_rectangle_collision(e1, e2);
        }
    }
}

void get_hot_items()
{
    auto interactors = registry.view<InteractorComponent>();
    for (auto e : interactors)
    {
        InteractorComponent& interactor = registry.get<InteractorComponent>(e);

        // if there was a previous hot item, reset its status
        if (interactor.hot_item != entt::null)
        {
            InteractableComponent& i = registry.get<InteractableComponent>(interactor.hot_item);
            i.isHot = false;
            i.isEnabled = true;

            interactor.hot_item = entt::null;
        }

        float highest_dot = 0; // highest dot product = closest to forward direction of interactor
        float minDistance = -1;

        PositionComponent& pos = registry.get<PositionComponent>(e);
        DirectionComponent& dir = registry.get<DirectionComponent>(e);

        auto interactable = registry.view<InteractableComponent>();
        for (auto entity : interactable)
        {
            InteractableComponent& i = registry.get<InteractableComponent>(entity);
            if (!i.isEnabled) continue;

            PositionComponent& item_pos = registry.get<PositionComponent>(entity);
            Vector2 interactor_to_item = Vector2Subtract(item_pos.position, pos.position);
            float distance = Vector2Length(interactor_to_item);

            // if item is within range of interactor,
            if (distance <= interact_range)
            {
                float dotProduct = Vector2DotProduct(dir.forward, Vector2Normalize(interactor_to_item));
                
                // if item is within 90 degrees of interactor's fov and has higher dot product than the last hot item
                // (no need to check for > 0 since initial value of highest_dot is 0)
                if (dotProduct > highest_dot)
                {
                    highest_dot = dotProduct;
                    minDistance = distance;
                    interactor.hot_item = entity;
                }

                // else if this has the same dot product as the highest so far (greater than 0), the closer will be the hot item
                else if (dotProduct == highest_dot && dotProduct > 0 && distance < minDistance)
                {
                    minDistance = distance;
                    interactor.hot_item = entity;                
                }
            }
        }

        // if there is a new hot item, set it to hot
        if (interactor.hot_item != entt::null)
        {
            InteractableComponent& i = registry.get<InteractableComponent>(interactor.hot_item);
            i.isHot = true;
        }
    }
}

void update_timers()
{
    auto timer = registry.view<TimerComponent>();
    for (auto entity : timer)
    {
        TimerComponent& ent_timer = registry.get<TimerComponent>(entity);

        if (!FloatEquals(ent_timer.time, 0.0f))
        {
            ent_timer.time -= TIMESTEP;

            if (ent_timer.time <= 0.0f)
            {
                ent_timer.time = 0.0f;

                CoffeeMachineComponent* machine = registry.try_get<CoffeeMachineComponent>(entity);
                if (machine)
                {
                    // espresso has been made
                    DrinkComponent& drink = registry.get<DrinkComponent>(machine->drink);
                    drink.name = "espresso";

                    // make the drink interactable
                    InteractableComponent& drink_in = registry.get<InteractableComponent>(machine->drink);
                    drink_in.isEnabled = true;

                    // detach it from coffee machine setup
                    machine->drink = entt::null;

                    std::cout << "Espresso ready!\n";

                    continue;
                }

                CustomerComponent* customer = registry.try_get<CustomerComponent>(entity);
                if (customer)
                {
                    TableComponent& table = registry.get<TableComponent>(customer->table);
                    table.hasItemOnTop = true;

                    InteractableComponent& i = registry.get<InteractableComponent>(customer->table);
                    i.isEnabled = false;
                    i.isHot = false;

                    PositionComponent& table_pos = registry.get<PositionComponent>(customer->table);

                    // put payment on table
                    entt::entity payment = registry.create();
                    registry.emplace<PositionComponent>(payment, table_pos.position);
                    registry.emplace<InteractableComponent>(payment, true, false);
                    registry.emplace<MoneyComponent>(payment, price[customer->order] * (1.0f + customer->patience / 100.0f));
                    registry.emplace<PlaceableComponent>(payment, customer->table);

                    registry.emplace<SpriteComponent>(payment, coffee_tools,
                                                        std::vector<Rectangle>{
                                                            {32,0,16,16}
                                                        }, 0);
                    // destroy drink
                    registry.destroy(customer->drink);

                    // remove customer from chair
                    DiningTableComponent& dining_table = registry.get<DiningTableComponent>(customer->table);
                    ChairComponent& chair = registry.get<ChairComponent>(dining_table.chair1);
                    chair.customer = entt::null;

                    // destroy customer
                    registry.destroy(entity);
                }
            }
        }
    }
}

void draw_level()
{
    // with sprites, do: view<sprite, __> where __ is the type of thing it is
    // (e.g. floor, object, interactable, customer, player) or smth like that

    // level layout
    for (int i = 0; i < WINDOW_WIDTH / GRID_SIZE; i++)
    {
        for (int j = 0; j < WINDOW_HEIGHT / GRID_SIZE; j++)
        {
            Vector2 position = {i * GRID_SIZE, j * GRID_SIZE};

            DrawLineV(position, Vector2Add(position, {GRID_SIZE, 0.0f}), BLACK);
            DrawLineV(position, Vector2Add(position, {0.0f, GRID_SIZE}), BLACK);
            DrawLineV(Vector2Add(position, {GRID_SIZE, GRID_SIZE}), Vector2Add(position, {GRID_SIZE, 0.0f}), BLACK);
            DrawLineV(Vector2Add(position, {GRID_SIZE, GRID_SIZE}), Vector2Add(position, {0.0f, GRID_SIZE}), BLACK);
        }
    }

    // obstacles
    auto obstacle = registry.view<TableComponent>();
    for (auto entity : obstacle)
    {
        CoffeeMachineComponent* machine = registry.try_get<CoffeeMachineComponent>(entity);
        if (machine) continue;

        PositionComponent& p = registry.get<PositionComponent>(entity);
        InteractableComponent& item = registry.get<InteractableComponent>(entity);
        SquareComponent& square = registry.get<SquareComponent>(entity);

        Color color;
        if (item.isHot) color = BROWN;
        else color = DARKBROWN;

        DrawRectangleV(Vector2Subtract(p.position, {square.half_size, square.half_size}),
                        {square.half_size * 2.0f, square.half_size * 2.0f}, color);
    }

    auto chair = registry.view<ChairComponent>();
    for (auto entity : chair)
    {
        PositionComponent& p = registry.get<PositionComponent>(entity);
        SquareComponent& square = registry.get<SquareComponent>(entity);

        DrawRectangleV(Vector2Subtract(p.position, {square.half_size, square.half_size}),
                        {square.half_size * 2.0f, square.half_size * 2.0f}, BEIGE);
    }

    auto machine = registry.view<CoffeeMachineComponent>();
    for (auto entity : machine)
    {
        PositionComponent& p = registry.get<PositionComponent>(entity);
        InteractableComponent& i = registry.get<InteractableComponent>(entity);

        Color color;
        if (i.isHot) color = GRAY;
        else color = BLACK;

        DrawRectangleV(Vector2Subtract(p.position, {GRID_SIZE * 0.375f, GRID_SIZE * 0.375f}),
                        {GRID_SIZE * 0.75f, GRID_SIZE * 0.75f}, color);
    }

    // interactables
    auto interactable = registry.view<InteractableComponent>();
    for (auto entity : interactable)
    {
        CoffeeMachineComponent* machine = registry.try_get<CoffeeMachineComponent>(entity);
        if (machine) continue;

        CustomerComponent* customer = registry.try_get<CustomerComponent>(entity);
        if (customer) continue;

        PositionComponent& p = registry.get<PositionComponent>(entity);
        InteractableComponent& item = registry.get<InteractableComponent>(entity);

        SquareComponent* square = registry.try_get<SquareComponent>(entity);
        HoldableComponent* holdable = registry.try_get<HoldableComponent>(entity);

        // if it is not an obstacle and it is not being held
        if (!square && (!holdable || !holdable->isHeld))
        {
            SpriteComponent* sprite = registry.try_get<SpriteComponent>(entity);
            if (sprite)
            {
                Color color;
                if (item.isHot) color = GRAY;
                else color = WHITE;

                Rectangle rec = sprite->frames[sprite->frame_number];

                DrawTexturePro(sprite->sprite_sheet, sprite->frames[sprite->frame_number],
                                {p.position.x, p.position.y, int(item_radius), int(item_radius)},
                                {0, 0}, 0.0f, color);

                sprite->frame_number = (sprite->frame_number + 1) & sprite->frames.size();

                continue;
            }

            Color color;
            if (item.isHot) color = YELLOW;
            else color = WHITE;

            DrawCircleV(p.position, radius / 2.0f, color);
        }
    }

    // customers
    auto customer = registry.view<CustomerComponent>();
    for (auto entity : customer)
    {
        PositionComponent& pos = registry.get<PositionComponent>(entity);
        CircleComponent& rad = registry.get<CircleComponent>(entity);
        InteractableComponent& i = registry.get<InteractableComponent>(entity);

        if (i.isHot) DrawCircleV(pos.position, rad.radius, PURPLE);
        else DrawCircleV(pos.position, rad.radius, DARKPURPLE);
    }

    // player
    PositionComponent& pos = registry.get<PositionComponent>(player);
    CircleComponent& rad = registry.get<CircleComponent>(player);
    DrawCircleV(pos.position, rad.radius, BLUE);

    //[TEMP?] draw held item
    HolderComponent& holder = registry.get<HolderComponent>(player);
    if (holder.held_item != entt::null)
    {
        DrawCircleV(Vector2Add(pos.position, {rad.radius / 1.5f, rad.radius / 1.5f}),
                    rad.radius / 2.0f, WHITE);
    }

    // score
    DrawText(TextFormat("Score: %01f",score), 300, 30, 30, BLACK);
}