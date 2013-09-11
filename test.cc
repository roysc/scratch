#include <string>
#include <vector>
// #include <random>
// #include <math>
#include <algorithm>
#include <array>
#include <iostream>
#include <bitset>

#include <cassert>

#include "util.hh"
#include "entity_space.hh"
#include "system.hh"

using namespace util::io;

struct Vec2 : public BasicComponent
{
    using Number = uint;
    union {                                     
        std::array<Number, 2> v;
        struct { Number x, y; };                
    };                                          
                                                
    Vec2() : v {{0, 0}} {}                
    using InputType = decltype(v);
};


struct Position : public Vec2, public BasicComponent
{
    static std::string name() { return "Position"; }
    std::string to_string() const
    {
        std::stringstream out;
        print_to(out, name(), "(", x, ", ", y, ")");
        return out.str();
    }
};


struct Velocity : public Vec2, public BasicComponent
{
    static std::string name() { return "Velocity"; }
    std::string to_string() const
    {
        std::stringstream out;
        print_to(out, "Velocity(", x, ", ", y, ")");
        return out.str();
    }
};

template <class EntitySpace>//, class... Components>
struct Motion
{
    static std::bitset<EntitySpace::n_components> mask;

    Motion(EntitySpace& space)
    {
        
    }
    
    void update(Position& p, Velocity v)
    {
        p.x += v.x;
        p.y += v.y;
    }
};

    
int main(int argc, char *argv[]) {
    
    using Space = EntitySpace<Position, Velocity>;
    Space space;

    std::vector<EntityID> entsp, entsv, entspv;
    
    for (int i = 0; i < 5; i++) {
        entsp.push_back(space.template create_entity<Position>());
        entsv.push_back(space.template create_entity<Velocity>());
        entspv.push_back(space.template create_entity<Position, Velocity>());
    }
    
    for (auto ent : entsp) {
        println(space[ent]);
        std::bitset<2> b("11");
        assert(space[ent].supports(b));
    }
    for (auto ent : entsv) println(space[ent]);
    for (auto ent : entspv) println(space[ent]);
    
    
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
