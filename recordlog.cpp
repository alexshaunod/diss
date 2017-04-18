#include "recordlog.h"

/**
*	@file recordlog.cpp
*   @desc Creates/maintains a HTML file to store information on the pedestrians detected in 
*   a table format.
*
*   @param ofstream file - the output file stream
*	@param string current_date - for naming files
*   @param int total_records - for naming files
*
*	@author Alex O'Donnell
*	@version 1.3
*/

/**
*  @desc Initialises a HTML file within the record_log directory, setting up the page
*  style and the table.
*
*  @param string videoPath - displays the path of the video file on the page
*  @param int bgs_history - history length used for the video
*  @param double bgs_threshold - dist2threshold used for the video
*/
void RecordLog::init_log(string videoPath, int bgs_history, double bgs_threshold)
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
	file << "<style>\n";
	file << "body {\n background-color: #ffeecc;\n}\n";
	file << "table {\n width:100%;\n}\n";
	file << "table, tr, th, td {\n border: 1px solid black;  border-collapse: collapse;\n}\n";
	file << "</style>\n";
	file << "<body>\n";
	file << "<p>Source file: " << videoPath << "</p>\n";
	file << "<p>BGS History Length: " << bgs_history << "</p>\n";
	file << "<p>Distance to Threshold: " << bgs_threshold << "</p>\n";
	file << "<table>\n";
	file << "<tr>\n" << "<th>Frame Number</th>\n"
		"<th>Timestamp</th>\n" <<
		"<th>Source</th>\n" <<
		"<th>Analysis</th>\n" << 
		"<th>Interpretation</th>\n" << "</tr>\n";
	
}

/**
*  @desc Creates a new table row and enters information into the log.
*
*  @param int frame_num - number of frames into the video
*  @param int mill_seconds - the timestamp of the video
*  @param Mat src_image - the source of the large shape
*  @param Mat contour_image - the PeopleFinder interpretation
*  @param string verdict - the classification
*/
void RecordLog::new_record(int frame_num, int mill_seconds, Mat src_image, Mat contour_image, string verdict)
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

	file << "<tr>\n" <<  "<td>" << frame_num << "</td>" <<
		"<td>" << timestamp << "</td>\n" << 
		"<td><img src = \"" << src_image_path << "\"></td>\n" << 
		"<td><img src = \"" << con_image_path << "\"></td>\n" <<
		"<td>" << verdict << "</td>\n" << "</tr>\n";
}

/**
*  @desc Converts the current date from time_t into a readable format
*
*  @returns string curr_date - current date in YY:MM:DD format
*/
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

/**
*  @desc Saves the image so it can be displayed in the HTML table.
*
*  @param string type_name - whether the image is a source image or PeopleFinder interpretation
*  @param Mat *image - the image to be saved
*
*  @returns image_path - path for the table record so it can display the image
*/
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
	file << "</body>\n";
	file << "</html>";

	file.close();
}
