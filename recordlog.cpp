#include "recordlog.h"

void RecordLog::init_log(const char *videoPath)
{
	stringstream ss;
	string path;
	

	path = "record_log/";
	current_date = get_date();
	ss << current_date;
	ss << ".html";
	path.append(ss.str());

	file.open(path);
	file << "<html>\n";
	file << "<p>Source file: " << videoPath << "</p>\n";
	file << "<table style=\"width:100%\">\n";
	file << "<tr>\n" << "<th>Timestamp</th>\n" <<
		"<th>Source</th>\n" <<
		"<th>Analysis</th>\n" << 
		"<th>Interpretation</th>\n" << "</tr>\n";
	
}

void RecordLog::new_record(int mill_seconds, Mat src_image, Mat contour_image, string verdict)
{
	int hours, mins, secs;
	string timestamp, src_image_path, con_image_path;
	stringstream ss;

	convert_milliseconds(mill_seconds, &hours, &mins, &secs);

	total_records++;

	ss << setw(2) << setfill('0') << hours;
	ss << ":";
	ss << setw(2) << setfill('0') << mins;
	ss << ":";
	ss << setw(2) << setfill('0') << secs;
	timestamp.append(ss.str());

	src_image_path = save_image("_SRC_IMAGE_", &src_image);
	con_image_path = save_image("_CNTR_IMAGE_", &contour_image);

	file << "<tr>\n" << "<td>" << timestamp << "</td>\n" << 
		"<td><img src = \"" << src_image_path << "\"></td>\n" << 
		"<td><img src = \"" << con_image_path << "\"></td>\n" <<
		"<td>" << verdict << "</td>\n" << "</tr>\n";
}

string RecordLog::get_date()
{
	string curr_date;
	stringstream ss;
	time_t now;
	time(&now);
	struct tm current_time;
	localtime_s(&current_time, &now);

	ss << 1900 + current_time.tm_year;
	ss << "_";
	ss << 1 + current_time.tm_mon;
	ss << "_";
	ss << current_time.tm_mday;
	curr_date.append(ss.str());

	return curr_date;
}

string RecordLog::save_image(string type_name, Mat *image)
{
	string image_path;
	stringstream ss;

	ss << current_date;
	ss << type_name;
	ss << total_records;
	ss << ".png";
	image_path.append(ss.str());
	imwrite("record_log/" + image_path, *image);

	return image_path;
}

void RecordLog::convert_milliseconds(int mill_seconds, int *hh, int *mm, int *ss)
{
	*ss = (int)(mill_seconds / 1000) % 60;
	*mm = (int)((mill_seconds / (1000 * 60)) % 60);
	*hh = (int)((mill_seconds / (1000 * 60 * 60)) % 24);
}

void RecordLog::close_log()
{
	file << "</table>\n";
	file << "</html>";

	file.close();
}
