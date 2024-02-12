#ifndef NODEC_WORLD__SYSTEM_LOADER_HPP_
#define NODEC_WORLD__SYSTEM_LOADER_HPP_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include <nodec_scene/scene_registry.hpp>

namespace nodec_world {

class SystemRegistry {
    struct SystemEntryBase {
        virtual ~SystemEntryBase() {}

        virtual void increment_reference() = 0;
        virtual void decrement_reference() = 0;
    };

    template<typename System>
    struct SystemEntry : public SystemEntryBase {
        std::unique_ptr<System> instance;
        std::function<std::unique_ptr<System>()> factory;

        void increment_reference() {
            reference_count_++;
            if (reference_count_ == 1) {
                instance = factory();
            }
        }

        void decrement_reference() {
            reference_count_--;
            if (reference_count_ == 0) {
                instance.reset();
            }
        }

        std::array<nodec::signals::Connection, 2> connections;

    private:
        int reference_count_ = 0;
    };

public:
    SystemRegistry(nodec_scene::SceneRegistry &scene_registry)
        : scene_registry_{scene_registry} {
    }

    template<typename System, typename SystemEnablerComponent, typename Factory>
    void register_system(Factory &&factory) {
        using namespace nodec_scene;
        static_assert(std::is_same<decltype(factory()), std::unique_ptr<System>>::value, "Factory function must return std::unique_ptr<System>.");

        auto entry = std::make_shared<SystemEntry<System>>();
        entry->factory = std::forward<Factory>(factory);

        entry->connections[0] = scene_registry_.component_constructed<SystemEnablerComponent>().connect([=](SceneRegistry &registry, SceneEntity entity) {
            entry->increment_reference();
        });

        entry->connections[1] = scene_registry_.component_destroyed<SystemEnablerComponent>().connect([=](SceneRegistry &registry, SceneEntity entity) {
            entry->decrement_reference();
        });
    }

public:
    nodec_scene::SceneRegistry &scene_registry_;
    std::unordered_map<nodec::type_info, std::shared_ptr<SystemEntryBase>> systems_;
};

} // namespace nodec_world

#endif