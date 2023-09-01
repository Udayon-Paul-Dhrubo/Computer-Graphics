#pragma once

#include<bits/stdc++.h>
using namespace std;

class Color{

public:
    double r, g, b;

    Color()
    {
        this->r = 0.0;
        this->g = 0.0;
        this->b = 0.0;
    }
    Color(double r, double g, double b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    Color operator *(double d)
    {
        return Color(this->r * d, this->g * d, this->b * d);
    }
    Color operator *(Color const &c)
    {
        return Color(this->r * c.r, this->g * c.g, this->b * c.b);
    }
    Color operator +(Color const &c)
    {
        return Color(this->r + c.r, this->g + c.g, this->b + c.b);
    }
    Color operator -(Color const &c)
    {
        return Color(this->r - c.r, this->g - c.g, this->b - c.b);
    }
    void fixRange()
    {
        this->r = std::min(this->r, 1.0);
        this->r = std::max(this->r, 0.0);
        this->g = std::min(this->g, 1.0);
        this->g = std::max(this->g, 0.0);
        this->b = std::min(this->b, 1.0);
        this->b = std::max(this->b, 0.0);
    }
    friend ostream& operator<<(ostream& os, const Color& c);
};