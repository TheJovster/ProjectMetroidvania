#include "Entity.h"

namespace Metroidvania {

    Entity::Entity(sf::Vector2f position, sf::Vector2f size)
        : m_position(position)
        , m_size(size)
        , m_velocity(0.f, 0.f)
        , m_alive(true)
    {
    }

    sf::FloatRect Entity::getBounds() const
    {
        return sf::FloatRect(m_position, m_size);
    }

    bool Entity::intersects(const Entity& other) const
    {
        return getBounds().findIntersection(other.getBounds()).has_value();
    }

}