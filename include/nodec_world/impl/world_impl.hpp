#ifndef NODEC_WORLD__IMPL__WORLD_IMPL_HPP_
#define NODEC_WORLD__IMPL__WORLD_IMPL_HPP_

#include "../components/destroy_later.hpp"
#include "../world.hpp"

namespace nodec_world {
namespace impl {

class WorldImpl final : public World {
public:
    WorldImpl()
        : World{initialized_, stepped_, clock_.clock_interface()} {}

public:
    void step() {
        clock_.tick();
        stepped_(*this);
        on_end_of_step();
    }

    void step(float delta_sec) {
        clock_.tick(delta_sec);
        stepped_(*this);
        on_end_of_step();
    }

    void reset() {
        scene().registry().clear();
        clock_.reset();
        initialized_(*this);
    }

private:
    void on_end_of_step() {
        auto &scene_registry = scene().registry();
        auto view = scene_registry.view<components::DestroyLater>();
        scene_registry.destroy_entities(view.begin(), view.end());
    }

private:
    WorldSignal initialized_;
    WorldSignal stepped_;
    WorldClock clock_;
};
} // namespace impl
} // namespace nodec_world

#endif