#include <iostream>

class CPU
{
private:
    long position;

public:
    void freeze()
    {
        std::cout << "CPU FREEZE" << std::endl;
    }
    
    void jump(long position)
    {
        this->position = position;
        std::cout << "CPU JUMP TO " << position << std::endl;
    }

    void execute()
    {
        std::cout << "CPU EXECUTE" << std::endl;
    }
};

class Memory
{
public:
    void load(long position, unsigned char* data)
    {
        std::cout << "Load To " << position << std::endl;
        std::cout << "DATA : " << data << std::endl;
    }
};

class HardDrive
{
public:
    unsigned char* read(long lba, int size)
    {
        std::cout << "lba : " << lba << std::endl;
        unsigned char* data = new unsigned char[size];
        return data;
    }
};

static const long BOOT_ADDRESS = 0x23594;
static const long BOOT_SECTOR = 0x42340;
static const int SECTOR_SIZE = 32;

// Facade
class Computer 
{
private:
    CPU* myCpu;
    Memory* myMemory;
    HardDrive* myHardDrive;

public:
    void startComputer()
    {
        myCpu = new CPU();
        myMemory = new Memory();
        myHardDrive = new HardDrive();
        myCpu->freeze();
        myMemory->load(BOOT_ADDRESS, myHardDrive->read(BOOT_SECTOR, SECTOR_SIZE));
        myCpu->jump(BOOT_ADDRESS);
        myCpu->execute();
    }
};

int main()
{
    Computer facade;
    facade.startComputer();

    return 0;
}