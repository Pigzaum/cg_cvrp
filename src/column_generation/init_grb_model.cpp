////////////////////////////////////////////////////////////////////////////////
/*
 * File: init_grb_model.cpp
 *
 * @brief GRB model initialization functions definitions.
 *
 * Created on April 16, 2022, 05:56 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <gurobi_c++.h>

#include "../../include/ext/loguru/loguru.hpp"

#include "../../include/column_generation/init_grb_model.hpp"
#include "../../include/instance.hpp"
#include "../../include/column_generation/column.hpp"
#include "../../include/column_generation/set_covering_lp.hpp"
#include "../../include/utils/tools.hpp"


std::vector<GRBVar> init::routeVars(
    GRBModel& model,
    const std::vector<Column>& columns)
{
    DRAW_LOG_F(INFO, "\tInitializing y route vars...");

    std::vector<GRBVar> y;
    y.reserve(columns.size());

    for (const auto& r : columns)
    {
        std::ostringstream oss;
        oss << "y_" << y.size();
        y.push_back(model.addVar(
            0, GRB_UNBOUNDED, r.getCost(), GRB_CONTINUOUS, oss.str()));
    }

    return y;
}


std::vector<GRBVar> init::visitVars(
    GRBModel& model,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tInitializing y visit vars...");

    std::vector<GRBVar> y;
    y.reserve(pInst->getNbVertices());

    for (int i = 0; i < pInst->getNbVertices(); ++i)
    {
        std::ostringstream oss;
        oss << "y_" << i;
        y.push_back(model.addVar(i == 0 ? 1 : 0,
                                 1,
                                 0, // the coeff will be updatew with pi values
                                 GRB_BINARY,
                                 oss.str()));
    }

    return y;
}


utils::Vec2D<GRBVar> init::routingVars(
    GRBModel& model,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tInitializing x routing vars...");

    utils::Vec2D<GRBVar> x(pInst->getNbVertices(),
                           std::vector<GRBVar>(pInst->getNbVertices()));

    for (int i = 0; i < pInst->getNbVertices(); ++i)
    {
        for (int j = i + 1; j < pInst->getNbVertices(); ++j)
        {
            std::ostringstream oss;
            oss << "x_" << i << "_" << j;
            x[i][j] = model.addVar(0,
                                   i == 0 ? 2 : 1,
                                   pInst->getcij(i, j),
                                   i == 0 ? GRB_INTEGER : GRB_BINARY,
                                   oss.str());
        }
    }

    return x;
}


void init::coveringConstrs(GRBModel& model,
                           std::vector<GRBConstr>& constrs,
                           const std::vector<GRBVar>& y,
                           const std::vector<Column>& columns,
                           const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tInitializing covering constrs...");
    DCHECK_F(y.size() == columns.size());

    for (int i = 0; i < pInst->getNbVertices(); ++i)
    {
        GRBLinExpr lhs = 0;
        for (int j = 0; j < static_cast<int>(columns.size()); ++j)
        {
            DCHECK_F(j < static_cast<int>(y.size()));
            lhs += columns[j].contains(i) * y[j];
        }

        std::ostringstream oss;
        oss << "C1_" << i;
        constrs.push_back(model.addConstr(lhs >= 1, oss.str()));
    }
}


void init::kRoutesConstr(GRBModel& model,
                         std::vector<GRBConstr>& constrs,
                         const std::vector<GRBVar>& y,
                         const std::vector<Column>& columns,
                         const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tInitializing k-routes constrs...");
    DCHECK_F(y.size() == columns.size());

    GRBLinExpr lhs = 0;
    for (int j = 0; j < static_cast<int>(columns.size()); ++j)
    {
        lhs += y[j];
    }

    std::ostringstream oss;
    oss << "C2";
    constrs.push_back(model.addConstr(lhs == pInst->getK(), oss.str()));
}


void init::matchingConstrs(GRBModel& model,
                           const std::vector<GRBVar>& y,
                           const utils::Vec2D<GRBVar>& x,
                           const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tInitializing matching constrs...");

    GRBLinExpr lhs = 0;
    for (int i = 0; i < pInst->getNbVertices(); ++i)
    {
        lhs = 0;
        for (int j = 0; j < pInst->getNbVertices(); ++j)
        {
            if (i < j && utils::tools::varExists(x[i][j]))
            {
                lhs += x[i][j];
            }
            else if (i > j && utils::tools::varExists(x[j][i]))
            {
                lhs += x[j][i];
            }
        }

        if (lhs.size())
        {
            std::ostringstream oss;
            oss << "C1_" << i;
            model.addConstr(lhs == 2 * y[i], oss.str());
        }
    }
}


void init::capacityConstr(GRBModel& model,
                          const std::vector<GRBVar>& y,
                          const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tInitializing capacity constrs...");

    GRBLinExpr lhs = 0;
    for (int i = 0; i < pInst->getNbVertices(); ++i)
    {
        lhs += pInst->getdi(i) * y[i];
    }

    std::ostringstream oss;
    oss << "C2";
    model.addConstr(lhs <= pInst->getC(), oss.str());
}
