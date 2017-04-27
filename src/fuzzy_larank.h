#ifndef FUZZY_LA_RANK_H
#define FUZZY_LA_RANK_H

#include "LaRank.h"

namespace struck
{
    /**
     * \brief       Calculate the fuzziness score for a specified distance.
     * \param[in]   distance            The distance for which to calculate the fuzziness.
     * \param[in]   maximum_distance    The distance which should result in a fuzziness of 0.
     * \return      The fuzziness as a function of distance:
     *              \f$ f \left(d, d_{max} \right) = 1 - \frac{d}{d_{max}} \f$
     */
    float fuzziness(const float distance, const float maximum_distance);

    /**
     * \brief       This is a fuzzy version of LaRank SVM.
     * \details     This applies a fuzzy weight to the SVM output. Use it just like you use a LaRank
     *              object.
     */
    class fuzzy_larank final: public LaRank
    {
        public:
            /**
             * \brief       Construct a fuzzy LaRank SVM.
             * \param[in]   configuration   The application configuration.
             * \param[in]   features        The set of features to extract from the imagery or
             *                              video frames.
             * \param[in]   kernel          The kernel function to use for solving the SVM.
             * \throws      std::runtime_error      This is thrown if the image has 0 area (0 with
             *                                      and 0 height). Such an image would result in a
             *                                      division by 0.
             */
            fuzzy_larank(const Config& configuration, const Features& features, const Kernel& kernel);

            /**
             * \brief   Copy a fuzzy SVM.
             * \throws  Unknown
             */
            fuzzy_larank(const fuzzy_larank&) = default;

            /**
             * \brief   Move a fuzzy SVM.
             * \throws  Unknown
             */
            fuzzy_larank(fuzzy_larank&&) = default;

            /**
             * \brief   Destroy a fuzzy SVM.
             * \throws  None
             */
            ~fuzzy_larank() noexcept = default;

            /**
             * \brief   Copy a fuzzy SVM.
             * \return  A reference to this fuzzy SVM.
             * \throws  Unknown
             */
            fuzzy_larank& operator=(const fuzzy_larank&) = default;

            /**
             * \brief   Move a fuzzy SVM.
             * \return  A reference to this fuzzy SVM.
             * \throws  Unknown
             */
            fuzzy_larank& operator=(fuzzy_larank&&) = default;

        private:
            /**
             * \brief       Add a new support vector to the SVM.
             * \param[in]   x   The support pattern (a feature vector).
             * \param[in]   y   The index to the support pattern's list of transformations for the
             *                  new support vector's transformation.
             * \param[in]   g   The gradient value.
             * \return      The index into the list of support vectors for the new vector.
             */
            virtual int AddSupportVector(SupportPattern* x, int y, double g) override;

            /**
             * \brief       Evaluate the discriminant function for a given support pattern and
             *              transformation.
             * \param[in]   x   The support pattern to evaluate.
             * \param[in]   y   The transformation for the support pattern's bounding box.
             * \return      The discriminant function value for \a x and \a y.
             * \throws      Unknown
             * \details     This invokes the basic LaRank evaluation, then scales it by a fuzziness
             *              function of \a y.
             */
            virtual double Evaluate(const Eigen::VectorXd& x, const FloatRect& y) const override;

            /**
             * \brief       The farthest possible translation in the image.
             * \details     The longest possible translation is from one corner to another corner.
             *              Thus, this is set to the distance of that diagonal:
             *              \f$ d_{max} = \sqrt{width^2 + height^2} \f$
             */
            int m_farthest_distance = 1;
    };
}

#endif
