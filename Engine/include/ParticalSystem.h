#include <string>
#include <unordered_map>
#include "Components.h"

namespace System
{
    class ParticleSystem
    {
    public:
        void Update(float dt)
        {
            for (auto& p : particles)
            {
                p.position += p.velocity * dt;
                p.life -= dt;
            }

            particles.erase(
                std::remove_if(particles.begin(), particles.end(),
                    [](const Components::Particle& p) { return p.life <= 0.f; }),
                particles.end()
            );
        }

        void EmitDust(Vec2 pos, int count)
        {
            for (int i = 0; i < count; i++)
            {
                Components::Particle p;
                p.position = pos;

                float angle = 1;
                float speed = 0;

                p.velocity = Vec2{ cos(angle), sin(angle) } *speed;
                p.life = 1;
                p.maxLife = p.life;
                p.size = 1;

                p.color = SDL_Color{ 255, 200, 120, 255 };

                particles.push_back(p);
            }
        }

        const std::vector<Components::Particle>& GetParticles() const { return particles; }

    private:
        std::vector<Components::Particle> particles;
    };
}