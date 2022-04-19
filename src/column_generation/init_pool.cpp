////////////////////////////////////////////////////////////////////////////////
/*
 * File: init_pool.cpp
 *
 * @brief Functions to generate an initial pool of columns.
 *
 * Created on April 17, 2022, 02:40 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <iostream>
#include <queue>
#include <numeric>
#include <random>

#include "../../include/ext/loguru/loguru.hpp"

#include "../../include/column_generation/init_pool.hpp"
#include "../../include/instance.hpp"
#include "../../include/column_generation/column.hpp"
#include "../../include/utils/multi_vector.hpp"
#include "../../include/ext/lkh-2.0.9/lkh_tsp.hpp"

/* ---------------------------- helper functions  --------------------------- */

namespace
{

std::random_device gRd;  // will be used to obtain a seed for the rand nb eng
std::mt19937 gGen(gRd()); // standard mersenne_twister_engine seeded with rd()


template<typename T>
class Vec2dArray
{
public:

    Vec2dArray() = delete;

    Vec2dArray(const Vec2dArray& other) = default;
    Vec2dArray(Vec2dArray&& other) = default;
    ~Vec2dArray() = default;

    Vec2dArray& operator=(const Vec2dArray& other) = default;
    Vec2dArray& operator=(Vec2dArray&& other) = default;

    Vec2dArray(const int dim1, const int dim2) :
        mDim1(dim1),
        mDim2(dim2),
        mData(dim1 * dim2)
    {
    }

    T& get(const int i, const int j)
    {
        DCHECK_F(i >= 0 && i < mDim1);
        DCHECK_F(j >= 0 && j < mDim2);
        DCHECK_F(i * mDim1 + j < static_cast<int>(mData.size()));
        return mData[i * mDim1 + j];
    }

    const T& get(const int i, const int j) const
    {
        DCHECK_F(i >= 0 && i < mDim1);
        DCHECK_F(j >= 0 && j < mDim2);
        DCHECK_F(i * mDim1 + j < static_cast<int>(mData.size()));
        return mData[i * mDim1 + j];
    }

    T* getData()
    {
        return mData.data();
    }

    int getSize() const
    {
        return mData.size();
    }

private:

    int mDim1;
    int mDim2;
    std::vector<T> mData;
};


Vec2dArray<int> genDistMtx(const std::vector<int>::const_iterator& itb,
                           const std::vector<int>::const_iterator& ite,
                           const std::shared_ptr<const Instance>& pInst)
{
    int size = std::distance(itb, ite);
    DCHECK_F(size > 0);

    Vec2dArray<int> distMtx(size, size);

    auto iti = itb;
    for (int i = 0; iti != ite; ++i, ++iti)
    {
        auto itj = std::next(iti);
        for (int j = i + 1; itj != ite; ++j, ++itj)
        {
            distMtx.get(i, j) = pInst->getcij(*iti, *itj);
            distMtx.get(j, i) = pInst->getcij(*iti, *itj);
        }
    }

    return distMtx;
}

} // anonymous namespace

/* -------------------------------------------------------------------------- */


std::vector<Column> initPool::randomCovers(
    const int n,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "Generating %d initial columns...", n);

    std::vector<Column> columns;
    columns.reserve(n * pInst->getK());

    std::vector<int> vertices(pInst->getNbVertices());
    std::iota(std::begin(vertices), std::end(vertices), 0);

    for (int i = 0; i < n; ++i)
    {
        std::shuffle(std::begin(vertices), std::end(vertices), gGen);

        for (int j = 0; j < pInst->getNbVertices(); ++j)
        {
            Column column(pInst);
            column.addVertex(0);

            while (j < pInst->getNbVertices() &&
                   pInst->getdi(vertices[j]) + column.getDemand() <=
                   pInst->getC())
            {
                if (vertices[j] != 0)
                {
                    column.addVertex(vertices[j]);
                }
                ++j;
            }

            int cost = 0;
            if (column.getNbVertices() > 2)
            {
                // call LKH algorithm to compute the TSP cost
                cost = LKH::execute(genDistMtx(
                    std::begin(column), std::end(column), pInst).getData(),
                    column.getNbVertices());
            }
            else if (column.getNbVertices() == 2) // round trip
            {
                cost = 2 * pInst->getcij(0, *std::prev(std::end(column)));
            }

            column.setCost(cost);
            columns.push_back(column);
        }
    }

    return columns;
}
