#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <chrono>

namespace fs = std::filesystem;
using namespace std;
//g++ -std=c++17 -O2 -Wall -no-pie main.cpp -o auto_transf
static const unsigned short default_overtime = 1;

static string get_ftime(const fs::path &fpath)
{
    using namespace std::chrono;
    const auto ftime = fs::last_write_time(fpath);
    const auto sctp = time_point_cast<system_clock::duration>(ftime - decltype(ftime)::clock::now() + system_clock::now());
    const time_t cftime = std::chrono::system_clock::to_time_t(sctp); // assuming system_clock
    return string(asctime(localtime(&cftime))); // returns char*
}

static void __copy(fs::path &from, fs::path &to)
{
    while (!fs::exists(from))
        sleep(default_overtime);

    if (fs::exists(to))
        fs::remove(to);

    fs::copy(from, to);
}

static void __print_res(fs::path &from)
{
    time_t  today;
    tm      *today_struct;
    time(&today);
    today_struct = localtime(&today);

    string fname(from.filename());
    cout<<setw(fname.size() + 3)<<left<<setfill(' ')
        <<fname<<" was transferred "
        <<today_struct->tm_hour<<':'
        <<today_struct->tm_min <<':'
        <<today_struct->tm_sec <<'\t'
        <<today_struct->tm_year + 1900<< '.'
        <<today_struct->tm_mon  + 1   << '.'
        <<today_struct->tm_mday       <<endl;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        return ENODATA;

    fs::path from(argv[1]), to(argv[2]);
    unsigned int overtime = (argc > 3)? atoi(argv[3]): default_overtime;

    while (!fs::exists(from)) {
        sleep(default_overtime);
    }

    string last_modif_time(get_ftime(from));

    __copy(from, to);
    __print_res(from);

    while(true) {
        while (!fs::exists(from))
            sleep(default_overtime);

        string curr_modif_time(get_ftime(from));

        if (last_modif_time != curr_modif_time) {
            __copy(from, to);
            last_modif_time = curr_modif_time;
            __print_res(from);
        }

        sleep(overtime);
    }

    return 0;
}
