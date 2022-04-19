////////////////////////////////////////////////////////////////////////////////
/*
 * File: tools.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Helper functions definition.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on May 4, 2021, 05:22 PM
 * 
 * References:
 * [1] https://bit.ly/3emVjgY
 */
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <gurobi_c++.h>

#include "../../include/ext/loguru/loguru.hpp"

#include "../../include/utils/tools.hpp"
#include "../../include/instance.hpp"
#include "../../include/utils/constants.hpp"


double utils::tools::correctVal(const double val,
                                const double eps /*= utils::GRB_EPSILON*/)
{
    return std::abs(std::round(val) - val) < eps ? std::round(val) : val;
}


bool utils::tools::doubleEquals(const double a,
                                const double b,
                                const double eps /*= 1e-6*/)
{
    return std::abs(a - b) < eps;
}


void utils::tools::printVar(const GRBVar& var)
{
    if (varExists(var) && correctVal(var.get(GRB_DoubleAttr_X)))
    {
        RAW_LOG_F(INFO, "  %s = \t%.2f",
            var.get(GRB_StringAttr_VarName).c_str(),
            var.get(GRB_DoubleAttr_X));
    }
}


void utils::tools::printVars(const std::vector<GRBVar>& vars)
{
    for (const auto& var : vars)
    {
        printVar(var);
    }
}


void utils::tools::printVars(const utils::Vec2D<GRBVar>& vars2d)
{
    for (const auto& vars1d : vars2d)
    {
        printVars(vars1d);
    }
}


void utils::tools::printVars(const utils::Vec3D<GRBVar>& vars3d)
{
    for (const auto& vars2d : vars3d)
    {
        printVars(vars2d);
    }
}


void utils::tools::printVars(const utils::Vec4D<GRBVar>& vars4d)
{
    for (const auto& vars3d : vars4d)
    {
        printVars(vars3d);
    }
}


bool utils::tools::varExists(const GRBVar& var)
{
    /* see [1] */
    return var.index() != -2 && var.index() != -1;
}
