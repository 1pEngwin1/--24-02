#pragma once
#include <string>
#include <sstream>

using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;
using namespace System::Globalization;

// јбстрактна€ база сотрудников
public ref class Employee abstract {
public:
    property String^ Name;
    Employee(String^ name) {
        if (String::IsNullOrWhiteSpace(name)) throw gcnew ArgumentException("Name must be non-empty");
        Name = name;
    }
    virtual double Salary() abstract; // мес€чна€ зарплата
    virtual String^ ToString() override {
        return String::Format("{0}", Name);
    }
    virtual String^ Serialize() abstract; // Type;params
    static Employee^ Deserialize(String^ line);
};


public ref class Developer sealed : public Employee {
    double hourlyRate;
    double hoursWorked;
    int experienceYears;
public:
    Developer(String^ name, double hourlyRate, double hoursWorked, int experienceYears)
        : Employee(name)
    {
        if (hourlyRate <= 0) throw gcnew ArgumentOutOfRangeException("hourlyRate");
        if (hoursWorked < 0) throw gcnew ArgumentOutOfRangeException("hoursWorked");
        if (experienceYears < 0) throw gcnew ArgumentOutOfRangeException("experienceYears");
        this->hourlyRate = hourlyRate;
        this->hoursWorked = hoursWorked;
        this->experienceYears = experienceYears;
    }

    virtual double Salary() override {
        
        double basePay = hourlyRate * hoursWorked;
        double bonus = basePay * 0.03 * experienceYears;
        return Math::Round(basePay + bonus, 2);
    }

    virtual String^ ToString() override {
        return String::Format("Developer: {0}, rate={1:R}, hours={2:R}, exp={3} -> salary={4:F2}",
            Name, hourlyRate, hoursWorked, experienceYears, Salary());
    }

    virtual String^ Serialize() override {
        return String::Format("Developer;{0};{1};{2};{3}",
            Name,
            hourlyRate.ToString("R", CultureInfo::InvariantCulture),
            hoursWorked.ToString("R", CultureInfo::InvariantCulture),
            experienceYears.ToString());
    }

    static Developer^ Parse(array<String^>^ parts) {
        if (parts->Length != 5) throw gcnew ArgumentException("Developer requires 4 parameters: Developer;Name;hourlyRate;hoursWorked;experienceYears");
        String^ name = parts[1];
        double r = Double::Parse(parts[2], CultureInfo::InvariantCulture);
        double h = Double::Parse(parts[3], CultureInfo::InvariantCulture);
        int e = Int32::Parse(parts[4], CultureInfo::InvariantCulture);
        return gcnew Developer(name, r, h, e);
    }
};

public ref class Manager sealed : public Employee {
    double baseSalary;
    double teamPerformance; 
public:
    Manager(String^ name, double baseSalary, double teamPerformance)
        : Employee(name)
    {
        if (baseSalary <= 0) throw gcnew ArgumentOutOfRangeException("baseSalary");
        if (Double::IsNaN(teamPerformance) || teamPerformance < 0.0 || teamPerformance > 1.0) throw gcnew ArgumentOutOfRangeException("teamPerformance");
        this->baseSalary = baseSalary;
        this->teamPerformance = teamPerformance;
    }

    virtual double Salary() override {
        
        double bonus = baseSalary * 0.30 * teamPerformance;
        return Math::Round(baseSalary + bonus, 2);
    }

    virtual String^ ToString() override {
        return String::Format("Manager: {0}, base={1:R}, perf={2:R} -> salary={3:F2}",
            Name, baseSalary, teamPerformance, Salary());
    }

    virtual String^ Serialize() override {
        return String::Format("Manager;{0};{1};{2}",
            Name,
            baseSalary.ToString("R", CultureInfo::InvariantCulture),
            teamPerformance.ToString("R", CultureInfo::InvariantCulture));
    }

    static Manager^ Parse(array<String^>^ parts) {
        // parts: ["Manager", name, baseSalary, teamPerformance]
        if (parts->Length != 4)
            throw gcnew ArgumentException("Manager requires 3 parameters: Manager;Name;baseSalary;teamPerformance");

        String^ name = parts[1];
        double baseS = Double::Parse(parts[2], CultureInfo::InvariantCulture);
        double perf = Double::Parse(parts[3], CultureInfo::InvariantCulture);
        return gcnew Manager(name, baseS, perf);
    }
};

// ‘абрикa десериализации
Employee^ Employee::Deserialize(String^ line) {
    if (String::IsNullOrWhiteSpace(line)) throw gcnew ArgumentException("Line is empty");
    array<String^>^ parts = line->Split(';');
    if (parts->Length == 0) throw gcnew ArgumentException("Bad format");
    String^ type = (parts->Length > 0 && parts[0] != nullptr) ? parts[0]->Trim() : String::Empty;
    try {
        if (type->Equals("Developer", StringComparison::InvariantCultureIgnoreCase)) {
            return Developer::Parse(parts);
        }
        else if (type->Equals("Manager", StringComparison::InvariantCultureIgnoreCase)) {
            return Manager::Parse(parts);
        }
        else {
            throw gcnew ArgumentException("Unknown employee type: " + type);
        }
    }
    catch (FormatException^) {
        throw gcnew ArgumentException("Invalid numeric format in parameters");
    }
    catch (OverflowException^) {
        throw gcnew ArgumentException("Numeric value out of range in parameters");
    }
}
