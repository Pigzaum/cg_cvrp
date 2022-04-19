////////////////////////////////////////////////////////////////////////////////
/*
 * File: init_pool.hpp
 *
 * @brief Functions to generate an initial pool of columns.
 *
 * Created on April 17, 2022, 02:30 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef INIT_POOL_HPP
#define INIT_POOL_HPP

#include <memory>
#include <vector>

class Instance;
class Column;

namespace initPool
{

/**
 * @brief Randomly generates an initial pool of columns. This algorithm
 * generates n covers. The cost of each column is computed by running the LKH
 * heuristic and finding a TSP solution.
 * 
 * @param [int]: number of covers (each vertex in at least one column).
 * @param [std::shared_ptr<const Instance>]: pointer to instance.
 * @return std::vector<Route>: columns obtained from n covers.
 */
std::vector<Column> randomCovers(const int n,
                                 const std::shared_ptr<const Instance>& pInst);

}

#endif // INIT_POOL_HPP
