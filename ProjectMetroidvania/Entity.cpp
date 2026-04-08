#include "Entity.h"

namespace Metroidvania {

    Entity::Entity(sf::Vector2f position, sf::Vector2f size)
        : m_physicsBody(position, size)
        , m_alive(true)
    {
    }

}