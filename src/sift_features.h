#ifndef STRUCK_SIFT_H
#define STRUCK_SIFT_H

#include "Features.h"

extern "C"
{
    #include <vl/sift.h>
}

/// Encapsulate a set of SIFT features.
class sift_features final: public Features
{
    public:
        /// Default construction of SIFT features is not allowed.
        sift_features() = delete;

        /**
         * \brief       Construct a set of SIFT features.
         * \param[in]   width,height    The dimensions of the images for which SIFT features will be
         *                              extracted.
         * \throws      std::bad_alloc  This will be thrown if allocation of a SIFT filter fails.
         */
        sift_features(const int width, const int height);

        /**
         * \brief   Copy a set of SIFT features.
         * \throws  Unknown
         */
        sift_features(const sift_features&) = default;

        /**
         * \brief   Move a set of SIFT features.
         * \throws  Unknown
         */
        sift_features(sift_features&&) = default;

        /**
         * \brief   Destroy a set of SIFT features.
         * \throws  None
         */
        ~sift_features() noexcept;

        /**
         * \brief   Copy a set of SIFT features.
         * \return  A reference to this set of SIFT features.
         * \throws  Unknown
         */
        sift_features& operator=(const sift_features&) = default;

        /**
         * \brief   Move a set of SIFT features.
         * \return  A reference to this set of SIFT features.
         * \throws  Unknown
         */
        sift_features& operator=(sift_features&&) = default;

        //virtual void Eval(const MultiSample& s, std::vector<Eigen::VectorXd>& featVecs) override;

    protected:
        /// \todo Implement this.
        virtual void UpdateFeatureVector(const Sample& s) override;

    private:
        VlSiftFilt* m_pFilter = nullptr; ///< The filter used by VLFeat to extract SIFT features.
};

#endif
