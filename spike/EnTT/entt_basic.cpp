#include <iostream>

#include <entt/entt.hpp> // https://skypjack.github.io/entt/

struct Position
{
    float x;
    float y;
};

struct Velocity
{
    float x;
    float y;
};

void one_comp();
void two_comp();

void print_registry_size(entt::registry &reg) // cannot be const ref
{
    size_t entityCount = reg.storage<entt::entity>().size();
    std::cout << "registry size = " << entityCount << std::endl;
}

int main()
{
    one_comp();

    two_comp();

    return 0;
}

void one_comp()
{
    entt::registry registry; // https://skypjack.github.io/entt/namespaceentt.html#ab8c1f492841aeafa5e75422af3f0cdc4

    auto entity = registry.create();

    std::cout << "========registry.valid==========" << std::endl;
    std::cout << "auto entity = registry.create();" << std::endl;
    std::cout << "registry.valid(entity) = " << registry.valid(entity) << std::endl;
    print_registry_size(registry);
    registry.clear();
    std::cout << "registry.clear();" << std::endl;
    std::cout << "registry.valid(entity) = " << registry.valid(entity) << std::endl;
    print_registry_size(registry); // still 1 despite the entity being invalid

    std::cout << "=========Registry size (valid/invalid)=========" << std::endl;
    entity = registry.create();
    std::cout << "entity = registry.create();" << std::endl;
    print_registry_size(registry); // still 1 despite the invalid entity previously

    size_t entityCount = registry.storage<Position>().size(); // can use Position too
    std::cout << "registry size (Position) = " << entityCount << std::endl;
    registry.emplace<Position>(entity, 1.0f, 2.3f);
    entityCount = registry.storage<Position>().size();
    std::cout << "registry size (Position) = " << entityCount << std::endl;

    std::cout << "=========Getting component from registry=========" << std::endl;
    auto posView = registry.view<Position>();
    posView.each([](const Position &pos)
                 { std::cout << "Before modifying: (" << pos.x << "," << pos.y << ")" << std::endl; }); // print
    posView.each([](Position &pos)
                 {pos.x = 100.0f; pos.y = 123.0f; });
    posView.each([](const Position &pos)
                 { std::cout << "After modifying: (" << pos.x << "," << pos.y << ")" << std::endl; }); // print

    Position pos1 = registry.get<Position>(entity);
    std::cout << "pos1: (" << pos1.x << "," << pos1.y << ")" << std::endl;
}

void two_comp()
{
    std::cout << "\n=========Two Components=========" << std::endl;
    entt::registry registry;

    auto entity = registry.create();
    registry.emplace<Position>(entity, 1.1f, 1.2f);
    registry.emplace<Velocity>(entity, 2.3f, 2.4f);

    auto entity2 = registry.create();
    registry.emplace<Position>(entity2, 4.1f, 4.2f);

    auto view = registry.view<Position, Velocity>(); // does not have entity2
    view.each([](const Position &pos, const Velocity &vel)
              { std::cout << "Position: (" << pos.x << ", " << pos.y << ")\n"
                          << "Velocity: (" << vel.x << ", " << vel.y << ")" << std::endl; }); // print
    auto posView = registry.view<Position>();
    posView.each([](const Position &pos)
                 { std::cout << "Position: (" << pos.x << ", " << pos.y << ")" << std::endl; }); // print
}
