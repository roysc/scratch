#include <string>
#include <vector>
// #include <random>
// #include <math>
#include <algorithm>
#include <array>
#include <iostream>

#include "component_space.hh"

struct Vec2
{
    using Number = uint;
    union {                                     
        std::array<Number, 2> v;                
        struct { Number x, y; };                
    };                                          
                                                
    using InputType = decltype(v);              
};

struct Position
{
    Vec2 m_;
    Vec2* const operator->() { return &m_; }
};

struct Velocity
{
    Vec2 m_;
    Vec2* const operator->() { return &m_; }
};

std::ostream& operator<<(std::ostream& out, Position v)
{ return out << "(" << v->x << ", " << v->y << ")"; }

std::ostream& operator<<(std::ostream& out, Velocity v)
{ return out << "(" << v->x << ", " << v->y << ")"; }

struct Motion
    : LogicSystem<Position, Velocity>
{
    void update(Position& p, Velocity& v)
    {
        p->x += v->x;
        p->y += v->y;
    }
};

struct NullSource
{
    template <class InputType>
    InputType next()
    {
        return InputType();
    }
};

    
int main(int argc, char *argv[]) {

    int n_ent =
        // argc > 1 ?
        // std::stoi(argv[0]) :
        100;

    using CIx = ComponentIndex<
        Position,
        Velocity
        // Hint,
        >;
    
    using CSpace = ComponentSpace<Position, Velocity>;

    NullSource null_src;
    CSpace space;

    std::vector<Entity<CIx> > ents;

    for (int i = 0; i < 10; i++) 
        ents.push_back(space.template create_entity<Position, Velocity>());
    
    for (auto& ent : ents) {
        std::cout << *ent.get_component<Position>() << ", "
                  << *ent.get_component<Velocity>() << "\n";
        

    }
    
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
