#ifndef NOISEWALKER_TOOLS_H
#define NOISEWALKER_TOOLS_H

#include <iostream>
#include <fstream>
#include <vector>
#include "agent.h"
#include <cmath>
#include <algorithm> 
#include <cassert>


/// function to get standard deviation of mass
std::vector<double> getSummaryMass (std::vector<Agent> &pop) {
    std::vector<double> popMass (pop.size());
    for(size_t p_i = 0; p_i  < pop.size(); ++p_i) {
        popMass[p_i] = pop[p_i].mass;
    }
    double popMassMean = std::accumulate(popMass.begin(), popMass.end(), 0.0);
    popMassMean = popMassMean / static_cast<double>(popMass.size());
    
    double sqSum = std::inner_product(popMass.begin(), popMass.end(), 
        popMass.begin(), 0.0);
    double stdev = std::sqrt(sqSum / popMass.size() - popMassMean * popMassMean);

    return (std::vector<double> {popMassMean, stdev});
}

/// construct agent output filename
std::vector<std::string> identifyOutpath(const int octaves,
                                         const float frequency,
                                         const float frequencyTransfer,
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
        summary_ofs << "filename,octaves,frequency,fTransfer,rep\n";
        summary_ofs.close();
    }
    // append if not
    summary_ofs.open(summary_out, std::ofstream::out | std::ofstream::app);
    summary_ofs << output_id << ","
                << octaves << ","
                << frequency << ","
                << frequencyTransfer << ","
                << rep << "\n";
    summary_ofs.close();

    return std::vector<std::string> {path, output_id};
}

/// function for agent reaction norms
void printReacNorm (std::vector<Agent> &pop,
                    std::vector<std::string> outpath) {
    // ofstream
    std::ofstream rnormOfs;
    // std::cout << "data path = " << outpath[0] + outpath[1] << "\n";

    // check if okay
    std::ifstream f(outpath[0] + outpath[1] + ".csv");
    /*if (!f.good()) {
        std::cout << "data path " << outpath[0] + outpath[1] << " good to write\n";
    }*/
    // write column names
    rnormOfs.open(outpath[0] + outpath[1] + ".csv",
            std::ofstream::out | std::ofstream::app);

    rnormOfs << "id,cue,distance,angle\n";

    // run through individuals and cues
    for (size_t indiv = 0; indiv < pop.size(); ++indiv) {
        for (float cue_i = -0.5f; cue_i <= 0.5f; cue_i+= 0.05f) {
            std::array<float, 1> testOutput = pop[indiv].annOutput(cue_i, cue_i);

            rnormOfs << indiv << ","
                     << cue_i << ","
                     << testOutput[0] << ","
                     << testOutput[1] << "\n";
        }
    }
    rnormOfs.close();
}

/// function to print evolved agent mass
void printPopMass (std::vector<Agent> &pop,
                    const int gen,
                    std::vector<std::string> outpath) {
    // ofstream
    std::ofstream rnormOfs;
    // std::cout << "data path = " << outpath[0] + outpath[1] << "\n";

    // check if okay
    std::ifstream f(outpath[0] + outpath[1] + "_mass.csv");
    // if (!f.good()) {
    //     std::cout << "data path " << outpath[0] + outpath[1] << " good to write\n";
    // }
    // write column names
    rnormOfs.open(outpath[0] + outpath[1] + "_mass.csv",
            std::ofstream::out | std::ofstream::app);

    if (gen == 0) {
        rnormOfs << "gen, mass\n";
    }

    // print gen
    rnormOfs << gen << ",";

    // run through individuals and cues
    for (size_t indiv = 0; indiv < pop.size(); ++indiv) {
            rnormOfs << pop[indiv].mass << ";";
    }
    rnormOfs << "\n";
    rnormOfs.close();
}

/// function to tabulate mass classes
std::vector<std::pair<float, int> > getMassTable (std::vector<Agent> &pop,
                                const float massRound)
{
    // collect masses
    std::vector<float> popMass (pop.size());

    // round to the nearest multiple of massRound
    for (size_t p_i = 0; p_i < pop.size(); ++p_i)
    {
        popMass[p_i] = pop[p_i].mass;
    }
    // round to the nearest multiple of massRound
    for (size_t p_i = 0; p_i < popMass.size(); ++p_i)
    {
        popMass[p_i] = roundf(popMass[p_i] / massRound) * massRound;
    }
    
    // make a copy
    std::vector<float> popMassUnique = popMass;
    std::sort(popMassUnique.begin(), popMassUnique.end());
    popMassUnique.erase(unique(popMassUnique.begin(), popMassUnique.end()), 
                                popMassUnique.end());

    // count unique masses in range
    std::vector<std::pair<float, int> > massTable (popMassUnique.size());

    for (size_t it = 0; it < popMassUnique.size(); ++it)
    {
        massTable[it].first = popMassUnique[it];
        const float thisVal = popMassUnique[it];
        massTable[it].second = static_cast<int>(std::count_if(popMass.begin(), popMass.end(), [thisVal](float f){
            return fabs(f - thisVal) < 0.00001f; 
        }) );
    }

    assert(massTable.size() > 0);
    return massTable;

}

/// function to print mass summary stats
void printSummaryMass (std::vector<Agent> &pop,
                    const int gen,
                    const float massRound,
                    std::vector<std::string> outpath) {
    // ofstream
    std::ofstream rnormOfs;
    // std::cout << "data path = " << outpath[0] + outpath[1] << "\n";

    // check if okay
    std::ifstream f(outpath[0] + outpath[1] + "_summary.csv");
    // if (!f.good()) {
    //     std::cout << "data path " << outpath[0] + outpath[1] << " good to write\n";
    // }
    // write column names
    rnormOfs.open(outpath[0] + outpath[1] + "_summary.csv",
            std::ofstream::out | std::ofstream::app);

    if (gen == 0) {
        rnormOfs << "gen,massRound,count\n";
    }
    
    // get population mass summary stats
    std::vector<std::pair<float, int> > popSummaryMass = getMassTable(pop, massRound);

    assert(popSummaryMass.size() > 0);

    // print mass counts
    for (size_t p_i = 0; p_i < popSummaryMass.size(); ++p_i)
    {
        rnormOfs << gen << "," 
                 << popSummaryMass[p_i].first << ","
                 << popSummaryMass[p_i].second << "\n";
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
    // if (!f.good() && gen == 0) {
    //     std::cout << "data path good to write\n";
    // }
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
