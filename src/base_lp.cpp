////////////////////////////////////////////////////////////////////////////////
/*
 * File: base_lp.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Base class definition for linear programming models using GUROBI's
 * API.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on July 20, 2021, 09:11 AM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include <filesystem>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/base_lp.hpp"
#include "../include/instance.hpp"
#include "../include/utils/constants.hpp"

/////////////////////////////// Helper functions ///////////////////////////////

namespace
{

/**
 * @brief Workaround to silence annoying GUROBI's outputs.
 */
GRBModel createModel(GRBEnv& env)
{
    try
    {
        env.set(GRB_IntParam_OutputFlag, 0);
        env.start();
        return GRBModel(env);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "BaseLp::createModel: error code: %d", 
            e.getErrorCode());
        RAW_LOG_F(FATAL, "BaseLp::createModel: C-Exp: %s",
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "BaseLp::createModel: unknown Exception");
    }

    return GRBModel(env); // to avoid warning
}

}

////////////////////////////////////////////////////////////////////////////////

BaseLp::BaseLp(const std::shared_ptr<const Instance>& pInst) :
    mStatus(-1),
    mpInst(pInst),
    mEnv(true),
    mModel(createModel(mEnv))
{
}


BaseLp::BaseLp(GRBEnv& env, const std::shared_ptr<const Instance>& pInst) :
    mStatus(-1),
    mpInst(pInst),
    mEnv(env),
    mModel(mEnv)
{
}

int BaseLp::get(GRB_IntParam param) const
{
    return mModel.get(param);
}


double BaseLp::get(GRB_DoubleParam param) const
{
    return mModel.get(param);
}


std::string BaseLp::get(GRB_StringParam param) const
{
    return mModel.get(param);
}


int BaseLp::get(GRB_IntAttr attr) const
{
    return mModel.get(attr);
}


double BaseLp::get(GRB_DoubleAttr attr) const
{
    return mModel.get(attr);
}


std::string BaseLp::get(GRB_StringAttr attr) const
{
    return mModel.get(attr);
}


GRBConstr BaseLp::getConstrByName(const std::string& name)
{
    return mModel.getConstrByName(name);
}


GRBEnv& BaseLp::getGRBEnv()
{
    return mEnv;
}


GRBModel& BaseLp::getGRBModel()
{
    return mModel;
}


int BaseLp::getStatus() const
{
    return mStatus;
}


bool BaseLp::optimize()
{
    try
    {
        if (mModel.get(GRB_IntParam_OutputFlag))
        {
            RAW_LOG_F(INFO, "%s\nSolving %s LP...",
                std::string(80, '-').c_str(),
                mModel.get(GRB_StringAttr_ModelName).c_str());
        }

        mModel.optimize();
        mStatus = mModel.get(GRB_IntAttr_Status);

        if (mModel.get(GRB_IntParam_OutputFlag))
        {
            RAW_LOG_F(INFO, "%s", std::string(80, '-').c_str());
        }

        if (mStatus == GRB_INFEASIBLE ||
            mStatus == GRB_UNBOUNDED ||
            mStatus == GRB_INF_OR_UNBD)/* ||
            mModel.get(GRB_IntAttr_SolCount) == 0)*/
        {
            mModel.computeIIS();
            std::string path = "./output/" +
                mModel.get(GRB_StringAttr_ModelName) + utils::ILP_EXT;
            mModel.write(path);
            return false;
        }
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "%s_lp::optimize: error code: %d",
            mModel.get(GRB_StringAttr_ModelName).c_str(),
            e.getErrorCode());
        RAW_LOG_F(FATAL, "%s_lp::optimize: C-Exp: %s",
            mModel.get(GRB_StringAttr_ModelName).c_str(),
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "%s_lp::optimize: unknown Exception",
            mModel.get(GRB_StringAttr_ModelName).c_str());
    }

    return true;
}


bool BaseLp::optimize(const ConfigParameters::solver& params)
{
    try
    {
        // set solver parameters
        mModel.set(GRB_IntParam_Threads, params.nbThreads_);
        mModel.set(GRB_DoubleParam_TimeLimit, params.timeLimit_);
        mModel.set(GRB_StringParam_LogFile, params.logFile_);
        mModel.set(GRB_IntParam_OutputFlag, params.showLog_);

        return optimize();
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "%s_lp::optimize: error code: %d",
            mModel.get(GRB_StringAttr_ModelName).c_str(), e.getErrorCode());
        RAW_LOG_F(FATAL, "%s_lp::optimize: C-Exp: %s",
            mModel.get(GRB_StringAttr_ModelName).c_str(),
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "%s_lp::optimize: unknown Exception",
            mModel.get(GRB_StringAttr_ModelName).c_str());
    }

    return false;
}



void BaseLp::update()
{
    std::string modelName = "";
    try
    {
        modelName = mModel.get(GRB_StringAttr_ModelName);
        mModel.update();
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "%s_lp::update: error code: %d", modelName.c_str(),
            e.getErrorCode());
        RAW_LOG_F(ERROR, "%s_lp::update: C-Exp: %s", modelName.c_str(),
            e.getMessage().c_str());
        std::exit(EXIT_FAILURE);
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "%s_lp::update: unknown Exception", modelName.c_str());
    }
}


void BaseLp::write(std::string filePath)
{
    const auto fsPath = std::filesystem::path(filePath);
    // DCHECK_F(std::filesystem::is_directory(fsPath.parent_path()),
    //          "lp::write: directory %s does not exists",
    //          fsPath.parent_path().string().c_str());

    std::string modelName = "";
    try
    {
        const auto fileExt = fsPath.extension().string();
        modelName = mModel.get(GRB_StringAttr_ModelName);
        filePath = fsPath.parent_path().string() + "/" + fsPath.stem().string()
                   + modelName + fileExt;

        if (fileExt == utils::ILP_EXT)
        {
            mModel.computeIIS();
        }

        mModel.write(filePath);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "%s_lp::write: error code: %d", modelName.c_str(),
            e.getErrorCode());
        RAW_LOG_F(FATAL, "%s_lp::write: C-exp: %s", modelName.c_str(),
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "%s_lp::write: unknown Exception", modelName.c_str());
    }
}
