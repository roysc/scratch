import std.typetuple;
import std.traits;

struct TypeVector(Ts...)
{
    import std.typecons;
    alias Tuple = std.typecons.Tuple!Ts;
    Tuple tuple;
    // alias tuple this;

    ref T get(T)() {
        enum idx = staticIndexOf!(T, Ts);

        import std.conv;
        static assert(idx != -1,
                      text("No ", T.stringof, " in ", Tuple.stringof));
        return tuple[idx];
    }
}

struct ComponentIndex(Cpts...)
{
    template AddPointer(T) { alias AddPointer = T*; }
    alias Components = Cpts;
    alias Pointers = TypeVector!(staticMap!(AddPointer, Cpts));
    
    Pointers contents;
    
    // alias Components this;

    ref Cpt* get(Cpt)()
    {
        return contents.get!(Cpt*);

        // enum idx = staticIndexOf!(Cpt*, Pointers);
        // static assert (idx != -1, "bu hao");
        // return contents[idx];
    }

    template supports(Cs...) {
        template supportsEach(C) {
            template predicate(C_)
            { enum bool predicate = is(C == C_); }
            
            enum bool supportsEach = anySatisfy!(predicate, Components);
        }

        enum bool supports = allSatisfy!(supportsEach, Cs);
    }
    
    // template supports(CIx : ComponentIndex!Cs, Cs...)
    template supports(CIx, Cs...) if (is(CIx : ComponentIndex!Cs))
    {
        static if (is(CIx == ComponentIndex!Cs, Cs...))
            enum bool supports = supports!Cs;
        else static assert(false);
    }
}

alias EntityID = ulong;


mixin template Vec2()
{
    union {
        uint[2] v;
        struct { uint x, y; }
    }
    alias InputType = typeof(v);

    string toString() {
        import std.conv;
        return text(__traits(identifier, typeof(this)),
                    "(", x, ", ", y, ")");
    }
}

struct Position
{
    mixin Vec2;
}

struct Motion
{
    alias Dependencies = TypeTuple!(Position);
    mixin Vec2;
}

template Dependencies(Cpt)
{
    static if (__traits(hasMember, Cpt, "Dependencies")) {
        alias Dependencies = Cpt.Dependencies;
    } else {
        alias Dependencies = TypeTuple!();
    }
}

struct NullSource
{
    auto next(T)()
    {
        import std.random;
        return T.init;
    }
}

struct Subsystem(Cpt)
{
    Cpt[EntityID] data;

    void update();
    
    Cpt* create(Source)(EntityID id, ref Source src)
    {
        auto input = src.next!(Cpt.InputType);
        data[id] = Cpt(input);
        return id in data;
    }
}

struct Subsystem(Cpt : Motion)
{
    Motion[EntityID] data;
    Motion.InputType cur;

    void update();
    
    Cpt* create(Source)(EntityID id, ref Source src)
    {
        auto input = cur;
        cur[] += 1;
        data[id] = Cpt(input);
        return id in data;
    }
}

struct ComponentSystem(SystemIndex)
{
    alias Subsystems =
        TypeVector!(staticMap!(Subsystem, SystemIndex.Components));
    // alias Subsystems = TypeVector!(staticMap!(Subsystem, SystemIndex));
    Subsystems subsystems;
    NullSource defaultInput;
    
    bool[EntityID] freeIDs;

    void update()
    {
        foreach (C; SystemIndex.Components)
            subsystems.get!(Subsystem!C).update();
    }
    
    auto createEntity(EntityIndex)()
    { return createEntity!(EntityIndex, NullSource)(defaultInput); }
    
    auto createEntity(EntityIndex, Source)(ref Source input)
    {
        Entity!(EntityIndex) entity;
        foreach (C; EntityIndex.Components) {
            auto id = getFreshID();
            C* cpt = subsystems.get!(Subsystem!C).create(id, input);
            entity.addComponent!C(cpt);
        }

        return entity;
    }

    EntityID getFreshID()
    {
        import std.algorithm;
        EntityID maxID = 0;
        foreach (id, ref used; freeIDs) { // reuse IDs
            if (!used) {
                used = true;
                return id;
            }
            maxID = max(id, maxID);
        }
        maxID += 1;
        // freeIDs ~= true;
        freeIDs[maxID] = true;
        // return freeIDs.length - 1;
        return maxID;
    }
    
    struct Entity(EntityIndex)
        if (SystemIndex.supports!(EntityIndex.Components))
    {
        EntityIndex index;
        
        void addComponent(Cpt)(Cpt* cpt)
            if (EntityIndex.supports!(Cpt))
        {
            import std.conv;
            foreach (C; Dependencies!Cpt)
                assert(index.get!C !is null,
                       text("Dependency ", typeid(C), " is not initiated!"));
                
            index.get!Cpt = cpt;
            // import std.stdio; writeln(*index.get!Cpt);
        }
    
        auto get(Cpt)() {
            return index.get!Cpt;
        }
    }
}

void main()
{
    // import core.memory; GC.disable;

    // Subsystem!Position positions;
    // Subsystem!Motion motions;

    alias AllCpts = ComponentIndex!(Position, Motion);
    alias CSys = ComponentSystem!(AllCpts);
    
    alias PM = ComponentIndex!(Position, Motion);
    alias EntPM = CSys.Entity!(PM);

    CSys system;

    system.update();
    
    auto ents = new EntPM[5];

    foreach (ref ent; ents) {
        ent = system.createEntity!PM();
    }    
    
    // foreach (id, ref ent; ents) {
    //     auto cpt = positions.create(id);
    //     ent.addComponent(cpt);
    //     ent.addComponent(motions.create(id));
    // }

    import std.stdio;
    
    foreach (ent; ents) writeln(*ent.get!Position);
    foreach (ent; ents) writeln(*ent.get!Motion);
    // foreach (id, pos; positions.data) writeln(id, " => ", pos);
}
