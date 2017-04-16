#include "sift_features.h"

namespace struck
{
    /**
     * \brief   The default number of of levels per octave for the SIFT filter.
     * \details This value comes from the VLFeat documentation. See
     *          [`vl_sift_new()`](http://www.vlfeat.org/api/sift_8c.html#adff66a155e30ed412bc8bbb97dfa2fae)
     *          and [the SIFT detector](http://www.vlfeat.org/api/sift.html#sift-intro-detector).
     */
    constexpr int LEVELS = 3;

    /**
     * \brief   The default first octave to use for evaluating SIFT.
     * \details This value comes from the VLFeat documentation. See
     *          [`vl_sift_new()`](http://www.vlfeat.org/api/sift_8c.html#adff66a155e30ed412bc8bbb97dfa2fae)
     *          and [the SIFT detector](http://www.vlfeat.org/api/sift.html#sift-intro-detector).
     */
    constexpr int FIRST_OCTAVE = 0;

    sift_features::sift_features(const int width, const int height):
        m_pFilter(vl_sift_new(width,
                              height,
                              std::log2(std::min(width, height)),
                              struck::LEVELS,
                              struck::FIRST_OCTAVE))
    {}


    sift_features::~sift_features() noexcept
    {
        vl_sift_delete(m_pFilter);
    }
}
