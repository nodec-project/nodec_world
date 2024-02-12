#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec_world/system_registry.hpp>

TEST_CASE("Testing system instantiation") {
    using namespace nodec_world;
    using namespace nodec_scene;

    struct Context {
        int system_a_constructed{0};
        int system_b_constructed{0};
    };

    class SystemA {
    public:
        SystemA(Context &context)
            : context_{context} {
            ++context_.system_a_constructed;
        }
        ~SystemA() {
            --context_.system_a_constructed;
        }

    private:
        Context &context_;
    };
    class SystemB {
    public:
        SystemB(Context &context)
            : context_{context} {
            ++context_.system_b_constructed;
        }
        ~SystemB() {
            --context_.system_b_constructed;
        }

    private:
        Context &context_;
    };

    struct SystemAEnabler {};
    struct SystemBEnabler {};

    SUBCASE("System A is constructed when SystemAEnabler is added") {
        SceneRegistry scene_registry;
        SystemRegistry system_registry(scene_registry);
        Context context;

        system_registry.register_system<SystemA, SystemAEnabler>([&]() {
            return std::make_unique<SystemA>(context);
        });

        const auto entity = scene_registry.create_entity();
        scene_registry.emplace_component<SystemAEnabler>(entity);

        CHECK(context.system_a_constructed == 1);
    }

    SUBCASE("System A is not constructed again when another SystemAEnabler is added") {
        SceneRegistry scene_registry;
        SystemRegistry system_registry(scene_registry);
        Context context;

        system_registry.register_system<SystemA, SystemAEnabler>([&]() {
            return std::make_unique<SystemA>(context);
        });

        const auto entity0 = scene_registry.create_entity();
        scene_registry.emplace_component<SystemAEnabler>(entity0);

        const auto entity1 = scene_registry.create_entity();
        scene_registry.emplace_component<SystemAEnabler>(entity1);

        CHECK(context.system_a_constructed == 1);
    }

    SUBCASE("System A is destructed when all SystemAEnablers are removed") {
        SceneRegistry scene_registry;
        SystemRegistry system_registry(scene_registry);
        Context context;

        system_registry.register_system<SystemA, SystemAEnabler>([&]() {
            return std::make_unique<SystemA>(context);
        });

        const auto entity0 = scene_registry.create_entity();
        scene_registry.emplace_component<SystemAEnabler>(entity0);

        const auto entity1 = scene_registry.create_entity();
        scene_registry.emplace_component<SystemAEnabler>(entity1);

        scene_registry.destroy_entity(entity0);
        scene_registry.destroy_entity(entity1);

        CHECK(context.system_a_constructed == 0);
    }
}