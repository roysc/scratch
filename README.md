##The "Scratch" engine.

An experimental game/simulation engine using an entity-component design. Also, an exercise in template metaprogramming.

The idea is to be able to specify components as type-parameters for an entity. To implement a component, an entity system just needs to include it in the template type-list:

```

struct Position { double x, y; };
struct Velocity { double x, y; };
...
struct Motion {
    void update(Position& p, Velocity& v) {
        p.x += v.x;  p.y += v.y;
    }
};

// Each type param is a component
using Particle = Entity<Position, Velocity, Mass>;
// Type params are the entity and systems acting on its components
using Space = System<Particle, Motion, KineticEnergy>;

Space space;
Entity* p = space.create_entity({0,0}, {1,1}, {5});
...
space.update();

```

