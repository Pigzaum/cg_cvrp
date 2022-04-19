#include "../../../include/ext/lkh-2.0.9/lkh_tsp.hpp" // modified by Guilherme
#include "../../../include/ext/lkh-2.0.9/LKH.h"
#include "../../../include/ext/lkh-2.0.9/Genetic.h"

#include <string>

int gNbCalls = 0;

/*
 * This file contains the main function of the program.
 */

int LKH::execute(const int* distMtx, const int size) // modified by Guilherme
// int main(int argc, char *argv[])
{
    GainType Cost, OldOptimum;
    double Time, LastTime;

    ReadParameters();

    StartTime = LastTime = GetTime();
    MaxMatrixDimension = 20000;
    MergeWithTour = Recombination == IPT ? MergeWithTourIPT :
        MergeWithTourGPX2;
    ReadProblem(distMtx, size);

    AllocateStructures();
    CreateCandidateSet();
    InitializeStatistics();

    if (Norm != 0)
    {
        BestCost = PLUS_INFINITY;
    }
    else {
        /* The ascent has solved the problem! */
        Optimum = BestCost = (GainType) LowerBound;
        UpdateStatistics(Optimum, GetTime() - LastTime);
        RecordBetterTour();
        RecordBestTour();
        // WriteTour(OutputTourFileName, BestTour, BestCost);
        // WriteTour(TourFileName, BestTour, BestCost);
        Runs = 0;
    }


    /* Find a specified number (Runs) of local optima */
    for (Run = 1; Run <= Runs; Run++) {
        LastTime = GetTime();
        if (LastTime - StartTime >= TimeLimit) {
            // if (TraceLevel >= 1)
            //     printff("*** Time limit exceeded ***\n");
            break;
        }
        Cost = FindTour();      /* using the Lin-Kernighan heuristic */
        
        // DRAW_LOG_F(INFO, "here");

        if (MaxPopulationSize > 1) {
            /* Genetic algorithm */
            int i;
            for (i = 0; i < PopulationSize; i++) {
                // GainType OldCost = Cost;
                Cost = MergeTourWithIndividual(i);
                // if (TraceLevel >= 1 && Cost < OldCost) {
                //     printff("  Merged with %d: Cost = " GainFormat, i + 1,
                //             Cost);
                //     if (Optimum != MINUS_INFINITY && Optimum != 0)
                //         printff(", Gap = %0.4f%%",
                //                 100.0 * (Cost - Optimum) / Optimum);
                //     printff("\n");
                // }
            }

            if (!HasFitness(Cost)) {
                if (PopulationSize < MaxPopulationSize) {
                    AddToPopulation(Cost);
                    // if (TraceLevel >= 1)
                    //     PrintPopulation();
                } else if (Cost < Fitness[PopulationSize - 1]) {
                    i = ReplacementIndividual(Cost);
                    ReplaceIndividualWithTour(i, Cost);
                    // if (TraceLevel >= 1)
                    //     PrintPopulation();
                }
            }
        } else if (Run > 1)
        {
            Cost = MergeTourWithBestTour();
        }

        if (Cost < BestCost) {
            BestCost = Cost;
            RecordBetterTour();
            RecordBestTour();
            // WriteTour(OutputTourFileName, BestTour, BestCost);
            // WriteTour(TourFileName, BestTour, BestCost);
        }
        OldOptimum = Optimum;
        if (Cost < Optimum) {
            if (FirstNode->InputSuc) {
                Node *N = FirstNode;
                while ((N = N->InputSuc = N->Suc) != FirstNode);
            }
            Optimum = Cost;
            // printff("*** New optimum = " GainFormat " ***\n", Optimum);
        }
        Time = fabs(GetTime() - LastTime);
        UpdateStatistics(Cost, Time);
        // if (TraceLevel >= 1 && Cost != PLUS_INFINITY) {
        //     printff("Run %d: Cost = " GainFormat, Run, Cost);
        //     if (Optimum != MINUS_INFINITY && Optimum != 0)
        //         printff(", Gap = %0.4f%%",
        //                 100.0 * (Cost - Optimum) / Optimum);
        //     printff(", Time = %0.2f sec. %s\n\n", Time,
        //             Cost < Optimum ? "<" : Cost == Optimum ? "=" : "");
        // }
        if (StopAtOptimum && Cost == OldOptimum && MaxPopulationSize >= 1) {
            Runs = Run;
            break;
        }
        if (PopulationSize >= 2 &&
            (PopulationSize == MaxPopulationSize ||
             Run >= 2 * MaxPopulationSize) && Run < Runs) {
            Node *N;
            int Parent1, Parent2;
            Parent1 = LinearSelection(PopulationSize, 1.25);
            do
                Parent2 = LinearSelection(PopulationSize, 1.25);
            while (Parent2 == Parent1);
            ApplyCrossover(Parent1, Parent2);
            N = FirstNode;
            do {
                if (ProblemType != HCP && ProblemType != HPP) {
                    int d = C(N, N->Suc);
                    AddCandidate(N, N->Suc, d, INT_MAX);
                    AddCandidate(N->Suc, N, d, INT_MAX);
                }
                N = N->InitialSuc = N->Suc;
            }
            while (N != FirstNode);
        }
        SRandom(++Seed);
    }
    int costmin = PrintStatistics();

    ++gNbCalls;
    FreeStructures();
    return costmin;
}
