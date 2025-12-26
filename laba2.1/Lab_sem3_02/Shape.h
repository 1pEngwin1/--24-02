#pragma once
#include <string>
#include <sstream>

using namespace System;

public ref class Shape abstract
{
public:
    virtual double Area() abstract;
    virtual double Perimeter() abstract;
    virtual void Print() abstract;
    virtual String^ ToString() override abstract;

    virtual String^ Serialize() abstract;
    static Shape^ Deserialize(String^ line);
};

public ref class Circle : public Shape
{
private:
    double radius;

public:
    Circle(double r) : radius(r)
    {
        if (r <= 0)
            throw gcnew ArgumentOutOfRangeException("radius", "Radius must be positive");
    }

    virtual double Area() override
    {
        return Math::PI * radius * radius;
    }

    virtual double Perimeter() override
    {
        return 2 * Math::PI * radius;
    }

    virtual void Print() override
    {
        Console::WriteLine("Circle: radius = {0:F2}, area = {1:F2}, perimeter = {2:F2}",
            radius, Area(), Perimeter());
    }

    virtual String^ ToString() override
    {
        return String::Format("Circle: radius={0:F2}", radius);
    }

    virtual String^ Serialize() override
    {
        return String::Format("Circle;{0}", radius);
    }

    property double Radius
    {
        double get() { return radius; }
    }
};

public ref class Rectangle : public Shape
{
private:
    double width;
    double height;

public:
    Rectangle(double w, double h) : width(w), height(h)
    {
        if (w <= 0)
            throw gcnew ArgumentOutOfRangeException("width", "Width must be positive");
        if (h <= 0)
            throw gcnew ArgumentOutOfRangeException("height", "Height must be positive");
    }

    virtual double Area() override
    {
        return width * height;
    }

    virtual double Perimeter() override
    {
        return 2 * (width + height);
    }

    virtual void Print() override
    {
        Console::WriteLine("Rectangle: {0:F2} x {1:F2}, area = {2:F2}, perimeter = {3:F2}",
            width, height, Area(), Perimeter());
    }

    virtual String^ ToString() override
    {
        return String::Format("Rectangle: {0:F2}x{1:F2}", width, height);
    }

    virtual String^ Serialize() override
    {
        return String::Format("Rectangle;{0};{1}", width, height);
    }

    property double Width
    {
        double get() { return width; }
    }

    property double Height
    {
        double get() { return height; }
    }
};

public ref class Triangle : public Shape
{
private:
    double side1;
    double side2;
    double side3;

public:
    Triangle(double a, double b, double c) : side1(a), side2(b), side3(c)
    {
        if (a <= 0 || b <= 0 || c <= 0)
            throw gcnew ArgumentOutOfRangeException("sides", "All sides must be positive");

        if (!IsValidTriangle(a, b, c))
            throw gcnew ArgumentException("Invalid triangle: sides do not satisfy triangle inequality");
    }

private:
    bool IsValidTriangle(double a, double b, double c)
    {
        return (a + b > c) && (a + c > b) && (b + c > a);
    }

public:
    virtual double Area() override
    {
        // Using Heron's formula
        double s = Perimeter() / 2;
        return Math::Sqrt(s * (s - side1) * (s - side2) * (s - side3));
    }

    virtual double Perimeter() override
    {
        return side1 + side2 + side3;
    }

    virtual void Print() override
    {
        Console::WriteLine("Triangle: sides = {0:F2}, {1:F2}, {2:F2}, area = {3:F2}, perimeter = {4:F2}",
            side1, side2, side3, Area(), Perimeter());
    }

    virtual String^ ToString() override
    {
        return String::Format("Triangle: {0:F2},{1:F2},{2:F2}", side1, side2, side3);
    }

    virtual String^ Serialize() override
    {
        return String::Format("Triangle;{0};{1};{2}", side1, side2, side3);
    }

    property double Side1
    {
        double get() { return side1; }
    }

    property double Side2
    {
        double get() { return side2; }
    }

    property double Side3
    {
        double get() { return side3; }
    }
};

Shape^ Shape::Deserialize(String^ line)
{
    if (String::IsNullOrEmpty(line))
        throw gcnew ArgumentException("Line cannot be null or empty");

    array<String^>^ parts = line->Split(';');

    if (parts->Length == 0)
        throw gcnew ArgumentException("Invalid format: no data");

    String^ type = parts[0];

    try
    {
        if (type == "Circle")
        {
            if (parts->Length != 2)
                throw gcnew ArgumentException("Circle requires 1 parameter");
            double radius = Double::Parse(parts[1]);
            return gcnew Circle(radius);
        }
        else if (type == "Rectangle")
        {
            if (parts->Length != 3)
                throw gcnew ArgumentException("Rectangle requires 2 parameters");
            double width = Double::Parse(parts[1]);
            double height = Double::Parse(parts[2]);
            return gcnew Rectangle(width, height);
        }
        else if (type == "Triangle")
        {
            if (parts->Length != 4)
                throw gcnew ArgumentException("Triangle requires 3 parameters");
            double side1 = Double::Parse(parts[1]);
            double side2 = Double::Parse(parts[2]);
            double side3 = Double::Parse(parts[3]);
            return gcnew Triangle(side1, side2, side3);
        }
        else
        {
            throw gcnew ArgumentException(String::Format("Unknown shape type: {0}", type));
        }
    }
    catch (FormatException^)
    {
        throw gcnew ArgumentException("Invalid number format in parameters");
    }
}