////////////////////////////////////////////////////////////////////////////////
/*
 * File: set_covering_lp.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief CG CVRP main problem definition.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on April 16, 2022, 02:42 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ext/loguru/loguru.hpp"

#include "../../include/column_generation/set_covering_lp.hpp"
#include "../../include/instance.hpp"
#include "../../include/column_generation/init_grb_model.hpp"
#include "../../include/column_generation/column.hpp"
#include "../../include/utils/constants.hpp"
#include "../../include/utils/tools.hpp"

/* ----------------------------- private methods ---------------------------- */

namespace
{

const std::string cBaseName = "set_covering_";

}

/* -------------------------------------------------------------------------- */


SetCoveringLp::SetCoveringLp(const std::vector<Column>& columns,
                             const std::shared_ptr<const Instance>& pInst) :
    BaseLp(pInst)
{
    initModel(columns);
}


void SetCoveringLp::appendColumn(const Column& route)
{
    try
    {
        const int j = m_y.size(); // column index of the new y variable

        // create new y variable
        {
            std::string name = "y_" + std::to_string(j);
            double coeff = route.getCost();
            m_y.push_back(mModel.addVar(0, 1, coeff, GRB_CONTINUOUS, name));
        }

        // add the new y's cover constrs coefficient (first |V| constraints)
        for (int v : route)
        {
            DCHECK_F(v >= 0 && v < mpInst->getNbVertices());
            DCHECK_F(v >= 0 && v < static_cast<int>(mConstrs.size()));
            mModel.chgCoeff(mConstrs[v], m_y[j], 1);
        }

        // add the new y's k routes constraint (|V|th constraint)
        DCHECK_F(mpInst->getNbVertices() < static_cast<int>(mConstrs.size()));
        mModel.chgCoeff(mConstrs[mpInst->getNbVertices()], m_y[j], 1);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(FATAL, "SetCoveringLp::appendColumn(): C-Exp: %s",
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "SetCoveringLp::appendColumn(): Unknown Exception");
    }
}


int SetCoveringLp::getNbCols() const
{
    return m_y.size();
}


double SetCoveringLp::getDual(const int i) const
{
    DCHECK_F(i >= 0 && i < static_cast<int>(mConstrs.size()));

    double pi = 0;

    try
    {
        pi = mConstrs[i].get(GRB_DoubleAttr_Pi);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(FATAL, "SetCoveringLp::getDual(): C-Exp: %s",
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "SetCoveringLp::getDual(): Unknown Exception");
    }

    return pi;
}


void SetCoveringLp::imposeIntegrality()
{
    try
    {
        // convert variables y to binary
        std::for_each(std::begin(m_y), std::end(m_y),
            [](GRBVar &y)
            {
                y.set(GRB_CharAttr_VType, GRB_BINARY);
            });
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(FATAL, "SetCoveringLp::imposeIntegrality(): C-Exp: %s",
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL,
            "SetCoveringLp::imposeIntegrality(): Unknown Exception");
    }
}


bool SetCoveringLp::isSolutionInteger() const
{
    int nbSolCols = 0;

    try
    {
        for (int i = 0; i < static_cast<int>(m_y.size()); ++i)
        {
            if (utils::tools::varExists(m_y[i]) &&
                m_y[i].get(GRB_DoubleAttr_X) > utils::GRB_EPSILON)
            {
                ++nbSolCols;
            }
        }
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(FATAL, "SetCoveringLp::isSolutionInteger(): C-Exp: %s",
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL,
            "SetCoveringLp::isSolutionInteger(): Unknown Exception");
    }

    return nbSolCols == mpInst->getK();
}

/* ----------------------------- private methods ---------------------------- */

void SetCoveringLp::initModel(const std::vector<Column>& columns)
{
    DRAW_LOG_F(INFO, "Building set covering problem...");

    try
    {
        const std::string modelName = cBaseName + mpInst->getName();
        mModel.set(GRB_StringAttr_ModelName, modelName);

        // variables
        m_y = init::routeVars(mModel, columns);

        // constraints
        init::coveringConstrs(mModel, mConstrs, m_y, columns, mpInst);
        init::kRoutesConstr(mModel, mConstrs, m_y, columns, mpInst);

        mModel.update();
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(FATAL, "SetCoveringLp::initModel(): C-Exp: %s",
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "SetCoveringLp::initModel(): Unknown Exception");
    }
}
