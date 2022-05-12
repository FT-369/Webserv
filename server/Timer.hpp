#ifndef TIMER_HPP
#define TIMER_HPP
#include <map>
#include <sys/time.h>
#include <string>
#include <ctime>
#include <iostream>
class Timer
{
private:
	typedef struct s_timer_info
	{
		int keep_alive_timeout;
		time_t start;
	} time_info;
	std::map<int, time_info> _timer_list;
	Timer();
	~Timer();

public:
	void init_time(int fd);
};

#endif
