////////////////////////////////////////////////////////////////////////////////
/*
 * File: config_parameters.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Configuration input parameters class declaration.
 * (see ./input/example.cfg).
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on January 9, 2020, 11:58 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_PARAMETERS_HPP
#define CONFIG_PARAMETERS_HPP

#include <map>
#include <string>

class ConfigParameters
{
public:

    /**
     * @brief Column generation parameters.
    */
    struct cg
    {
        int initPoolSize_;  // # of columns in the initial pool
        int maxIter_;       // max CG iterations
        int timeLimit_;     // execution time limit
    };

    /**
     * @brief Model parameters.
    */
    struct model
    {
        int K_;
    };

    /**
     * @brief Solver parameters.
    */
    struct solver
    {
        bool showLog_;          // print output parameters
        int timeLimit_;  // time limit of the solver
        int nbThreads_; // # of threads used by solver
        std::string logFile_;
    };

    /**
     * @brief Default constructor, copy constructor, move constructor,
     * destructor, copy assingment operator and move assingment operator.
    */
    ConfigParameters() = default;
    ConfigParameters(const ConfigParameters& other) = default;
    ConfigParameters(ConfigParameters&& other) = default;
    ~ConfigParameters() = default;
    ConfigParameters& operator=(const ConfigParameters& other) = default;
    ConfigParameters& operator=(ConfigParameters&& other) = default;

    /**
     * @brief Constructor from file.
     * @param const std::string&: configuration file path with values of all
     * parameters.
    */
    ConfigParameters(const std::string config_path);

    /**
     * @brief Get the instance path.
     * @return {std::string}:.
    */
    std::string getInstancePath() const;

    /**
     * @brief Get the default output folder path.
     * @return {std::string}:.
    */
    std::string getOutputDir() const;

    const cg& getCgParams() const;

    /**
     * @brief 
     */
    const model& getModelParams() const;


    /**
     * @brief.
    */
    const solver& getSolverParams() const;

    void setSolverLogFilePath(const std::string& logFile);

    /**
     * @brief Prints instance value.
    */
    void show() const;

private:

    // Instance path (or instaces directory path)
    std::string mInstancePath;

    // Output folder path.
    std::string mOutputDir;

    // Column generation parameters
    ConfigParameters::cg mCgParam;

    // Model parameters
    ConfigParameters::model mModelParam;

    // Solver parameters
    ConfigParameters::solver mSolverParam;

    // Config_parameters values (from input file) mapping.
    std::map<std::string, std::string> mData;

    /**
     * @brief Initializes the parameters.
    */
    void setupParameters();
};

#endif // CONFIG_PARAMETERS_HPPs