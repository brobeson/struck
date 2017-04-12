#include <fstream>
#include <iostream>
#include <vector>
#include "bounding_box.h"
#include "ctype.h"


namespace iou
{
    using box_list = std::vector<bounding_box<float>>;

    box_list loadResults(std::string sequence)
    {
        sequence.append(".csv");
        std::ifstream file(sequence.c_str());
        if (!file)
            throw std::runtime_error("could not open results file " + sequence);

        std::locale comma_delimiter(std::locale::classic(), new iou::ctype);
        file.imbue(comma_delimiter);

        float left, right, top, bottom;
        box_list boxes;
        while (file)
        {
            file >> left >> right >> top >> bottom;
            if (file)
                boxes.emplace_back(left, right, top, bottom);
        }
        return boxes;
    }


    void analyze(const std::string& sequence)
    {
        std::cout << "analyzing " << sequence << "...\n";
        try
        {
            auto results = loadResults(sequence);
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
