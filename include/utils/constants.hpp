////////////////////////////////////////////////////////////////////////////////
/*
 * File: constants.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Constants file.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on January 7, 2021, 5:56 PM
 * 
 * References:
 * [1] https://bit.ly/2RHRnfm.
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_CONSTANTS_HPP
#define UTILS_CONSTANTS_HPP

namespace utils
{

/**
 * @brief GUROBI's epsilon [1].
*/
const static double GRB_EPSILON = 1e-5;

/**
 * @brief File extensions (used in GUROBI's API)
*/
const std::string ILP_EXT = ".ilp";
const std::string LP_EXT = ".lp";
const std::string SOL_EXT = ".sol";
const std::string JSON_EXT = ".json";

} // utils namespace

#endif // UTILS_CONSTANTS_HPP