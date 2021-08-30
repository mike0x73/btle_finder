#include <iostream>
#include "btle_enumerator.cpp"

void usage(char *argv[])
{
    std::cout << "Usage: " << argv[0] << std::endl;
    std::cout << "Enumerate available BTLE devices. Does not filter malformed packets." << std::endl << std::endl; 
    std::cout << "\t-h\tPrint help." << std::endl;
    std::cout << "\t-d\tDebug mode. Prints full packet of advertising data." << std::endl;
    std::cout << "\t-p\tOnly show devices with printable names. Without this option, expect erratic output." << std::endl;
    std::cout << "\t-f\tFilter out seen bluetooth addresses." << std::endl;
}

int main(int argc, char *argv[]) 
{
    bool check_printable_name = false;
    bool debug = false;
    bool filter_seen = false;

    for (int i=0; i<argc; i++)
    {
        std::string argument = argv[i];

        if (argument == "-h")
        {
            usage(argv);
            return 0;
        }

        if (argument == "-p")
        {
            check_printable_name = true;
        }

        if (argument == "-d")
        {
            debug = true;
        }

        if (argument == "-f")
        {
            filter_seen = true;
        }
    }
    
    enumerate_btle_devices(check_printable_name, debug, filter_seen);
}