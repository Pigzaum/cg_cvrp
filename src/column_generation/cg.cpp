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
#include "../../include/utils/constants.hpp"
#include "../../include/utils/stopwatch.hpp"


Cg::Cg(const ConfigParameters::cg& params,
       const std::vector<Column>& initialColumns,
       const std::shared_ptr<const Instance>& pInst) :
    mParams(params),
    mpRMP(std::make_shared<SetCoveringLp>(initialColumns, pInst)),
    mpSubproblem(std::make_shared<PctspIlp>(mpRMP, pInst)),
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

        RAW_LOG_F(INFO, "CG(%d): UB %.2f, %d columns, %.2fs ",
            iter, mpRMP->get(GRB_DoubleAttr_ObjVal), mpRMP->getNbCols(),
            stopwatch.elapsed());

        ++iter;
    }
    while (solved &&
           iter < mParams.maxIter_ &&
           stopwatch.elapsed() < mParams.timeLimit_ &&
           generateColumn());

    /* post processing step: if solution is fractional, then impose integrality
       on y variables and solve it again */
    if (!mpRMP->isSolutionInteger())
    {
        RAW_LOG_F(INFO, "Imposing integrality on y vars...");
        mpRMP->imposeIntegrality();
        mpRMP->optimize();
        RAW_LOG_F(INFO, "ILP: UB %.2f, LB %.2f, gap %.2f, %.2fs ",
            mpRMP->get(GRB_DoubleAttr_ObjVal),
            mpRMP->get(GRB_DoubleAttr_ObjBound),
            mpRMP->get(GRB_DoubleAttr_MIPGap),
            stopwatch.elapsed());
    }

    return solved;
}

/* ---------------------------- private methods ----------------------------- */

bool Cg::generateColumn()
{
    // update the y vars of coeff with the dual values from the main problem
    mpSubproblem->updateVisitVarsObjCoeff();

    // solve subproblem
    mpSubproblem->optimize();

    auto [column, rc] = mpSubproblem->extractColumn();

    if (rc < -utils::GRB_EPSILON)
    {
        mpRMP->appendColumn(column);
        return true;
    }

    return false;
}
