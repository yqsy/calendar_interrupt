#include <time.h>
#include <stdio.h>

#include <iostream>
#include <queue>
#include <string>

#include <windows.h>



// refer:
// http://www.cplusplus.com/reference/ctime/strftime/
// https://www.epochconverter.com/
class CalendarInterrupt
{
public:
    struct TimePointsCompare
    {
        bool operator()(const time_t& l, const time_t& r)
        {
            return l > r;
        }
    };

    typedef std::priority_queue<time_t, std::vector<time_t>, TimePointsCompare> TimePriorityQueue;

public:
    
    bool AddTimePoint(const std::string& time_point)
    {
        if(time_point.size() != 6)
            return false;
           
        if(time_point < "000000" || time_point > "240000")
            return false;
        
        std::string hour = time_point.substr(0, 2);
        std::string minute = time_point.substr(2,2);
        std::string second = time_point.substr(4,2);
        
        if (hour < "00" || hour > "23")
            return false;
        
        if (minute < "00" || minute > "59")
            return false;
        
        if(second < "00" || second > "61")
            return false;

        time_t t;
        time(&t);
        tm timeinfo;
        localtime_s(&timeinfo, &t);
        
        timeinfo.tm_hour = atoi(hour.c_str());
        timeinfo.tm_min = atoi(minute.c_str());
        timeinfo.tm_sec = atoi(second.c_str());

        time_t interrupt_time_point = mktime(&timeinfo);
        
        // 今日的该中断时间点已过去,那么再加一天
        if(interrupt_time_point < t)
            interrupt_time_point += 86400;

        interrupt_time_points_.push(interrupt_time_point);

        return true;
    }
    

    bool BlockToNextTimePoint()
    {
        if(interrupt_time_points_.size() < 1)
            return false;
        
        time_t now_time;
        time(&now_time);

        time_t next_block_time = interrupt_time_points_.top();
        interrupt_time_points_.pop();

        if(next_block_time > now_time)
        {
            time_t sleep_secs = next_block_time - now_time;
            Sleep((DWORD)sleep_secs * 1000);
        }
        
        // 这个时间点今天已经睡过了,再加上一天
        next_block_time += 86400;

        interrupt_time_points_.push(next_block_time);

        return true;
    }
    
    TimePriorityQueue GetTimePointsQueue()
    {
        return interrupt_time_points_;
    }

private:


    TimePriorityQueue interrupt_time_points_;
};

void PrintQueue(const CalendarInterrupt::TimePriorityQueue& queue)
{
    CalendarInterrupt::TimePriorityQueue copy_queue = queue;

    std::string print_str = "[队列中等待中断的时间点]\n";

    while(!copy_queue.empty())
    {
        time_t t = copy_queue.top();
        tm timeinfo;
        localtime_s(&timeinfo, &t);
        char time_buf[100] = {};
        strftime(time_buf,100,"[%Y-%m-%d %H:%M:%S]\n", &timeinfo);
        print_str += time_buf;
        copy_queue.pop();
    }

    std::cout << print_str << std::endl;
}


int main() 
{
    CalendarInterrupt calendar_interrupt;
    calendar_interrupt.AddTimePoint("143035");
    calendar_interrupt.AddTimePoint("154720");
    calendar_interrupt.AddTimePoint("154740");
    calendar_interrupt.AddTimePoint("154741");
    
    while(calendar_interrupt.BlockToNextTimePoint())
    {
        time_t t;
        time(&t);
        tm timeinfo;
        localtime_s(&timeinfo, &t);
        char time_buf[100] = {};
        strftime(time_buf,100,"%Y-%m-%d %H:%M:%S", &timeinfo);
        std::cout << "当前中断的时间点:" << time_buf << std::endl;

        PrintQueue(calendar_interrupt.GetTimePointsQueue());
    }

    return 0;
}
 