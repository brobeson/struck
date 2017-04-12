#include <fstream>
#include <iostream>
#include <vector>
#include "bounding_box.h"
#include "ctype.h"
#include "iou.h"


namespace analyze
{
    using box_list = std::vector<bounding_box<float>>;

    using iou_list = std::vector<iou<float>>;

    box_list load_results(const std::string& file_name)
    {
        std::ifstream file(file_name.c_str());
        if (!file)
            throw std::runtime_error("could not open results file " + file_name);

        std::locale comma_delimiter(std::locale::classic(), new ctype);
        file.imbue(comma_delimiter);

        float left, width, top, height;
        box_list boxes;
        while (file)
        {
            file >> left >> width >> top >> height;
            if (file)
                boxes.emplace_back(left, left + width, top, top + height);
        }
        return boxes;
    }

    iou_list calculate_ious(const box_list& results, const box_list& ground_truth)
    {
        iou_list ious;

        constexpr box_list::size_type stride = 5;
        const auto length = std::min(results.size(), ground_truth.size());
        for (box_list::size_type b = 0; b < length; b += stride)
            ious.emplace_back(results[b], ground_truth[b]);
        return ious;
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

    void validate_box_lists(const box_list& results, const box_list& ground_truth)
    {
        if (results.size() != ground_truth.size())
        {
            std::cerr << "warning: There are " << results.size() << " results boxes, and " << ground_truth.size() << " ground truth boxes.\n"
                      << "         Only the first " << std::min(results.size(), ground_truth.size()) << " boxes will be considered.\n";
        }
    }

    void write_ious(const iou_list& ious, const std::string& file_name)
    {
        std::ofstream file(file_name.c_str());
        if (!file)
        {
            std::cerr << "error: could not open " << file_name << " for writing IoU data.\n";
            return;
        }

        for (const auto i : ious)
            file << i.value() << std::endl;
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

            validate_box_lists(results, ground_truth);
            const auto ious = calculate_ious(results, ground_truth);
            write_ious(ious, sequence + ".ious");
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
