#include "recordlog.h"

void RecordLog::init_log()
{
	ofstream file;
	stringstream ss;
	string path;
	time_t now;
	time (&now);
	struct tm current_time;
	localtime_s(&current_time, &now);

	path = "record_log/";
	ss << 1900 + current_time.tm_year;
	path.append(ss.str());
	ss << 1 + current_time.tm_mon;
	path.append(ss.str());
	ss << current_time.tm_mday;
	path.append(ss.str());
	ss << ".html";
	path.append(ss.str());

	file.open(path);
	file << "Content";
	file.close();
}
