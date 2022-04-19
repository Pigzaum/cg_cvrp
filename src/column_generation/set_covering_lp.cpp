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

/* ----------------------------- private methods ---------------------------- */

void SetCoveringLp::initModel(const std::vector<Column>& columns)
{
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
