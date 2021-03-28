#include "EmployeeFactory.h"

int main()
{
    auto john = EmployeeFactory::GetInstance()->NewAuxOfficeEmployee("John Doe", 123);
    auto jane = EmployeeFactory::GetInstance()->NewMainOfficeEmployee("Jane Doe", 125);

    std::cout << *john;
    std::cout << *jane;
}