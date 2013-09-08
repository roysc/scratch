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
                                                
    Vec2() : v {{0, 0}} {}                
    using InputType = decltype(v);
};

using String = std::string;


struct Position : public BasicComponent
{
    Vec2 m_;
    Vec2* const operator->() { return &m_; }

    static String name() { return "Position"; }
    std::string to_string() const
    {
        std::stringstream out;
        out << "Position(" << m_.x << ", " << m_.y << ")";
        return out.str();
    }
};


struct Velocity : public BasicComponent
{
    Vec2 m_;
    Vec2* const operator->() { return &m_; }

    std::string to_string() const
    {
        std::stringstream out;
        out << "Velocity(" << m_.x << ", " << m_.y << ")";
        return out.str();
    }
};


std::ostream& operator<<(std::ostream& out, Velocity v)
{ return out << "Velocity(" << v->x << ", " << v->y << ")"; }

struct Motion
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

    // std::cout << "Names: "
    //           << name<Position>() << ", "
    //           << name<Velocity>() << "\n";
    
    for (int i = 0; i < 5; i++) {
        entsp.push_back(space.template create_entity<Position>());
        entsv.push_back(space.template create_entity<Velocity>());
        entspv.push_back(space.template create_entity<Position, Velocity>());
    }
    
    for (auto ent : entsp) println(to_string(space[ent]));
    for (auto ent : entsv) println(to_string(space[ent]));
    for (auto ent : entspv) println(to_string(space[ent]));
    
    
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
