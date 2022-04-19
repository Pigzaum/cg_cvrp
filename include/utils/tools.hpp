////////////////////////////////////////////////////////////////////////////////
/*
 * File: tools.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Helper functions declaration.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on April 5, 2021, 07:51 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_TOOLS_HPP
#define UTILS_TOOLS_HPP

#include <algorithm>
#include <iomanip>
#include <iterator>
#include <memory>
#include <sstream>

#include "../ext/loguru/loguru.hpp"

#include "constants.hpp"
#include "multi_vector.hpp"

class GRBModel;
class GRBVar;
class Instance;

namespace utils
{

namespace tools
{

/**
 * @brief Correct binary to avoid infeasibility.
 * @param [double]:.
 * @return [double]:.
*/
double correctVal(const double val, const double eps = utils::GRB_EPSILON);

/**
 * @brief Check if the two given double values are (almost) equal.
 * @param [double]: first value a.
 * @param [double]: second value b.
 * @param [double]: (optinal) epsilon value.
 * @return [bool]: true if |a - b| < eps, false otherwise.
 */
bool doubleEquals(const double a,
                  const double b,
                  const double eps = utils::GRB_EPSILON);

/**
 * @brief Prints the given STL container in the output console.
 * @param [const C<T>&]: STL container.
*/
template <template <typename> typename C, typename T>
void printContainer(const C<T>& c)
{
    std::stringstream oss;
    oss << "[";
    std::copy(std::begin(c), std::end(c), std::ostream_iterator<T>(oss, "\t"));
    oss << std::setw(0) << "]";
    RAW_LOG_F(INFO, "%s", oss.str().c_str());
}

/**
 * @brief Prints the given 2D STL container in the output console.
 * @param [const C<C<T>>&]: 2D STL container.
*/
template <template <typename> typename C, typename T>
void print2DContainer(const C<C<T>>& c)
{
    std::for_each(std::begin(c), std::end(c), printContainer<C, T>);
}

void printVar(const GRBVar& var);

void printVars(const std::vector<GRBVar>& vars);

void printVars(const utils::Vec2D<GRBVar>& vars);

void printVars(const utils::Vec3D<GRBVar>& vars);

void printVars(const utils::Vec4D<GRBVar>& vars);

/**
 * @brief Checks whether the given GRB var exists in the model or not.
 * @param [const GRBVar&]: GRB var.
 * @return [bool]:.
*/
bool varExists(const GRBVar& var);

} // tools namespace

} // utils namespace

#endif // UTILS_TOOLS_HPP
