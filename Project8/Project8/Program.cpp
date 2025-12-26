#include "Employee_proccesor.h"

int main(array<String^>^ args)
{
    try {
        
        List<Employee^>^ staff = gcnew List<Employee^>();
        staff->Add(gcnew Developer("Alice", 25.0, 160.0, 3));   
        staff->Add(gcnew Manager("Bob", 4000.0, 0.8));          
        staff->Add(gcnew Developer("Charlie", 30.0, 150.0, 5));
        staff->Add(gcnew Developer("Diana", 20.0, 120.0, 1));
        staff->Add(gcnew Manager("Eve", 3500.0, 0.5));

        
        String^ file = "employees.txt";
        SaveEmployees(file, staff);
        Console::WriteLine("Saved {0} employees to {1}", staff->Count, file);

        
        List<Employee^>^ loaded = LoadEmployees(file);
        Console::WriteLine("Loaded {0} employees from {1}", loaded->Count, file);
        Console::WriteLine();

        
        

        loaded->Sort(gcnew Comparison<Employee^>(Program::Comp));

        
        Console::WriteLine("Employees (sorted by salary desc):");
        double total = 0.0;
        for each (Employee ^ e in loaded) {
            Console::WriteLine(" - {0}", e->ToString());
            total += e->Salary();
        }
        double average = (loaded->Count > 0) ? total / loaded->Count : 0.0;
        Console::WriteLine();
        Console::WriteLine("Total payroll = {0:F2}", total);
        Console::WriteLine("Average salary = {0:F2}", average);

      
        int devCount = 0, mgrCount = 0;
        double devTotal = 0.0, mgrTotal = 0.0;
        for each (Employee ^ e in loaded) {
            Developer^ d = dynamic_cast<Developer^>(e);
            if (d != nullptr) { devCount++; devTotal += d->Salary(); continue; }
            Manager^ m = dynamic_cast<Manager^>(e);
            if (m != nullptr) { mgrCount++; mgrTotal += m->Salary(); continue; }
        }
        Console::WriteLine();
        Console::WriteLine("Developers: {0}, total={1:F2}, average={2:F2}", devCount, devTotal, devCount > 0 ? devTotal / devCount : 0.0);
        Console::WriteLine("Managers:   {0}, total={1:F2}, average={2:F2}", mgrCount, mgrTotal, mgrCount > 0 ? mgrTotal / mgrCount : 0.0);

    }
    catch (Exception^ ex) {
        Console::WriteLine("Fatal error: {0}", ex->ToString());
    }
    Console::WriteLine("Press Enter to exit...");
    Console::ReadLine();
    return 0;
}