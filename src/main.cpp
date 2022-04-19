////////////////////////////////////////////////////////////////////////////////
/*
 * File: main.cpp
 *
 * @brief Symmetric Capacitated Vehicle-Routing Problem (CVRP) linear program.
 * @author Guilherme O. Chagas.
 * @date This file was created on October 22, 2020, 05:52 PM
 * @warning I'm sorry for my bad English xD.
 * @acknowledgment Special thanks to Ph.D. Leandro C. Coelho and Ph.D. Cleder
 * Marcos Schenekemberg.
 * @copyright GNU General Public License.
 *
 * References:
 * [1] P. Toth and D. Vigo. The Vehicle Routing Problem, Discrete Mathematics
 * and Applications, SIAM, 2002.
 */
////////////////////////////////////////////////////////////////////////////////

#include <filesystem>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/config_parameters.hpp"
#include "../include/instance.hpp"
#include "../include/column_generation/cg.hpp"
#include "../include/column_generation/column.hpp"
#include "../include/column_generation/init_pool.hpp"
#include "../include/utils/helper.hpp"


void buildNsolve(const std::string& path,
                 const ConfigParameters& params)
{
    RAW_LOG_F(INFO, "executing instance: %s", path.c_str());

    auto pInst = std::make_shared<Instance>(path, params.getModelParams().K_);
    pInst->show();

    auto columns = initPool::randomCovers(50, pInst);

    Cg cg(params.getCgParams(), columns, pInst);
    cg.execute(params.getSolverParams());
}


int main(int argc, char **argv)
{
    ConfigParameters params(utils::helper::parseCmdLine(argc, argv));
    loguru::init(argc, argv);

    std::filesystem::create_directories(params.getOutputDir());

    /* Put every log message in the log file */
    {
        using namespace loguru;
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << params.getOutputDir() << "cg_cvrp_execution_" <<
            std::put_time(&tm, "%d_%m_%Y_%H_%M_%S") << ".log";
        loguru::add_file(oss.str().c_str(), Append, Verbosity_MAX);
        params.setSolverLogFilePath(oss.str());
    }

    params.show();

    const std::string path = params.getInstancePath();
    if (std::filesystem::is_directory(path))
    {
        /* execute in batch */
        for (const auto &f : std::filesystem::directory_iterator(path))
        {
            buildNsolve(f.path(), params);
            RAW_LOG_F(INFO, std::string(80, '=').c_str());
        }
    }
    else
    {
        /* single instance execution */
        buildNsolve(path, params);
    }

    return EXIT_SUCCESS;
}
