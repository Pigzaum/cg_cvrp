////////////////////////////////////////////////////////////////////////////////
/*
 * File: stopwatch.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Stopwatch class, used for profiling performance.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on June 17, 2020, 12:03 PM
 * 
 * References:.
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_STOPWATCH_HPP
#define UTILS_STOPWATCH_HPP

#include <chrono>
#include <string>

//////////////////////////////// Define macros /////////////////////////////////

#ifndef __FUNCSIG__
    #define __FUNCSIG__ __PRETTY_FUNCTION__
#endif

#define PROFILING 1
#ifdef PROFILING
    #define PROFILE_SCOPE(name) utils::Stopwatch stopwatch##__LINE__(name)
    #define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCSIG__)
#else
    #define PROFILE_SCOPE(name)
#endif

////////////////////////////////////////////////////////////////////////////////

namespace utils
{

// template <class Fn>
class Stopwatch
{
public:

    Stopwatch() = delete;
    /**
     * @brief default constructor, copy constructor, move constructor, copy
     * assignment, move assignment.
    */
    Stopwatch(const Stopwatch &other) = default;
    Stopwatch(Stopwatch &&other) = default;
    Stopwatch& operator=(const Stopwatch &other) = default;
    Stopwatch& operator=(Stopwatch &&other) = default;

    /**
     * @brief constructor.
     * @param: const char*: scope name.
    */
    // Stopwatch(const char* name, Fn &&func);
    Stopwatch(const char* name);
    Stopwatch(const std::string &name);

    /**
     * @destructor
    */
    ~Stopwatch();

    /**
     * @brief Elapsed time in seconds since the start of the stopwacth.
     * @return double: time in seconds.
    */
    double elapsed() const;

    /**
     * @brief Elapsed time in seconds since the last call of lap.
     * @return double: time in seconds.
    */
    double lap();

    /**
     * @brief Starts (or re-starts) the timer.
    */
    void start();

    /**
     * @brief Stops the timer.
    */
    double stop();

private:

    using time_pt = std::chrono::time_point<std::chrono::system_clock>;

    /**
     * @brief scope name.
    */
    const char* m_name;

    /**
     * @brief start time point.
    */
    time_pt m_start;

    /**
     * @brief.
    */
    time_pt m_last_time;

    /**
     * @brief.
    */
    bool m_stopped;

};

} // namespace utils

#endif // UTILS_STOPWATCH_HPP