#include <iostream>

#include "BankAccount.h"

int main()
{
    BankAccount ba{ 100 };
    ba.depiosit(50);
    ba.depiosit(25);
    cout << ba << endl;

    ba.undo();
    cout << ba << endl;
    
    ba.undo();
    cout << ba << endl;
    
    ba.redo();
    cout << ba << endl;

    ba.redo();
    cout << ba << endl;
}