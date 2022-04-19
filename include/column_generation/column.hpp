////////////////////////////////////////////////////////////////////////////////
/*
 * File: column.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Column class declaration.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on April 16, 2022, 04:45 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef COLUMN_HPP
#define COLUMN_HPP

#include <memory>
#include <vector>

class Instance;

class Column
{
public:

    Column() = default;
    Column(const Column& other) = default;
    Column(Column&& other) = default;
    ~Column() = default;

    Column& operator=(const Column& other) = default;
    Column& operator=(Column&& other) = default;

    Column(const std::shared_ptr<const Instance>& pInst);

    std::vector<int>::const_iterator begin() const;
    std::vector<int>::const_iterator end() const;

    bool contains(const int i) const;
    int getNbVertices() const;
    double getCost() const;
    double getDemand() const;

    bool addVertex(const int i);

    void setCost(const double cost);

private:

    double mCost;
    double mDemand;
    std::vector<bool> mContained;
    std::vector<int> mRoute; // TODO
    std::shared_ptr<const Instance> mpInst;
};

#endif // COLUMN_HPP
