#ifndef ___TEST_SDK__PROGRESS_H___
#define ___TEST_SDK__PROGRESS_H___

#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <stdint.h>


class Progress
{
public:
	Progress(const int count, const int period, std::string log_file="") :
		count(count),
		period(period),
		start_tick( time(NULL) ),
		start_tick_clock(),
		log_file(log_file)
	{
		prev_i = 0;
	}

	void tick(const int i)
	{
		if(i >= prev_i + period || i == count)
		{
			prev_i = i;

			const std::time_t tick = time(NULL);

			std::cout << i << " / " << count << "  |";

			if(i == count)
			{
				std::cout << " Total: " << make_time_str(tick - start_tick);

				if(!log_file.empty())
				{
					std::ofstream log_ostream(log_file.c_str());
					if(log_ostream.is_open())
					{
						log_ostream << "Total_real_time: " << make_time_str(tick - start_tick) << "\n";
						log_ostream << "Average_real_time_per_step: " << make_time_str(double(tick - start_tick) / count) << "\n";

						log_ostream << "Total_cpu_time: " << make_time_str_from_clock(
							clock() - start_tick_clock) << "\n";
						log_ostream << "Average_cpu_time_per_step: " << make_time_str_from_clock(
							double(clock() - start_tick_clock) / count) << "\n";

						log_ostream.flush();
						if(!log_ostream.good())
							std::cout << "Warning: write error to log file in Progress" << std::endl;
					}
					else
					{
						std::cout << "Warning: can't open log file in Progress" << std::endl;
					}
				}
			}
			else
				std::cout << " ETA: " << make_time_str( double(tick - start_tick) * (count - i) / i );

			std::cout << " | Step: " << make_time_str( double(tick - start_tick) / i ) << "     \r";

			if(i == count)
				std::cout << std::endl;
			else
				std::cout << std::flush;
		}
	}

private:

	const int count;
	const int period;
	const std::time_t start_tick;
	const std::clock_t start_tick_clock;
	int prev_i;
	std::string log_file;

	static
	std::string make_time_str(const double ticks)
	{
		int64_t ms = ticks * 1000;
		int64_t s = ticks;
		ms -= s * 1000;
		int64_t m = s / 60;
		s -= m * 60;
		int64_t h = m / 60;
		m -= h * 60;

		std::ostringstream r;

		if(h > 0) r << h << "h ";
		if(m > 0) r << m << "m ";
		if(s > 0 && h == 0) r << s << "s ";
		if(ms > 0 && m == 0) r << ms << "ms ";
		if(h == 0 && m == 0 && s == 0 && ms == 0) r << "0 ";

		return r.str();
	}

	static
	std::string make_time_str_from_clock(const std::clock_t ticks)
	{
		return make_time_str(double(ticks) / CLOCKS_PER_SEC);
	}
};


#endif  // ___TEST_SDK__PROGRESS_H___
