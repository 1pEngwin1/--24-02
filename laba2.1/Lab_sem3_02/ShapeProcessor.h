#pragma once
#include "Shape.h"

using namespace System::Collections::Generic;

public ref class ShapeProcessor
{
private:
    List<Shape^>^ shapes;

public:
    ShapeProcessor()
    {
        shapes = gcnew List<Shape^>();
    }

    void AddShape(Shape^ shape)
    {
        if (shape == nullptr)
            throw gcnew ArgumentNullException("shape");
        shapes->Add(shape);
    }

    void GenerateReport()
    {
        Console::WriteLine("=== SHAPES REPORT ===");
        Console::WriteLine();

        for each (Shape ^ shape in shapes)
        {
            shape->Print();
        }

        Console::WriteLine();

        double totalArea = 0;
        double totalPerimeter = 0;

        for each (Shape ^ shape in shapes)
        {
            totalArea += shape->Area();
            totalPerimeter += shape->Perimeter();
        }

        Console::WriteLine("TOTALS: Area = {0:F2}, Perimeter = {1:F2}", totalArea, totalPerimeter);
        Console::WriteLine();
    }

    void DemonstrateTypeCasting()
    {
        Console::WriteLine("=== TRIANGLE ANALYSIS ===");

        int triangleCount = 0;
        double totalTriangleArea = 0;

        for each (Shape ^ shape in shapes)
        {
            Triangle^ triangle = dynamic_cast<Triangle^>(shape);
            if (triangle != nullptr)
            {
                triangleCount++;
                totalTriangleArea += triangle->Area();
                Console::WriteLine("Found triangle: {0}", triangle);
            }
        }

        if (triangleCount > 0)
        {
            double averageArea = totalTriangleArea / triangleCount;
            Console::WriteLine("Triangles found: {0}", triangleCount);
            Console::WriteLine("Average triangle area: {0:F2}", averageArea);
        }
        else
        {
            Console::WriteLine("No triangles found in collection");
        }
        Console::WriteLine();
    }

    void SaveToFile(String^ filename)
    {
        try
        {
            IO::StreamWriter^ writer = gcnew IO::StreamWriter(filename);
            try
            {
                for each (Shape ^ shape in shapes)
                {
                    writer->WriteLine(shape->Serialize());
                }
                Console::WriteLine("Successfully saved {0} shapes to {1}", shapes->Count, filename);
            }
            finally
            {
                delete writer;
            }
        }
        catch (Exception^ ex)
        {
            throw gcnew InvalidOperationException(
                String::Format("Failed to save shapes to file: {0}", ex->Message));
        }
    }

    void LoadFromFile(String^ filename)
    {
        if (!IO::File::Exists(filename))
            throw gcnew IO::FileNotFoundException("File not found", filename);

        shapes->Clear();
        int successCount = 0;
        int errorCount = 0;

        try
        {
            array<String^>^ lines = IO::File::ReadAllLines(filename);

            for (int i = 0; i < lines->Length; i++)
            {
                String^ line = lines[i]->Trim();
                if (String::IsNullOrEmpty(line))
                    continue;

                try
                {
                    Shape^ shape = Shape::Deserialize(line);
                    shapes->Add(shape);
                    successCount++;
                }
                catch (Exception^ ex)
                {
                    errorCount++;
                    Console::WriteLine("Error on line {0}: {1}", i + 1, ex->Message);
                }
            }

            Console::WriteLine("Loaded {0} shapes successfully, {1} errors", successCount, errorCount);
        }
        catch (Exception^ ex)
        {
            throw gcnew InvalidOperationException(
                String::Format("Failed to load shapes from file: {0}", ex->Message));
        }
    }

    property int Count
    {
        int get() { return shapes->Count; }
    }
};