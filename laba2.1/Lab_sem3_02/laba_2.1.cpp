#include "pch.h"
#include "ShapeProcessor.h"

using namespace System;

int main(array<System::String ^> ^args)
{
    ShapeProcessor^ processor = gcnew ShapeProcessor();

    Console::WriteLine("Creating sample shapes...");
    processor->AddShape(gcnew Circle(5.0));
    processor->AddShape(gcnew Rectangle(4.0, 6.0));
    processor->AddShape(gcnew Triangle(3.0, 4.0, 5.0));
    processor->AddShape(gcnew Circle(2.5));
    processor->AddShape(gcnew Rectangle(3.0, 3.0));
    processor->AddShape(gcnew Triangle(5.0, 12.0, 13.0));

    processor->GenerateReport();

    processor->DemonstrateTypeCasting();

    String^ filename = "shapes.txt";
    processor->SaveToFile(filename);

    Console::WriteLine();
    Console::WriteLine("Loading shapes from file...");

    ShapeProcessor^ loadedProcessor = gcnew ShapeProcessor();
    loadedProcessor->LoadFromFile(filename);

    if (loadedProcessor->Count > 0)
    {
        loadedProcessor->GenerateReport();
        loadedProcessor->DemonstrateTypeCasting();
    }
    return 0;
}
