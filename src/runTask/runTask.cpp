
#include <libotf/Task.h>
#include <sstream>
#include <fstream>

int main(int argc, char **argv)
{
    Task* task = new Task();

    std::ostringstream task_string;
    if (argc >= 2)
    {
        std::ifstream file;
        file.open(argv[1]);
        if (file.good())
        {
            task_string << file.rdbuf(); ;
        }
    }
    else
    {
        std::cout << "runtask task.json" << std::endl;
    }

    if (task_string.str().empty())
    {
        std::cout << "task_string is empty" << std::endl;
        return 1;
    }

    if (!task->fromJson(task_string.str()))
    {
        std::cout << "json is bad" << std::endl;
        return 1;
    }

    task->run();

    while (!task->isRun())
    {
        task->update();

        //std::cout << ".";
        Sleep(1000);
    }

    std::cout << "end" << std::endl;
    delete task; task = 0;

    return 0;
}
