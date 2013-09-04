#include <string>
#include <vector>
// #include <random>
// #include <math>
#include <algorithm>
#include <array>
#include <iostream>

#include "component_space.hh"

#define MIXIN_VEC_2()                           \
    using Number = uint;                        \
    union {                                     \
        std::array<Number, 2> v;                \
        struct { Number x, y; };                \
    };                                          \
                                                \
    using InputType = decltype(v);              

// struct Vec2 : Component
// {
//     using Number = uint;
//     union {
//         std::array<Number, 2> v;
//         struct { Number x, y; };
//     };

//     using InputType = decltype(v);
    
//     // Vec2(InputType init) : v(init) {}
//     // Vec2(Number _x, Number _y) : x(_x), y(_y) {}
// };

struct Position : Component
{
    MIXIN_VEC_2()
};

struct Velocity : Component
{
    MIXIN_VEC_2()
};

struct Motion
    : LogicSystem<ComponentIndex<Position, Velocity> >
{
    void update(Position* p, Velocity* v)
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

    auto ent = space.template create_entity<CIx>();

    std::cout << ent.get_component<Position>()->x << ", "
              << ent.get_component<Position>()->y << "\n";
    
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
