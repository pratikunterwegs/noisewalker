#ifndef NOISEWALKER_TOOLS_H
#define NOISEWALKER_TOOLS_H

#include <iostream>
#include <fstream>
#include <vector>
#include "agent.hpp"

/// construct agent output filename
std::vector<std::string> identifyOutpath(const float frequency,
                                         const int tempRate,
                                         const std::string rep){
    // assumes path/type already prepared
    std::string path = "data/";
    // output filename as milliseconds since epoch
    std::string output_id;
    {
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto value = now_ms.time_since_epoch();
        // add a random number to be sure of discrete values
        long duration = value.count() +
                static_cast<long>(gsl_rng_uniform_int(r, 10000));

        output_id = std::to_string(duration);
    }

    // write summary with filename to agent data
    // and parameter files
    // start with output id
    const std::string summary_out = path + "/lookup.csv";
    std::ofstream summary_ofs;

    // if not exists write col names
    std::ifstream f2(summary_out.c_str());
    if (!f2.good()) {
        summary_ofs.open(summary_out, std::ofstream::out);
        summary_ofs << "filename,growthRate,sRange,rep\n";
        summary_ofs.close();
    }
    // append if not
    summary_ofs.open(summary_out, std::ofstream::out | std::ofstream::app);
    summary_ofs << output_id << ","
                << frequency << ","
                << tempRate << ","
                << rep << "\n";
    summary_ofs.close();

    return std::vector<std::string> {path, output_id};
}

/// function for agent reaction norms
void printReacNorm (std::vector<Agent> &pop,
                    std::vector<std::string> outpath) {
    // ofstream
    std::ofstream rnormOfs;
    std::cout << "data path = " << outpath[0] + outpath[1] << "\n";

    // check if okay
    std::ifstream f(outpath[0] + outpath[1] + ".csv");
    if (!f.good()) {
        std::cout << "data path " << outpath[0] + outpath[1] << " good to write\n";
    }
    // write column names
    rnormOfs.open(outpath[0] + outpath[1] + ".csv",
            std::ofstream::out | std::ofstream::app);

    rnormOfs << "id,cue,distance,angle\n";

    // run through individuals and cues
    for (size_t indiv = 0; indiv < pop.size(); ++indiv) {
        for (float cue_i = -0.5f; cue_i <= 0.5f; cue_i+= 0.05f) {
            std::array<float, 2> testOutput = pop[indiv].annOutput(cue_i, cue_i, cue_i, cue_i);

            rnormOfs << indiv << ","
                     << cue_i << ","
                     << testOutput[0] << ","
                     << testOutput[1] << "\n";
        }
    }
    rnormOfs.close();
}

/// function to print agent weights
void printAgentWeights(std::vector<Agent> &pop,
                       std::vector<std::string> outpath,
                       int gen) {
    // get output stream
    std::ofstream annOfs;

    // flexibly get the number of weights
    const int nWeights = static_cast<int> (pop[0].getAnnWeights().size());

    // get population weights
    const std::vector<std::vector<float> > popWeights = getPopAnnWts(pop);

    // check if okay
    std::ifstream f(outpath[0] + outpath[1] + ".csv");
    if (!f.good() && gen == 0) {
        std::cout << "data path good to write\n";
    }
    if (gen == 0) {
        annOfs.open(outpath[0]+ outpath[1] + ".csv",
                std::ofstream::out);
        annOfs << "gen,id,";
        // print wt_number in a loop
        for (size_t wlen = 0; wlen < static_cast<size_t>(nWeights); ++wlen) {
            // print commas or newline as appropriate
            if (wlen < static_cast<size_t> (nWeights) - 1) {
                annOfs << "wt_" + std::to_string(wlen) << ",";
            } else {
                annOfs << "wt_" + std::to_string(wlen) << "\n";
            }
        }
        annOfs.close();
    }
    // open ofs
    annOfs.open(outpath[0] + outpath[1] + ".csv",
            std::ofstream::out | std::ofstream::app);
    // loop through agents and print weights
    for (size_t i_print = 0; i_print < pop.size(); ++i_print) {

        annOfs << gen << ","
               << i_print << ",";

        // print weights
        for (size_t wlen = 0; wlen < static_cast<size_t>(nWeights); ++wlen) {
            if (wlen < static_cast<size_t> (nWeights) - 1) {
                annOfs << popWeights[i_print][wlen] << ",";
            } else {
                annOfs << popWeights[i_print][wlen] << "\n";
            }
        }
    }

    annOfs.close();
}


#endif // NOISEWALKER_TOOLS_H
