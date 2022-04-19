////////////////////////////////////////////////////////////////////////////////
/*
 * File: callback_sec.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Callback class definition for lazy/cut subtour separation
 * constraints.
 *
 * @acknowledgment Special thanks to Ph.D. Cleder Marcos Schenekemberg.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on November 16, 2020, 00:15 AM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ext/loguru/loguru.hpp"
#include "../../include/ext/cvrpsep/capsep.h"
#include "../../include/ext/cvrpsep/cnstrmgr.h"

#include "../../include/column_generation/callback_sec.hpp"
#include "../../include/utils/constants.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace
{

static const int cDim = 100; // ?
static const int cMaxNbCapCuts = 8; // ?
static const int cMaxNbCombCuts = 20;

/**
 * @brief
*/
int checkForDepot(const int i, const int n)
{
    return i == n ? 0 : i;
}

/*
* @brief
*/
std::vector<double> getDemands(
    const std::shared_ptr<const Instance>& pInst)
{
    std::vector<double> demand(pInst->getNbVertices(), 0);

    for (std::size_t i = 1; i < demand.size(); ++i)
    {
        demand[i] = pInst->getdi(i);
    }

    return demand;
}

/**
 * @brief
*/
std::tuple<int, std::vector<int>, std::vector<int>, std::vector<double>>
    getEdges(
        const utils::Vec2D<double>& xVal,
        const int n)
{
    int nbEdges = 0;
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            if (xVal[i][j] > utils::GRB_EPSILON)
            {
                ++nbEdges;
            }
        }
    }

    std::vector<int> edgeTail;
    std::vector<int> edgeHead;
    std::vector<double> edgeX;

    if (nbEdges > 0)
    {
        edgeTail.reserve(nbEdges + 1);
        edgeHead.reserve(nbEdges + 1);
        edgeX.reserve(nbEdges + 1);
        /* skip first element */
        edgeTail.push_back(0); 
        edgeHead.push_back(0);
        edgeX.push_back(0);

        for (int i = 0; i < n; ++i)
        {
            for (int j = i + 1; j < n; ++j)
            {
                if (xVal[i][j] > utils::GRB_EPSILON)
                {
                    edgeTail.push_back(i == 0 ? n : i);
                    edgeHead.push_back(j);
                    edgeX.push_back(xVal[i][j]);
                }
            }
        }
    }

    return std::make_tuple(nbEdges, edgeTail, edgeHead, edgeX);
}

}

////////////////////////////////////////////////////////////////////////////////

CallbackSEC::CallbackSEC(
    const utils::Vec2D<GRBVar>& x,
    const std::vector<GRBVar> & y,
    const std::shared_ptr<const Instance>& p_inst) :
        mr_x(x),
        mr_y(y),
        mpInst(p_inst)
{}


void CallbackSEC::callback()
{
    try
    {
        if (where == GRB_CB_MIPSOL)
        {
            addCVRPSEPCAP(constrsType::lazy);
        }
        else if (where == GRB_CB_MIPNODE &&
                 getIntInfo(GRB_CB_MIPNODE_STATUS) == GRB_OPTIMAL)
        {
            addCVRPSEPCAP(constrsType::cut);
        }
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "callback() exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "callback(): Unknown Exception");
    }
}

/* -------------------------------------------------------------------------- */


int CallbackSEC::addCVRPSEPCAP(const constrsType cstType)
{
    int nbAdded = 0; // # of lazy/cuts added in this call

    /* get solution: routing */
    auto xVal = getxVarsValues(cstType);
    /* get solution: visitation */
    auto yVal = getyVarsValues(cstType);

    const int n = mpInst->getNbVertices();
    
    auto [nbEdges, edgeTail, edgeHead, edgeX] = getEdges(xVal, n);


    if (nbEdges == 0) return nbAdded;

    /* Parameters of the CVRPSEP */
    char integerAndFeasible;
    double maxViolation = 0;
    auto demands = getDemands(mpInst);
    CnstrMgrPointer cutsCMP, myOldCutsCMP;
    CMGR_CreateCMgr(&cutsCMP, cDim);
    CMGR_CreateCMgr(&myOldCutsCMP, cDim);

    CAPSEP_SeparateCapCuts(n - 1,
                           demands.data(),
                           mpInst->getC(),
                           nbEdges,
                           edgeTail.data(),
                           edgeHead.data(),
                           edgeX.data(),
                           myOldCutsCMP,
                           cMaxNbCapCuts,
                           utils::GRB_EPSILON,
                           utils::GRB_EPSILON,
                           &integerAndFeasible,
                           &maxViolation,
                           cutsCMP);

    for (int c = 0; c < cutsCMP->Size; ++c)
    {
        if (cutsCMP->CPL[c]->CType == CMGR_CT_CAP)
        {
            std::vector<int> list;
            list.reserve(cutsCMP->CPL[c]->IntListSize);
            for (int j = 1; j <= cutsCMP->CPL[c]->IntListSize; ++j)
            {
                list.push_back(
                    checkForDepot(cutsCMP->CPL[c]->IntList[j], n));
            }

            GRBLinExpr xExpr = 0, yExpr = 0;
            double xSum = 0, ySum = 0;
            for (std::size_t i = 0; i < list.size(); ++i)
            {
                for (std::size_t j = 0; j < list.size(); ++j)
                {
                    if (list[i] < list[j])
                    {
                        xExpr += mr_x[list[i]][list[j]];
                        xSum += xVal[list[i]][list[j]];
                    }
                }
                yExpr += mr_y[list[i]];
                ySum += yVal[list[i]];
            }

            for (std::size_t i = 0; i < list.size(); ++i)
            {
                if (cstType == constrsType::lazy)
                {
                    addLazy(xExpr <= yExpr - mr_y[list[i]]);
                    ++nbAdded;
                }
                else if (xSum - ySum + yVal[list[i]] > utils::GRB_EPSILON)
                {
                    addCut(xExpr <= yExpr - mr_y[list[i]]);
                    ++nbAdded;
                }
            }
        }
    }

    CMGR_FreeMemCMgr(&cutsCMP);
    CMGR_FreeMemCMgr(&myOldCutsCMP);

    return nbAdded;
}


utils::Vec2D<double> CallbackSEC::getxVarsValues(const constrsType cstType)
{
    utils::Vec2D<double> xVal(mpInst->getNbVertices(),
                              std::vector<double>(mpInst->getNbVertices(), 0));

    for (int i = 0; i < mpInst->getNbVertices(); ++i)
    {
        for (int j = i + 1; j < mpInst->getNbVertices(); ++j)
        {
            DCHECK_F(i < static_cast<int>(mr_x.size()));
            DCHECK_F(j < static_cast<int>(mr_x[i].size()));
            if (cstType == constrsType::lazy)
            {
                xVal[i][j] = getSolution(mr_x[i][j]);
            }
            else
            {
                xVal[i][j] = getNodeRel(mr_x[i][j]);
            }
        }
    }

    return xVal;
}


std::vector<double> CallbackSEC::getyVarsValues(const constrsType cstType)
{
    std::vector<double> yVal(mpInst->getNbVertices(), 0);

    for (int i = 0; i < mpInst->getNbVertices(); ++i)
    {
        DCHECK_F(i < static_cast<int>(mr_y.size()));
        if (cstType == constrsType::lazy)
        {
            yVal[i] = getSolution(mr_y[i]);
        }
        else
        {
            yVal[i] = getNodeRel(mr_y[i]);
        }
    }

    return yVal;
}
