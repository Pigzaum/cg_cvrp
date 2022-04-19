////////////////////////////////////////////////////////////////////////////////
/*
 * File: init_grb_model.hpp
 *
 * @brief GRB model initialization functions declarations.
 *
 * Created on April 16, 2022, 05:56 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef INIT_MODEL_HPP
#define INIT_MODEL_HPP

#include <memory>

#include "../../include/utils/multi_vector.hpp"

class Column;
class GRBConstr;
class GRBModel;
class GRBVar;
class Instance;
class SetCoveringLp;

namespace init
{

/**
 * @brief y variables from the set covering model (restriced main problem).
 */
std::vector<GRBVar> routeVars(GRBModel& model,
                              const std::vector<Column>& routes);

/**
 * @brief y variables from the TSP subproblem.
 */
std::vector<GRBVar> visitVars(GRBModel& model,
                              const std::shared_ptr<SetCoveringLp>& pRMP,
                              const std::shared_ptr<const Instance>& pInst);

/**
 * @brief x variables from the TSP subproblem.
 */
utils::Vec2D<GRBVar> routingVars(GRBModel& model,
                                 const std::shared_ptr<const Instance>& pInst);

void coveringConstrs(GRBModel& model,
                     std::vector<GRBConstr>& constrs,
                     const std::vector<GRBVar>& y,
                     const std::vector<Column>& routes,
                     const std::shared_ptr<const Instance>& pInst);

// set covering
void kRoutesConstr(GRBModel& model,
                   std::vector<GRBConstr>& constrs,
                   const std::vector<GRBVar>& y,
                   const std::vector<Column>& routes,
                   const std::shared_ptr<const Instance>& pInst);

// TSP
void matchingConstrs(GRBModel& model,
                     const std::vector<GRBVar>& y,
                     const utils::Vec2D<GRBVar>& x,
                     const std::shared_ptr<const Instance>& pInst);

// TSP
void capacityConstr(GRBModel& model,
                    const std::vector<GRBVar>& y,
                    const std::shared_ptr<const Instance>& pInst);

} // init namespace

#endif // INIT_MODEL_HPP
