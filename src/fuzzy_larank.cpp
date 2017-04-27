#include "fuzzy_larank.h"
#include <utility>

namespace struck
{
    //------------------------------------------------------------------------------------------
    //                                                                   local data & functions
    //------------------------------------------------------------------------------------------
    /**
     * \brief       Calculate the fuzziness score for a specified distance.
     * \param[in]   distance            The distance for which to calculate the fuzziness.
     * \param[in]   maximum_distance    The distance which should result in a fuzziness of 0.
     * \return      The fuzziness as a function of distance:
     *              \f$ f \left(d, d_{max} \right) = 1 - \frac{d}{d_{max}} \f$
     */
    float fuzziness(const float distance, const float maximum_distance)
    {
        return 1.0f - (distance / maximum_distance);
    }

    //------------------------------------------------------------------------------------------
    //                                                                        fuzzy SVM methods
    //------------------------------------------------------------------------------------------
    fuzzy_larank::fuzzy_larank(const Config& configuration,
                               const Features& features,
                               const Kernel& kernel):
        LaRank(configuration, features, kernel),
        m_farthest_distance(std::sqrt(configuration.frameWidth  * configuration.frameWidth +
                                      configuration.frameHeight * configuration.frameHeight))
    {
        if (m_farthest_distance == 0)
            throw std::runtime_error("A fuzzy LaRank SVM requires that the image has some non-zero area.");
    }

    int fuzzy_larank::AddSupportVector(SupportPattern* x, int y, double g)
    {
        const auto index = LaRank::AddSupportVector(x, y, g);
        m_svs[index]->m_fuzziness = fuzziness(diagonal_length<float>(x->yv[y]), m_farthest_distance);
        return index;
    }

    double fuzzy_larank::Evaluate(const Eigen::VectorXd& x, const FloatRect& y) const
    {
        //double f = 0.0;
        //for (int i = 0; i < (int)m_svs.size(); ++i)
        //{
        //    const SupportVector& sv = *m_svs[i];
        //    f += sv.b * m_kernel.Eval(x, sv.x->x[sv.y]);
        //}
        //return f * fuzziness(diagonal_length<float>(y), m_farthest_distance);
        return fuzziness(diagonal_length<float>(y), m_farthest_distance) * LaRank::Evaluate(x, y);
    }
}
