////////////////////////////////////////////////////////////////////////////////
/*
 * File: tsp_bcsl_ilp.cpp
 *
 * @brief Travelling salesman problem (TSP) CG Bixby-Coullard-Simchi-Levi (BCSL)
 * subproblem [1] class declaration. It is a prize collection TSP. This model is
 * used as the subproblem of the column generation algorithm as described by
 * [1, 2]. Note, however, that we only use the subtour elimination constraints
 * (SEC) rather than others cuts as [1].
 *
 * Created on April 19, 2022, 01:26 AM
 * 
 * References:
 * [1] A. Bixby, C. Coullard, and D. Simchi-Levi. The capacitated
 * prize-collecting traveling salesman problem. Working paper, Department of
 * Industrial Engineering and Engineering Management, Northwestern University,
 * Evanston, IL, 1997.
 * [2] P. Toth and D. Vigo. The Vehicle Routing Problem, Discrete Mathematics
 * and Applications, SIAM, 2002.
 * [3] https://bit.ly/2Kcnm6C
 * [4] https://bit.ly/2N6iMbl
 */
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ext/loguru/loguru.hpp"

#include "../../include/column_generation/pctsp_ilp.hpp"
#include "../../include/instance.hpp"
#include "../../include/column_generation/callback_sec.hpp"
#include "../../include/column_generation/column.hpp"
#include "../../include/column_generation/init_grb_model.hpp"
#include "../../include/column_generation/set_covering_lp.hpp"
#include "../../include/utils/constants.hpp"


/* ----------------------------- private methods ---------------------------- */

namespace
{

const std::string cBaseName = "tsp_bcsl_";

}

/* -------------------------------------------------------------------------- */


PctspIlp::PctspIlp(const std::shared_ptr<SetCoveringLp>& pRMP,
                   const std::shared_ptr<const Instance>& pInst) :
    BaseLp(pRMP->getGRBEnv(), pInst),
    mpRMP(pRMP),
    mpCb(nullptr)
{
    initModel();
}


std::pair<Column, double> PctspIlp::extractColumn() const
{
    double rc = 0;
    Column column(mpInst);

    try
    {
        rc -= mpRMP->getDual(mpInst->getNbVertices()); // |V| constraint dual
        rc += mModel.get(GRB_DoubleAttr_ObjVal);

        if (rc < 0)
        {
            double cost = mModel.get(GRB_DoubleAttr_ObjVal);
            for (int i = 0; i < mpInst->getNbVertices(); ++i)
            {
                if (m_y[i].get(GRB_DoubleAttr_X) > utils::GRB_EPSILON)
                {
                    column.addVertex(i);
                    cost -= m_y[i].get(GRB_DoubleAttr_Obj);
                }
            }

            column.setCost(cost);
        }
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(FATAL, "PctspIlp::extractColumn(): C-Exp: %s",
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "PctspIlp::extractColumn(): Unknown Exception");
    }

    return {column, rc};
}

/* ----------------------------- private methods ---------------------------- */

void PctspIlp::initModel()
{
    try
    {
        const std::string modelName = cBaseName + mpInst->getName();
        mModel.set(GRB_StringAttr_ModelName, modelName);

        // variables
        m_x = init::routingVars(mModel, mpInst);
        m_y = init::visitVars(mModel, mpRMP, mpInst);

        // constraints
        init::matchingConstrs(mModel, m_y, m_x, mpInst);
        init::capacityConstr(mModel, m_y, mpInst);

        // setup CVRPSEP callback
        mModel.set(GRB_IntParam_LazyConstraints, 1); // see [3]
        mModel.set(GRB_IntParam_PreCrush, 1); // see [4]

        mpCb = std::make_shared<CallbackSEC>(m_x, m_y, mpInst);
        mModel.setCallback(mpCb.get());

        mModel.update();
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(FATAL, "PctspIlp::initModel(): C-Exp: %s",
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "PctspIlp::initModel(): Unknown Exception");
    }
}
