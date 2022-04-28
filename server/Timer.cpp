#include "Timer.hpp"

void Timer::init_time(int fd){
	time_info info;

	timer_list[fd] = info;
}


Timer::Timer(){}

Timer::~Timer(){}
