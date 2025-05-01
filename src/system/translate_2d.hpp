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
    static void init(sigslot::signal<entt::registry &> &signal) // TODO: WIP
    {
        static bool hasInit = false;
        if (!hasInit)
        {
            hasInit = true;
            auto lambda = [](entt::registry &reg)
            {
                // TODO: assert deltaTimeView size to be 1
                auto deltaTimeView = reg.view<DeltaTime>();
                if (!deltaTimeView.empty())
                {
                    const DeltaTime dT = deltaTimeView.get(deltaTimeView.front());
                    auto translateView = reg.view<Position, Velocity>();
                    translateView.each([const & dT](Position &pos, const Velocity &vel)
                                       {
                        pos.x += vel.x * (dT.dt_ms / 1000.0f);
                        pos.y += vel.y * (dT.dt_ms / 1000.0f); });
                }
            };
            signal.connect(lambda);
        }
    }

    static void run(entt::registry &reg) // TODO: WIP
    {
        // TODO: assert deltaTimeView size to be 1
        auto deltaTimeView = reg.view<DeltaTime>();
        if (!deltaTimeView.empty())
        {
            const DeltaTime dT = deltaTimeView.get(deltaTimeView.front());
            auto translateView = reg.view<Position, Velocity>();
            translateView.each([const & dT](Position &pos, const Velocity &vel)
                               {
                        pos.x += vel.x * (dT.dt_ms / 1000.0f);
                        pos.y += vel.y * (dT.dt_ms / 1000.0f); });
        }
    }
} // namespace SystemTranslate2D

#endif // SRC_SYSTEM_TRANSLATE_2D_HPP
