#include "sift_features.h"

namespace sift
{
    //---------------------------------------------------------------------------------------------
    //                                                                  local functions and data
    //---------------------------------------------------------------------------------------------
    /**
     * \brief   The default number of of levels per octave for the SIFT filter.
     * \details This value comes from the VLFeat documentation. See
     *          [`vl_sift_new()`](http://www.vlfeat.org/api/sift_8c.html#adff66a155e30ed412bc8bbb97dfa2fae)
     *          and [the SIFT detector](http://www.vlfeat.org/api/sift.html#sift-intro-detector).
     */
    constexpr int levels_per_octave = 3;

    /**
     * \brief   The default first octave to use for evaluating SIFT.
     * \details This value comes from the VLFeat documentation. See
     *          [`vl_sift_new()`](http://www.vlfeat.org/api/sift_8c.html#adff66a155e30ed412bc8bbb97dfa2fae)
     *          and [the SIFT detector](http://www.vlfeat.org/api/sift.html#sift-intro-detector).
     */
    constexpr int first_octave = 0;

    /**
     * \brief       Convert an OpenCV image matrix to a VLFeat image.
     * \param[in]   cvImage     The OpenCV image to convert.
     * \return      A vector of floating point data as expected by VLFeat SIFT operations.
     * \throws      None
     * \todo        This needs to be made more robust. It seems OpenCV images aren't necessarily
     *              stored as assumed here.
     */
    std::vector<float> opencvToVlfeat(const cv::Mat& cvImage) noexcept
    {
        std::vector<float> vlImage(cvImage.rows * cvImage.cols, 0.0f);
        std::vector<float>::size_type v = 0;
        for (int r = 0; r < cvImage.rows; ++r)
        {
            const auto pRow = cvImage.ptr<uchar>(r);
            for (int c = 0; c < cvImage.cols; ++c)
            {
                vlImage[v] = pRow[c];
                ++v;
            }
        }
        return vlImage;
    }

    void process(VlSiftFilt* pFilter, feature_list& feats) noexcept
    {
        vl_sift_detect(pFilter);
        auto pKeypoints = vl_sift_get_keypoints(pFilter);
        const auto count = vl_sift_get_nkeypoints(pFilter);

        double angles[] = { 0.0, 0.0, 0.0, 0.0 };
        int number_of_angles = 0;
        vl_sift_pix descriptor[128];

        for (int k = 0; k < count; ++k)
        {
            keypoint_t key(pKeypoints[k]);

            number_of_angles = vl_sift_calc_keypoint_orientations(pFilter, angles, &(pKeypoints[k]));
            for (int a = 0; a < number_of_angles; ++a)
            {
                vl_sift_calc_keypoint_descriptor(pFilter, descriptor, &(pKeypoints[k]), angles[a]);
                feats.push_back(feature_t(key, descriptor_t(descriptor)));
            }
        }
    }

    std::ostream& operator<<(std::ostream& output, const feature_list& f)
    {
        const auto list = f.list();
        for (const auto& l : list)
            output << l << std::endl;
        return output;
    }

    std::ostream& operator<<(std::ostream& output, const feature_t& f)
    {
        return output << f.keypoint() << " | " << f.descriptor();
    }

    std::ostream& operator<<(std::ostream& output, const keypoint_t& k)
    {
        return output << k.octave() << ": (" << k.x_as_integer() << ", " << k.y_as_integer() << ") ("
                                             << k.x()            << ", " << k.y()            << ") "
                                             << k.s_as_integer() << ", " << k.s() << " x" << k.scale();
    }

    std::ostream& operator<<(std::ostream& output, const descriptor_t& descriptor)
    {
        for (descriptor_t::size_type i = 0; i < descriptor_t::length; ++i)
            output << descriptor[i] << " ";
        return output;
    }

    //---------------------------------------------------------------------------------------------
    //                                                                      sift features methods
    //---------------------------------------------------------------------------------------------
    feature_list::feature_list(const int width, const int height):
        m_pFilter(vl_sift_new(width,
                              height,
                              std::log2(std::min(width, height)),
                              levels_per_octave,
                              first_octave))
    {}


    feature_list::~feature_list() noexcept
    {
        vl_sift_delete(m_pFilter);
    }

    void feature_list::evaluate(const cv::Mat& cvImage)
    {
        if (!m_pFilter)
            return;

        auto vlImage = opencvToVlfeat(cvImage);

        // process the first octave
        auto error = VL_ERR_OK;vl_sift_process_first_octave(m_pFilter, vlImage.data());
        process(m_pFilter, *this);

        while (error != VL_ERR_OK)
        {
           error = vl_sift_process_next_octave(m_pFilter);
        }
    }

    void feature_list::push_back(const feature_t& f)
    {
        m_features.push_back(f);
    }
}
