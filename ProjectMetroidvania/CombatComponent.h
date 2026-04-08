#pragma once

namespace Metroidvania {

    class CombatComponent {
    public:
        CombatComponent() = default;

        //getters
        bool getIsAttacking() const { return m_isAttacking; }
        bool getCanAttack()   const { return m_canAttack; }

        //setters
        void setIsAttacking(bool value) { m_isAttacking = value; }
        void setCanAttack(bool value) { m_canAttack = value; }

    private:
        bool m_isAttacking = false;
        bool m_canAttack = true;
    };

}