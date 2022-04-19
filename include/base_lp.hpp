////////////////////////////////////////////////////////////////////////////////
/*
 * File: base_lp.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Base class definition for linear programming models using GUROBI's
 * API.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on July 20, 2021, 09:07 AM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef BASE_LP_HPP
#define BASE_LP_HPP

#include <limits>
#include <memory>
#include <gurobi_c++.h>

#include "config_parameters.hpp"

class Instance;

class BaseLp
{
public:

    /**
     * @brief Default copy constructor, move constructor,
     * destructor, copy assingment operator and move assingment operator.
    */
    BaseLp(const BaseLp& other) = default;
    BaseLp(BaseLp&& other) = default;
    virtual ~BaseLp() = default;
    BaseLp& operator=(const BaseLp& other) = default;
    BaseLp& operator=(BaseLp&& other) = default;
    
    /**
     * @brief Deleted default constructor.
    */
    BaseLp() = delete;

    /**
     * @brief.
     * @param:.
    */
    BaseLp(const std::shared_ptr<const Instance>& pInst);

    BaseLp(GRBEnv& env, const std::shared_ptr<const Instance>& pInst);

    int get(GRB_IntParam param) const;
    double get(GRB_DoubleParam param) const;
    std::string get(GRB_StringParam param) const;
    int get(GRB_IntAttr attr) const;
    double get(GRB_DoubleAttr attr) const;
    std::string get(GRB_StringAttr attr) const;

    GRBConstr getConstrByName(const std::string& name);

    GRBEnv& getGRBEnv();
    GRBModel& getGRBModel();

    int getStatus() const;

    template <class E, class T>
    void set(const E grbEnum, const T val)
    {
        mModel.set(grbEnum, val);
    }

    bool optimize();

    /**
     * @brief.
     * @param:.
     * @return:.
    */
    bool optimize(const ConfigParameters::solver& solverParams);

    void update();

    /**
     * @brief.
     * @param:.
    */
    void write(std::string filePath);

    void writeResultsCSV(std::string filePath) const;

protected:

    // GRB model status from the lastest optimization
    int mStatus;
    // Pointer to instance
    std::shared_ptr<const Instance> mpInst;
    // GUROBI's environment
    GRBEnv mEnv;
    // GUROBI's model
    GRBModel mModel;
};

#endif // BASE_LP_HPP

