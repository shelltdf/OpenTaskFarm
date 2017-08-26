
#include <libotf/Task.h>

Task::Task()
{
}

Task::~Task()
{
}

bool Task::fromJson(const std::string json)
{

    //parser json
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(json, root, false))
    {
        std::cout << json.c_str() << " parser error" << std::endl;
        return false;
    }


    for (auto it = root.begin(); it != root.end(); it++)
    {
        Json::Value key = it.key();
        Json::Value value = *it;

        if (key.asString() == "setting")
        {
            for (auto it2 = value.begin(); it2 != value.end(); it2++)
            {
                Json::Value key2 = it2.key();
                Json::Value value2 = *it2;
                if (key2.asString() == "taskID")
                {
                    setting.taskID = value2.asString();
                }
                if (key2.asString() == "workdir")
                {
                    setting.workdir = value2.asString();
                }
            }
        }

        if (key.asString() == "downloads"
            || key.asString() == "processes"
            || key.asString() == "uploads"
            )
        {
            for (size_t i = 0; i < value.size(); i++)
            {
                TaskOperationData data;
                for (auto it2 = value[i].begin(); it2 != value[i].end(); it2++)
                {
                    Json::Value key2 = it2.key();
                    Json::Value value2 = *it2;
                    if (key2.asString() == "source")
                    {
                        data.source = value2.asString();
                    }
                    //if (key2.asString() == "dest")
                    //{
                    //    data.dest = value2.asString();
                    //}
                    //if (key2.asString() == "log")
                    //{
                    //    data.log = value2.asString();
                    //}
                }

                if (key.asString() == "downloads")
                {
                    downloads.push_back(data);
                }
                if (key.asString() == "processes")
                {
                    processes.push_back(data);
                }
                if (key.asString() == "uploads")
                {
                    uploads.push_back(data);
                }

            }
        }

    }//for root

    return true;
}

std::string Task::toJson()
{
    std::string ret;


    return ret;
}


//void Task::clean()
//{
//}

void Task::run()
{
    state = 1;
}

void Task::stop()
{
    state = 0;
}

bool Task::isRun()
{
    //return state == 0;
    return !done;
}


std::string Task::replaceVariable(std::string str)const
{
    // ${TaksID} 
    //std::string ret;

    //for (size_t i = 0; i < str.size(); i++)
    //{
    //    //if(str[i] == /)
    //}

    return str;
}



void Task::update()
{
    //state change
    if (pre_state != state)
    {
        if (state == 0)
        {
            //stop all
            if (sp) { delete sp; sp = 0; }
            done = true;
        }

        if (state == 1)
        {
            //start all
            if (sp == 0) { sp = new Subprocess(); }
            done = false;

            if (setting.workdir.size() > 0)
            {
                int s = _mkdir(setting.workdir.c_str());
            }
        }

        pre_state = state;
    }


    //running loop
    if (state == 1)
    {
        bool is_all_done = true;

        if (is_all_done) is_all_done = doOperation(downloads);
        if (is_all_done) is_all_done = doOperation(processes);
        if (is_all_done) is_all_done = doOperation(uploads);

        if (is_all_done)
        {
            state = 0;
        }

    }// if (state == 1)

}


bool Task::doOperation(std::vector<TaskOperationData>& data_list) const
{
    bool is_all_done = true;

    for (size_t i = 0; i < data_list.size(); i++)
    {
        if ((!data_list[i].done) && (!data_list[i].running))
        {
            //start
            sp->run(replaceVariable(data_list[i].source));
            data_list[i].running = true;

            std::cout << "<-Task::Operation->" << data_list[i].source.c_str() << std::endl;
        }

        if (data_list[i].running)
        {
            if (sp->isRun())
            {
                is_all_done = false;
                break;
            }
            else
            {
                //stop 
                data_list[i].done = true;
                data_list[i].running = false;
            }
        }
    }//for

    return is_all_done;
}
