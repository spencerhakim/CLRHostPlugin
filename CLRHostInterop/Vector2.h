#pragma once

using namespace System;

namespace CLROBS {
    public ref class Vector2
    {
    private:
        float x;
        float y;

    public:
        Vector2(float x, float y);

        property float X
        {
            float get();
            void set(float x);
        }

        property float Y
        {
            float get();
            void set(float y);
        }
    };
};