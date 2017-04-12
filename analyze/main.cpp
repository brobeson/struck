#include <fstream>
#include <iostream>
#include <vector>
#include "ctype.h"


namespace iou
{
    void loadResults(std::string sequence)
    {
        sequence.append(".csv");
        std::ifstream file(sequence.c_str());
        if (!file)
            throw std::runtime_error("could not open results file " + sequence);

        std::locale comma_delimiter(std::locale::classic(), new iou::ctype);
        file.imbue(comma_delimiter);

        float x = 0.0f;
        for (int i = 0; i < 10; ++i)
        {
            file >> x;
            std::cout << "x_" << i << " == " << x << std::endl;
        }
    }


    void analyze(const std::string& sequence)
    {
        std::cout << "analyzing " << sequence << "...\n";
        try
        {
            loadResults(sequence);
        }
        catch (std::exception& e)
        {
            std::cerr << "error in " << __func__ << ": " << e.what() << std::endl;
        }
    }
}


int main(int argc, char** argv)
{
    const std::vector<std::string> sequences =
    {
        "coke11",
        //"david",
        //"faceocc",
        //"faceocc2",
        //"girl",
        //"sylv",
        //"tiger1",
        //"tiger2"
    };

    for (const auto& sequence: sequences)
        iou::analyze(sequence);

    return 0;
}
