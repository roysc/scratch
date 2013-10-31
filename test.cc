#include <string>
#include <vector>
#include <array>
#include <random>
#include <cmath>

#include <cassert>

#include "util.hh"
#include "entity_space.hh"
#include "system.hh"

using namespace util::io;

struct Vec2// : public BasicComponent
{
    using Number = float;
    using Array = std::array<Number, 2>;
    union { Array v; struct { Number x, y; }; };                                          
                                                
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

EXTEND_Vec2(Pos);
EXTEND_Vec2(Vel);
EXTEND_Vec2(Accel);

struct Energy// : public BasicComponent
{
    float e;
    std::string to_string() const
    {
        return util::text("E(", e, ")");
    }
};

struct Mass
{
    float m;
    std::string to_string() const
    {
        return util::text("M(", m, ")");
    }
};

struct Motion : public Logic<Pos, Vel>
{
    void operate(Pos& p, Vel& v)
    {
        // Logic<Pos, Vel>::operate(p, v);
            
        // println("Motion operating on (", p, ", ", v, ")");
        p.x += v.x;
        p.y += v.y;
    }
};

struct Motion2 : public Logic<Vel, Accel>
{
    void operate(Vel& v, Accel& a)
    {
        // println("Motion operating on (", p, ", ", v, ")");
        v.x += a.x;
        v.y += a.y;
    }
};

struct KineticEnergy : public Logic<Vel, Mass, Energy>
{
    void operate(Vel& v, Mass& m, Energy& e)
    {
        e.e += sqrt(v.x * v.x + v.y * v.y) * m.m;
    }
};

    
int main(int argc, char* argv[]) {

    int n_updates = argc > 1 ? std::stoi(argv[1]) : 10,
        n_entities = argc > 2 ? std::stoi(argv[2]) : 1;

    using Space = EntitySpace<Pos, Vel, Accel, Mass, Energy>;
    // Space space;
    
    using System = System<Space, Motion2, Motion, KineticEnergy>;

    std::default_random_engine rng;
    std::uniform_int_distribution<int> di(-100, 100);
    std::uniform_real_distribution<float> df(0, 1);
    std::uniform_real_distribution<float> dfn(-1, 1);
    
    System sys;
    std::vector<EntityID> ents;

    for (int i = 0; i < n_entities; ++i) {
        // ents.push_back(sys.template create_entity<Pos>());
        // ents.push_back(sys.create_entity(Pos (1,1)));
        // ents.push_back(sys.template create_entity<Vel>());
        // va.push_back(sys.create_entity(Vel()));
        // pv.push_back(sys.template create_entity<Pos, Vel>());
        // ents.push_back(sys.create_entity(
        //                   Pos(),
        //                   Vel(1,-1)));
        //                   Vel(1,-1)));
        ents.push_back(sys.create_entity(
                           Pos(),
                           Vel(),
                           Accel(dfn(rng), dfn(rng)),
                           Energy {df(rng)},
                           Mass {df(rng)}
                           ));
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
    //     CSpace::Spawn<Pos>(n_ent, )
    // );

    // TextUI ui;
    // game.attach_UI(ui);
    // // 100 "clock ticks"
    // // want this to spawn its own thread.
    // game.go(100);
    // ui.show();

    return 0;
}
