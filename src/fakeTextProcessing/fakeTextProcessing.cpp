
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>

int main(int argc, char** argv)
{
    //argument
    std::string input_filename;
    std::string output_filename;
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-i") == 0)
        {
            input_filename = argv[i + 1];
            i++;
            continue;
        }
        if (strcmp(argv[i], "-o") == 0)
        {
            output_filename = argv[i + 1];
            i++;
            continue;
        }
    }

    //if
    if (input_filename.size() == 0
        || output_filename.size() == 0
        )
    {
        printf("fakeTextProcessing -i input.txt -o output.txt");
        return 1;
    }


    //read json file
    std::stringstream input_string;
    std::ifstream ifile;
    ifile.open(input_filename);
    if (ifile.good())
    {
        input_string << ifile.rdbuf();

        std::ofstream ofile;
        ofile.open(output_filename);
        if (ofile.good())
        {
            ofile << input_string.str();
            std::cout << "1" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));

            ofile << input_string.str();
            std::cout << "2" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));

            ofile << input_string.str();
            std::cout << "3" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));

            ofile << input_string.str();
            std::cout << "4" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));

            ofile << input_string.str();
            std::cout << "5" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    else
    {
        std::cout << "bad file" << std::endl;
    }


    return 0;
}

