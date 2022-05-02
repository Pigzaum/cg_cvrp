////////////////////////////////////////////////////////////////////////////////
/*
 * File: cg.cpp
 *
 * @brief Column generation algorithm class definition.
 *
 * Created on April 18, 2022, 11:50 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ext/loguru/loguru.hpp"

#include "../../include/column_generation/cg.hpp"
#include "../../include/column_generation/column.hpp"
#include "../../include/column_generation/set_covering_lp.hpp"
#include "../../include/column_generation/pctsp_ilp.hpp"
#include "../../include/utils/stopwatch.hpp"


Cg::Cg(const ConfigParameters::cg& params,
       const std::vector<Column>& initialColumns,
       const std::shared_ptr<const Instance>& pInst) :
    mParams(params),
    mpRMP(std::make_shared<SetCoveringLp>(initialColumns, pInst)),
    mpInst(pInst)
{
}


bool Cg::execute(const ConfigParameters::solver& solverParams)
{
    utils::Stopwatch stopwatch("cg::execute");
    bool solved = false;
    int iter = 0;

    do
    {
        // solve the restricted main problem
        solved = mpRMP->optimize(solverParams);

        RAW_LOG_F(INFO, "CG(%d): UB %.2f, %d colums, %.2fs ",
            iter, mpRMP->get(GRB_DoubleAttr_ObjVal), 0, stopwatch.elapsed());

        ++iter;
    }
    while (solved &&
           iter < mParams.maxIter_ &&
           stopwatch.elapsed() < mParams.timeLimit_ &&
           generateColumn());

    return solved;
}

/* ---------------------------- private methods ----------------------------- */

bool Cg::generateColumn()
{
    // build subproblem
    PctspIlp subproblem(mpRMP, mpInst);

    // solve subproblem
    subproblem.optimize();

    auto [column, rc] = subproblem.extractColumn();

    DRAW_LOG_F(INFO, "rc: %.2f", rc);

    if (rc < 0)
    {
        // append column
        mpRMP->appendColumn(column);
        return true;
    }

    return false;
}
