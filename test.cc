#include <string>
#include <vector>
#include <array>

#include <cassert>

#include "util.hh"
#include "entity_space.hh"
#include "system.hh"

using namespace util::io;

struct Vec2 : public BasicComponent
{
    using Number = int;
    using Array = std::array<Number, 2>;
    union {                                     
        Array v;
        struct { Number x, y; };                
    };                                          
                                                
    Vec2(Array a) : v(a) {}
    Vec2(Number _x, Number _y) : v {{_x, _y}} {}
    Vec2() : v {{0, 0}} {}
    
    using InputType = Array;

};

#define EXTEND_Vec2(DerivedName)                                 \
    struct DerivedName : public Vec2                             \
    {                                                            \
        static std::string name() { return #DerivedName; }       \
        std::string to_string() const                            \
        {                                                        \
            std::stringstream out;                               \
            print_to(out, #DerivedName "(", x, ", ", y, ")");    \
            return out.str();                                    \
        }                                                        \
                                                                 \
        using Vec2::Vec2;                                        \
    }

EXTEND_Vec2(Position);
EXTEND_Vec2(Velocity);
EXTEND_Vec2(Acceleration);

struct Motion : public Logic<Position, Velocity>
{
    void operate(Position& p, Velocity& v)
    {
        // Logic<Position, Velocity>::operate(p, v);
            
        // println("Motion operating on (", p, ", ", v, ")");
        p.x += v.x;
        p.y += v.y;
    }
};

struct Motion2 : public Logic<Velocity, Acceleration>
{
    void operate(Velocity& v, Acceleration& a)
    {
        // println("Motion operating on (", p, ", ", v, ")");
        v.x += a.x;
        v.y += a.y;
    }
};

    
int main(int argc, char* argv[]) {

    int n_updates = argc > 1 ? std::stoi(argv[1]) : 10,
        n_entities = argc > 2 ? std::stoi(argv[2]) : 1;

    using Space = EntitySpace<Position, Velocity, Acceleration>;
    // Space space;
    
    using System = System<Space, Motion2, Motion>;
    System sys;

    std::vector<EntityID> ents;

    for (int i = 0; i < n_entities; ++i) {
        ents.push_back(sys.template create_entity<Position>());
        ents.push_back(sys.create_entity(Position (1,1)));
        ents.push_back(sys.template create_entity<Velocity>());
        // va.push_back(sys.create_entity(Velocity()));
        // pv.push_back(sys.template create_entity<Position, Velocity>());
        ents.push_back(sys.create_entity(
                          Position(),
                          Velocity(1,-1)));
        ents.push_back(sys.create_entity(
                          Position(),
                          Velocity(),
                          Acceleration(-1, 1)));
    }
    
    // for (auto ent : v) println(sys.space[ent]);

    auto show = [&] (std::string s) {
        println(s);
        for (auto ent : ents)
            println("@", ent, ": ", sys.space[ent]);
    };

    show("  initial:");
    for (int i = 0; i < n_updates; ++i) sys.update();
    show(text("  after ", n_updates, " updates:"));

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
