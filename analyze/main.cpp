#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>
#include "bounding_box.h"
#include "ctype.h"
#include "iou.h"


namespace analyze
{
    /// Alias the type representing a list of bounding boxes.
    using box_list = std::vector<bounding_box<float>>;

    /// Alias the type representing a list of IoU objects.
    using iou_list = std::vector<iou>;

    /**
     * \brief       Read bounding box data from a file.
     * \param[in]   file_name   The path to the file containing the bounding box data.
     * \return      A list of bounding box data from the file.
     * \throws      std::runtime_error  This is thrown if the file cannot be opened.
     * \details     Bounding box data in the file must adhere to these restrictions:
     *              \li The file must be plain text.
     *              \li Each line must correspond to one frame of imagery or video.
     *              \li Each line must contain four values, in this order: bounding box left edge,
     *              bounding box width, bounding box top edge, bounding box height, all measured in
     *              pixels.
     *              \li Fractional pixels are allowed, but not required.
     *              \li Each value in a line must be separated by a comma.
     *              \li Nothing else may be on the line.
     *              Results are undefined if the file violates any of these restrictions.
     */
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

    /**
     * \brief       Calculate IoU values for two lists of bounding boxes.
     * \param[in]   results         The list of bounding boxes representing algorithm results.
     * \param[in]   ground_truth    The list of bounding boxes representing ground truth.
     * \return      A list of intersection-over-union (IoU) values.
     * \throws      None
     * \details     Each entry in the IoU list is the IoU for the corresponding entries in the
     *              \a results and \a ground_truth lists. The IoU formula is:
     *              \f$ IoU(B,G) = \frac{B \cap G}{B \cup G} \f$
     */
    iou_list calculate_ious(const box_list& results, const box_list& ground_truth) noexcept
    {
        iou_list ious;

        constexpr box_list::size_type stride = 5;
        const auto length = std::min(results.size(), ground_truth.size());
        for (box_list::size_type b = 0; b < length; b += stride)
            ious.emplace_back(results[b], ground_truth[b]);
        return ious;
    }

    /**
     * \brief       Write a list of bounding boxes to a file.
     * \param[in]   boxes       The list of boxes to write.
     * \param[in]   file_name   The path to the file to write.
     * \throws      None.
     * \details     The purpose of this function is that a user can compare the written file to the
     *              file from which the boxes were read.
     * \warning     This will overwrite \a file_name without asking.
     * \todo        Move this to a unit test.
     */
    void sanity_check(const box_list& boxes, const std::string& file_name) noexcept
    {
        std::ofstream file(file_name.c_str());
        if (file)
        {
            for (const auto& box: boxes)
                file << box.left() << "," << box.right() << "," << box.top() << "," << box.bottom() << "\n";
        }
    }

    /**
     * \brief       Determine if there are an equal number of results as ground truth.
     * \param[in]   results         The list of algorithm results bounding boxes.
     * \param[in]   ground_truth    The list of ground truth bounding boxes.
     * \throws      None
     */
    void validate_box_lists(const box_list& results, const box_list& ground_truth) noexcept
    {
        if (results.size() != ground_truth.size())
        {
            std::cerr << "warning: There are " << results.size() << " results boxes, and " << ground_truth.size() << " ground truth boxes.\n"
                      << "         Only the first " << std::min(results.size(), ground_truth.size()) << " boxes will be considered.\n";
        }
    }

    /**
     * \brief       Write a list of IoU values to a file.
     * \param[in]   ious        The list of IoU values to write.
     * \param[in]   file_name   The path to the file to write.
     * \throws      None
     * \details     This writes one IoU for each line.
     */
    void write_ious(const iou_list& ious, const std::string& file_name) noexcept
    {
        std::ofstream file(file_name.c_str());
        if (!file)
        {
            std::cerr << "error: could not open " << file_name << " for writing IoU data.\n";
            return;
        }

        for (const auto i : ious)
            file << i.value() << std::endl;

        // write the minimum, average, and maximum
        file << "minimum: "   << std::min_element(ious.cbegin(), ious.cend())->value()
             << "\nmaximum: " << std::max_element(ious.cbegin(), ious.cend())->value()
             << "\naverage: " << std::accumulate(ious.cbegin(), ious.cend(), iou(0.0f)).value() / ious.size();
    }

    /**
     * \brief       Analyze the tracking results for a video or image sequence.
     * \param[in]   sequence
     * \throws      None
     * \details     This will load the bounding box results and ground truth, then calculate and
     *              output IoU data.
     */
    void analyze(const std::string& sequence) noexcept
    {
        std::cout << "analyzing " << sequence << "...\n";
        try
        {
            // load the struck results for the sequence
            auto results = load_results(sequence + ".boxes");

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
    if (argc < 2)
    {
        std::cerr << "error: at least one sequence is required\n";
        return EXIT_FAILURE;
    }

    for (int a = 1; a < argc; ++a)
        analyze::analyze(argv[a]);

    return 0;
}
