////////////////////////////////////////////////////////////////////////////////
/*
 * File: pctsp_ilp.hpp
 *
 * @brief Prize collecting Travelling salesman problem (PCTSP)
 * Bixby-Coullard-Simchi-Levi CG subproblem [1] class declaration. This model is
 * used as the subproblem of the column generation algorithm as described by
 * [1, 2]. Note, however, that we only use the subtour elimination constraints
 * (SEC) rather than others cuts as [1].
 *
 * Created on April 18, 2022, 11:12 PM
 * 
 * References:
 * [1] A. Bixby, C. Coullard, and D. Simchi-Levi. The capacitated
 * prize-collecting traveling salesman problem. Working paper, Department of
 * Industrial Engineering and Engineering Management, Northwestern University,
 * Evanston, IL, 1997.
 * [2] P. Toth and D. Vigo. The Vehicle Routing Problem, Discrete Mathematics
 * and Applications, SIAM, 2002.
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef PCTSP_ILP_HPP
#define PCTSP_ILP_HPP

#include "../base_lp.hpp"
#include "../utils/multi_vector.hpp"

class CallbackSEC;
class Column;
class Instance;
class SetCoveringLp;

class PctspIlp : public BaseLp
{
public:

    PctspIlp() = default;
    PctspIlp(const PctspIlp& other) = default;
    PctspIlp(PctspIlp&& other) = default;
    ~PctspIlp() = default;
    PctspIlp& operator=(const PctspIlp& other) = default;
    PctspIlp& operator=(PctspIlp&& other) = default;

    PctspIlp(const std::shared_ptr<SetCoveringLp>& pRMP,
               const std::shared_ptr<const Instance>& pInst);

    std::pair<Column, double> extractColumn() const;

    void updateVisitVarsObjCoeff();

private:

    // visit variables
    std::vector<GRBVar> m_y;
    // routing variables
    utils::Vec2D<GRBVar> m_x;

    std::shared_ptr<SetCoveringLp> mpRMP;
    std::shared_ptr<CallbackSEC> mpCb;

    void initModel();
};

#endif // PCTSP_ILP_HPP
