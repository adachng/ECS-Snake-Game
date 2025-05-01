#pragma once
#ifndef SRC_SYSTEM_TRANSLATE_2D_HPP
#define SRC_SYSTEM_TRANSLATE_2D_HPP

#include <entt/entt.hpp>
#include <sigslot/signal.hpp>

#include <component/position.hpp>
#include <component/velocity.hpp>
#include <component/delta_time.hpp>

namespace SystemTranslate2D
{
    static void iterate(entt::registry &reg)
    {
        // TODO: assert deltaTimeView size to be 1
        auto deltaTimeView = reg.view<DeltaTime>();
        if (!deltaTimeView.empty())
        {
            DeltaTime &dT = reg.get<DeltaTime>(deltaTimeView.front());
            auto translateView = reg.view<Position, Velocity>();
            translateView.each([&dT](Position &pos, const Velocity &vel)
                               {
                        pos.x += vel.x * (dT.dt_ms / 1000.0f);
                        pos.y += vel.y * (dT.dt_ms / 1000.0f); });
        }
    }

    static void init(sigslot::signal<entt::registry &> &signal)
    {
        static bool hasInit = false;
        if (!hasInit)
        {
            hasInit = true;
            signal.connect(SystemTranslate2D::iterate);
        }
    }
} // namespace SystemTranslate2D

#endif // SRC_SYSTEM_TRANSLATE_2D_HPP
