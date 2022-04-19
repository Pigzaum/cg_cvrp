////////////////////////////////////////////////////////////////////////////////
/*
 * File: stopwatch.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Stopwatch class implementation.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on June 17, 2020, 12:39 PM
 * 
 * References:.
 */
////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <vector>

#include "../../include/ext/loguru/loguru.hpp"

#include "../../include/utils/stopwatch.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace detail
{

} // detail namespace

////////////////////////////////////////////////////////////////////////////////

// template <class Fn>
// utils::Stopwatch<Fn>::Stopwatch(const char* name) :
utils::Stopwatch::Stopwatch(const char* name) :
    m_name(name)
{
    start();
}


// template <class Fn>
// utils::Stopwatch<Fn>::Stopwatch(const std::string &name) :
utils::Stopwatch::Stopwatch(const std::string &name) :
    m_name(name.c_str())
{
    start();
}


// template <class Fn>
// utils::Stopwatch<Fn>::~Stopwatch()
utils::Stopwatch::~Stopwatch()
{
    if (!m_stopped)
    {
        stop();
    }
}


// template <class Fn>
// double utils::Stopwatch<Fn>::elapsed() const
double utils::Stopwatch::elapsed() const
{
    auto end = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::duration<double>>(
        end - m_start).count();
}


double utils::Stopwatch::lap()
{
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
        end - m_last_time).count();
    m_last_time = end;
    return duration;
}


// template <class Fn>
// void utils::Stopwatch<Fn>::start()
void utils::Stopwatch::start()
{
    m_stopped = false;
    m_start = std::chrono::high_resolution_clock::now();
    m_last_time = m_start;
}


// template <class Fn>
// void utils::Stopwatch<Fn>::stop()
double utils::Stopwatch::stop()
{
    auto end = std::chrono::high_resolution_clock::now();

    m_stopped = true;

    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
        end - m_start).count();

    // RAW_LOG_F(INFO, "%s took %.3fs", m_name, duration);
    // RAW_LOG_F(INFO, "%.3f", duration);
    // m_func({m_name, duration});
    return duration;
}