/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Distributed under BSD 3-Clause license.                                   *
 * Copyright by the Illinois Institute of Technology.                        *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of Coeus-adapter. The full Coeus-adapter copyright      *
 * notice, including terms governing use, modification, and redistribution,  *
 * is contained in the COPYING file, which can be found at the top directory.*
 * If you do not have access to the file, you may request a copy             *
 * from scslab@iit.edu.                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <cassert>
#include "coeus/HermesEngine.h"

namespace hapi = hermes::api;

int main() {
    adios2::ADIOS adios;
    // NOTE(llogan): LD_LIBRARY_PATH must point to the directory which contains
    // the libhermes_engine.so file.

    // The file we will be creating
    std::string file = "/tmp/myFile.bp";
    // We will be performing writes
    adios2::IO io = adios.DeclareIO("CoeusTest");
    // Tell ADIOS we will be using a custom plugin
    io.SetEngine("Plugin");
    // Specify which plugin to use
    adios2::Params params;
    // The name that will be passed to the engine
    // should be the file you're trying to open
    params["PluginName"] = file;
    // The name of the shared library in the CMakeLists.txt
    params["PluginLibrary"] = "hermes_engine";
    // Any other paramaters to the engine
    io.SetParameters(params);

    // Define variables for comparison
    std::vector<double> data_u1 = {0, 0, 0, 0, 0, 0};
    std::vector<double> data_v1 = {-3, -2, 1, 0, 2, -1};

    std::vector<double> data_u2 = {1, 1, -3, 1, 3, 1};
    std::vector<double> data_v2 = {1, 2, 2, 0, -3, 0};

    std::vector<double> data_u3 = {2, 2, 1, 8, 2, 2};
    std::vector<double> data_v3 = {-2, -2, -7, -1, -2, -2};


    std::vector<double> data_get_u(6);
    std::vector<double> data_get_v(6);

    const adios2::Dims shape = {2, 3};
    const adios2::Dims start = {0, 0};
    const adios2::Dims count = {2, 3};

    adios2::Variable<double> var_u = io.DefineVariable<double>(
            "U", shape, start, count);
    adios2::Variable<double> var_v = io.DefineVariable<double>(
            "V", shape, start, count);

    adios2::Engine reader = io.Open(file, adios2::Mode::Read);
    std::cout << "-- READER ENGINE INITIALIZED --" << std::endl;

    reader.BeginStep();
    //reader.CurrentStep();
    reader.Get(var_u, data_get_u);
    reader.Get(var_v, data_get_v);
    assert(data_u1 == data_get_u);
    assert(data_v1 == data_get_v);
    reader.EndStep();

    reader.BeginStep();
    reader.Get(var_u, data_get_u);
    reader.Get(var_v, data_get_v);
    assert(data_u2 == data_get_u);
    assert(data_v2 == data_get_v);
    reader.EndStep();

    reader.BeginStep();
    reader.Get(var_u, data_get_u);
    reader.Get(var_v, data_get_v);
    assert(data_u3 == data_get_u);
    assert(data_v3 == data_get_v);
    reader.EndStep();

    reader.Close();

    std::cout << "All data arrays match!" << std::endl;
    std::cout << "Done" << std::endl;
    return 0;
}
