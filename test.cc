#include <string>
#include <vector>
// #include <random>
// #include <math>
#include <algorithm>
#include <array>
#include <iostream>

#include "entity_space.hh"

struct Vec2
{
    using Number = uint;
    union {                                     
        std::array<Number, 2> v;                
        struct { Number x, y; };                
    };                                          
                                                
    // using InputType = decltype(v);              
};

struct Position
{
    Vec2 m_;
    Vec2* const operator->() { return &m_; }
};

std::ostream& operator<<(std::ostream& out, Position v)
{ return out << "Position(" << v->x << ", " << v->y << ")"; }

struct Velocity
{
    Vec2 m_;
    Vec2* const operator->() { return &m_; }
};

std::ostream& operator<<(std::ostream& out, Velocity v)
{ return out << "Velocity(" << v->x << ", " << v->y << ")"; }

struct Motion
    : LogicSystem<Position, Velocity>
{
    void update(Position& p, Velocity v)
    {
        p->x += v->x;
        p->y += v->y;
    }
};

    
int main(int argc, char *argv[]) {

    int n_ent =
        // argc > 1 ?
        // std::stoi(argv[0]) :
        100;
    
    using Space = EntitySpace<Position, Velocity>;
    Space space;

    std::vector<EntityID> entsp, entsv, entspv;
    // std::vector<Entity<ComponentIndex<Position> > > entsp;
    
    for (int i = 0; i < 5; i++) {
        entsp.push_back(space.template create_entity<Position>());
        entsv.push_back(space.template create_entity<Velocity>());
        entspv.push_back(space.template create_entity<Position, Velocity>());
    }
    
    for (auto ent : entsp) std::cout << space[ent] << "\n";
    for (auto ent : entsv) std::cout << space[ent] << "\n";
    for (auto ent : entspv) std::cout << space[ent] << "\n";
    
    
    // using Logic = LogicIndex<Motion>;

    // using Game = GameSystem<CSpace, Logic>;
    // Game game;
    
    // game.enqueue(
    //     CSpace::Spawn<RandomInput>(1),
    //     CSpace::Spawn<Position>(n_ent, )
    // );

    // TextUI ui;
    // game.attach_UI(ui);
    // // 100 "clock ticks"
    // // want this to spawn its own thread.
    // game.go(100);
    // ui.show();

    return 0;
}
