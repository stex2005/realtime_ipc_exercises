#ifndef LOOP_TIME_STATS_H
#define LOOP_TIME_STATS_H

#include <iostream>
#include <fstream>
#include "time_spec_operation.h"

using std::cout;
using std::endl;
using std::string;

#define MAX_LOOP_TIME_FOR_LOOP_TIME_STATS_US 3000

class loop_time_stats
{
private:
    const int SIZE_OF_BINS_FOR_TERMINAL_DISPLAY_US = 5;
    const int NUMBER_OF_FIRST_CYCLES_TO_BE_SKIPPED_FOR_STATS = 5;

    std::ofstream ofs;           // output file stream
    int t_loop_time_bins[MAX_LOOP_TIME_FOR_LOOP_TIME_STATS_US];   // bins to count loop time
    int t_loop_time_us = 0;               // loop time in us. this will be used as an index of array
    struct timespec t_now,t_diff,t_prev;    // variables for measuring time
    bool flag_file_out = false;
    bool flag_screen_out = false;
    string filename_loop_time_stats;
    long unsigned int cnt = 0;


public:
    enum class output_mode{fileout_only, screenout_only, fileout_and_screenout};
    loop_time_stats(string myfile, output_mode m = output_mode::fileout_only);     // initialize the class object, and open the file with the name myfile
    ~loop_time_stats();  // close the class object
    void loop_starting_point();         // insert this member function at the starting point of a loop
    void store_loop_time_stats();       // execute this member function after the end of loop
private:
    output_mode mode;
};


#endif // LOOP_TIME_STATS_H
