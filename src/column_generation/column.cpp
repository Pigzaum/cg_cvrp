////////////////////////////////////////////////////////////////////////////////
/*
 * File: column.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Column class definition.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on April 16, 2022, 04:59 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ext/loguru/loguru.hpp"

#include "../../include/column_generation/column.hpp"
#include "../../include/instance.hpp"


Column::Column(const std::shared_ptr<const Instance>& pInst) :
    mCost(0),
    mDemand(0),
    mContained(pInst->getNbVertices(), false),
    mpInst(pInst)
{
    mRoute.reserve(pInst->getNbVertices());
}


std::vector<int>::const_iterator Column::begin() const
{
    return std::cbegin(mRoute);
}


std::vector<int>::const_iterator Column::end() const
{
    return std::cend(mRoute);
}


bool Column::contains(const int i) const
{
    DCHECK_F(i >= 0 && i < static_cast<int>(mContained.size()));
    return mContained[i];
}


int Column::getNbVertices() const
{
    return mRoute.size();
}


double Column::getCost() const
{
    return mCost;
}


double Column::getDemand() const
{
    return mDemand;
}


bool Column::addVertex(const int i)
{
    DCHECK_F(i < static_cast<int>(mContained.size()));

    if (contains(i))
    {
        return false;
    }

    mDemand += mpInst->getdi(i);

    mRoute.push_back(i);

    return mContained[i] = true;
}


void Column::setCost(const double cost)
{
    mCost = cost;
}
