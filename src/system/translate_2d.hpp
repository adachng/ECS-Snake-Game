#ifndef SRC_SYSTEM_TRANSLATE_2D_HPP
#define SRC_SYSTEM_TRANSLATE_2D_HPP

#include <SDL3/SDL_assert.h>
#include <entt/entt.hpp>
#include <sigslot/signal.hpp>

#include <component/position.hpp>
#include <component/velocity.hpp>
#include <component/delta_time.hpp>

namespace SystemTranslate2D
{
    static void iterate(entt::registry &reg)
    {
        auto deltaTimeView = reg.view<DeltaTime>();
        if (!deltaTimeView.empty())
        {
            SDL_assert(deltaTimeView.size() == 1);
            DeltaTime &dT = reg.get<DeltaTime>(deltaTimeView.front());
            auto translateView = reg.view<Position, Velocity>();
            translateView.each([&dT](Position &pos, const Velocity &vel)
                               {
                        pos.x += vel.x * (dT.dt_ms / 1000.0f);
                        pos.y += vel.y * (dT.dt_ms / 1000.0f); });
        }
    }

    static bool init(sigslot::signal<entt::registry &> &signal)
    {
        static std::list<sigslot::signal<entt::registry &> *> regSignalArray;
        bool ret = true;
        for (auto connectedSignal : regSignalArray)
        {
            if (connectedSignal == &signal)
            {
                ret = false;
                break;
            }
        }
        if (ret)
        {
            signal.connect(SystemTranslate2D::iterate);
            regSignalArray.push_back(&signal);
        }
        return ret;
    }
} // namespace SystemTranslate2D

#endif // SRC_SYSTEM_TRANSLATE_2D_HPP
