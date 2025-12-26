#pragma once
#include "Employee.h"

void SaveEmployees(String^ filename, List<Employee^>^ list) {
    StreamWriter^ sw = gcnew StreamWriter(filename, false, System::Text::Encoding::UTF8);
    for each (Employee ^ e in list) sw->WriteLine(e->Serialize());
    sw->Close();
}

List<Employee^>^ LoadEmployees(String^ filename) {
    List<Employee^>^ res = gcnew List<Employee^>();
    StreamReader^ sr = gcnew StreamReader(filename);
    String^ line; int lineno = 0;
    while ((line = sr->ReadLine()) != nullptr) {
        lineno++;
        try {
            Employee^ e = Employee::Deserialize(line);
            res->Add(e);
        }
        catch (Exception^ ex) {
            Console::WriteLine("Error parsing line {0}: {1} -> \"{2}\"", lineno, ex->Message, line);
        }
    }
    sr->Close();
    return res;
}

ref class Program {
public:
    static int Comp(Employee^ a, Employee^ b) {
        double diff = b->Salary() - a->Salary();
        if (diff > 0) return 1;
        if (diff < 0) return -1;
        return 0;
    }
};
