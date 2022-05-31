////////////////////////////////////////////////////////////////////////////////
/*
 * File: set_covering_lp.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief CG CVRP main problem declaration.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on April 16, 2022, 02:42 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef SET_COVERING_LP_HPP
#define SET_COVERING_LP_HPP

#include "../base_lp.hpp"
#include "../utils/multi_vector.hpp"

class Column;
class Instance;

class SetCoveringLp : public BaseLp
{
public:

    SetCoveringLp() = default;
    SetCoveringLp(const SetCoveringLp& other) = default;
    SetCoveringLp(SetCoveringLp&& other) = default;
    ~SetCoveringLp() = default;

    SetCoveringLp& operator=(const SetCoveringLp& other) = default;
    SetCoveringLp& operator=(SetCoveringLp&& other) = default;

    SetCoveringLp(const std::vector<Column>& columns,
                  const std::shared_ptr<const Instance>& pInst);

    void appendColumn(const Column& columns);

    int getNbCols() const;

    double getDual(const int i) const;

private:

    std::vector<GRBVar> m_y;
    std::vector<GRBConstr> mConstrs;

    void initModel(const std::vector<Column>& columns);
};

#endif // SET_COVERING_LP_HPP
