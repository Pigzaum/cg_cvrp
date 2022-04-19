////////////////////////////////////////////////////////////////////////////////
/*
 * File: cg.hpp
 *
 * @brief Column generation algorithm class declaration. This algorithm follows
 * the idea described at chaper 4 of [1]. It is used the approach of [2] to
 * generate new columns.
 *
 * Created on April 18, 2022, 11:12 PM
 * 
 * References:
 * [1] P. Toth and D. Vigo. The Vehicle Routing Problem, Discrete Mathematics
 * and Applications, SIAM, 2002.
 * [2] A. Bixby, C. Coullard, and D. Simchi-Levi. The capacitated prize-
 * collecting traveling salesman problem. Working paper, Department of
 * Industrial Engineering and Engineering Management, Northwestern University,
 * Evanston, IL, 1997.
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef CG_HPP
#define CG_HPP

#include <memory>
#include <vector>

#include "../config_parameters.hpp"

class Column;
class Instance;
class SetCoveringLp;

class Cg
{
public:

    Cg() = default;
    Cg(const Cg& other) = default;
    Cg(Cg&& other) = default;
    ~Cg() = default;
    Cg& operator=(const Cg& other) = default;
    Cg& operator=(Cg&& other) = default;

    Cg(const ConfigParameters::cg& params,
       const std::vector<Column>& initialColumns,
       const std::shared_ptr<const Instance>& pInst);

    bool execute(const ConfigParameters::solver& solverParams);

private:

    ConfigParameters::cg mParams;

    // restricted main problem (RMP)
    std::shared_ptr<SetCoveringLp> mpRMP;
    // pointer to instance
    std::shared_ptr<const Instance> mpInst;

    /**
     * @brief Generate a new column by solving the subproblem and add it to the
     * RMP if the column has negative reduced cost. A TSP-like model [2] is
     * solved to generate the new column.
     * 
     * @return [bool]: true if a new generate column was added to the RMP. False
     * otherwise.
     */
    bool generateColumn();
};

#endif // CG_HPP
