#include "Stdafx.h"
#include "Vector2.h"

namespace CLROBS {
    Vector2::Vector2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    float Vector2::X::get()
    {
        return this->x;
    }

    void Vector2::X::set(float x)
    {
        this->x = x;
    }

    float Vector2::Y::get()
    {
        return this->y;
    }

    void Vector2::Y::set(float y)
    {
        this->y = y;
    }
}