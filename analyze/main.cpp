#include <fstream>
#include <iostream>
#include <vector>
#include "bounding_box.h"
#include "ctype.h"


namespace analyze
{
    using box_list = std::vector<bounding_box<float>>;

    box_list load_results(const std::string& file_name)
    {
        std::ifstream file(file_name.c_str());
        if (!file)
            throw std::runtime_error("could not open results file " + file_name);

        std::locale comma_delimiter(std::locale::classic(), new ctype);
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

    void sanity_check(const box_list& boxes, const std::string& file_name)
    {
        std::ofstream file(file_name.c_str());
        if (file)
        {
            for (const auto& box: boxes)
                file << box.left() << "," << box.right() << "," << box.top() << "," << box.bottom() << "\n";
        }
    }

    void analyze(const std::string& sequence)
    {
        std::cout << "analyzing " << sequence << "...\n";
        try
        {
            // load the struck results for the sequence
            auto results = load_results(sequence + ".csv");

            // load the ground truth for the sequence
            std::string ground_truth_path("/home/brendan/Videos/struck_data/");
            ground_truth_path.append(sequence)
                             .append("/")
                             .append(sequence)
                             .append("_gt.txt");
            auto ground_truth = load_results(ground_truth_path);

            //sanity_check(results, "results.txt");
            //sanity_check(ground_truth, "ground_truth.txt");

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
        analyze::analyze(sequence);

    return 0;
}
