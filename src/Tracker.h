/*
 * Struck: Structured Output Tracking with Kernels
 *
 * Code to accompany the paper:
 *   Struck: Structured Output Tracking with Kernels
 *   Sam Hare, Amir Saffari, Philip H. S. Torr
 *   International Conference on Computer Vision (ICCV), 2011
 *
 * Copyright (C) 2011 Sam Hare, Oxford Brookes University, Oxford, UK
 *
 * This file is part of Struck.
 *
 * Struck is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Struck is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Struck.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef TRACKER_H
#define TRACKER_H

#include "Rect.h"

#include <vector>
#include <Eigen/Core>
#include <opencv/cv.h>
#include "fuzzy_larank.h"
#include "HaarFeatures.h"
#include "HistogramFeatures.h"
#include "MultiFeatures.h"
#include "RawFeatures.h"
#include "sift_features.h"

class Config;
class Features;
class Kernel;
class LaRank;
class ImageRep;

/**
 * \brief       The class responsible for tracking the target frame to frame.
 * \details     This is the primary workhorse of the Struck application. This is responsible for
 *              managing the SVM used for classifying target vs. non-target within the images or
 *              video frames. The tracker locates the target within a given sequence frame, then
 *              updates the SVM for future tracking.
 */
class Tracker
{
    public:
        /**
         * \brief       Construct a tracker object.
         * \param[in]   conf        The system configuration.
         * \throws      Unknown     Anything thrown by constructing data members is allowed to
         *                          propagate.
         * \details     This will set up a LaRank structured output SVM for learning.
         * \see         Reset()
         */
        Tracker(const Config& conf);

        /**
         * \brief   Destroy a tracker object.
         * \throws  None
         */
        ~Tracker();

        /**
         * \brief       Initialize the tracker for tracking a target in a video or image sequence.
         * \param[in]   frame   The first frame of the sequence.
         * \param[in]   bb      The bounding box for the target within the first \a frame.
         * \throws      Unknown
         * \details     This function will set the initial target bounding box, and update the SVM
         *              so that it learns from the first frame and target bounding box.
         */
        void Initialise(const cv::Mat& frame, FloatRect bb);

        /**
         * \brief       Reset how the tracker tracks a target.
         * \tparam      SVM_type    The type of SVM to use for learning. This must be LaRank, or a
         *                          class derived from LaRank.
         * \throws      Unknown
         * \warning     This will delete the current SVM state. All learned data will be lost. This
         *              probably shouldn't be executed in the middle of tracking.
         * \details     This function will set up a new SVM for learning the tracking discriminant
         *              function. It resets all the types of features extracted from the image
         *              samples, and the kernel functions used for evaluating the discriminant
         *              function.
         */
        template <class SVM_type>
            void Reset();

        /**
         * \brief       Attempt to the locate the target in a sequence frame.
         * \param[in]   frame   The image or video frame in which to locate the target.
         * \throws      Unknown.
         * \details     This function will attempt to locate the target within the given \a frame.
         *              It samples the frame around the bounding box location from the previous
         *              frame. For each sample, it will extract features, which are used to evaluate
         *              the discriminant function. The sample with the highest discriminant function
         *              value is regarded as the new target location. After locating the target,
         *              this function will update the SVM.
         */
        void Track(const cv::Mat& frame);

        /**
         * \brief       Show debugging visualizations.
         * \throws      Unknown
         * \details     This will show visualizations useful for debugging and analysis. It will
         *              also have the SVM do the same.
         */
        void Debug();

        /**
         * \brief   Get the current bounding box of the target.
         * \throws  None
         */
        const FloatRect& GetBB() const { return m_bb; }

        /**
         * \brief   Query if the tracker has been initialized.
         * \retval  true    The tracker has been initialized.
         * \retval  false   The tracker has not been initialized.
         * \throws  None
         * \see     Initialise()
         */
        bool IsInitialised() const { return m_initialised; }

        /**
         * \brief   Get the set of samples used for updating the learner.
         * \return  A copy of the update samples.
         * \throws  None
         */
        std::vector<FloatRect> update_samples() const noexcept { return m_update_samples; }

    private:
        /// The system-wide configuration.
        const Config& m_config;

        /// True indicates that the tracker has been initialized with a first frame and target
        /// bounding box.
        bool m_initialised;

        /// A list of types of features to extract from image samples.
        std::vector<Features*> m_features;

        /// A list of kernel functions to use for evaluating the discriminant function.
        std::vector<Kernel*> m_kernels;

        /// The SVM for classifying feature vectors.
        LaRank* m_pLearner;

        /**
         * \brief   The bounding box of the target.
         * \warning This is only applicable for the last frame submitted to the Track() function.
         */
        FloatRect m_bb;

        /**
         * \brief   An image visualizing tracking data.
         * \details This is useful for debugging and for analyzing performance.
         * \todo    Figure out what this visualization represents.
         */
        cv::Mat m_debugImage;

        bool m_needsIntegralImage;
        bool m_needsIntegralHist;

        /// The list of samples used to update the learner.
        std::vector<FloatRect> m_update_samples;

        /**
         * \brief       Update the SVM.
         * \param[in]   image   Unknown
         * \throws      Unknown
         * \details     After locating the target in the current frame, this function updates the
         *              SVM so that it learns from the new target location and representation. This
         *              samples the image around the target bounding box, then causes the SVM to
         *              update itself with those samples.
         * \todo        Figure out what an ImageRep object is.
         */
        void UpdateLearner(const ImageRep& image);

        void UpdateDebugImage(const std::vector<FloatRect>& samples, const FloatRect& centre, const std::vector<double>& scores);
};

template <class SVM_type>
void Tracker::Reset()
{
    static_assert(std::is_base_of<LaRank, SVM_type>::value,
                  "error: SVM_type must be derived from LaRank");

    m_initialised = false;
    m_debugImage.setTo(0);
    if (m_pLearner) delete m_pLearner;
    for (int i = 0; i < (int)m_features.size(); ++i)
    {
        delete m_features[i];
        delete m_kernels[i];
    }
    m_features.clear();
    m_kernels.clear();

    m_needsIntegralImage = false;
    m_needsIntegralHist = false;

    int numFeatures = m_config.features.size();
    std::vector<int> featureCounts;
    for (int i = 0; i < numFeatures; ++i)
    {
        switch (m_config.features[i].feature)
        {
        case Config::kFeatureTypeHaar:
            m_features.push_back(new HaarFeatures(m_config));
            m_needsIntegralImage = true;
            break;
        case Config::kFeatureTypeRaw:
            m_features.push_back(new RawFeatures(m_config));
            break;
        case Config::kFeatureTypeHistogram:
            m_features.push_back(new HistogramFeatures(m_config));
            m_needsIntegralHist = true;
            break;
        case Config::kFeatureTypeSift:
            m_features.emplace_back(new sift::feature_list(m_config.bounding_box.Width(),
                                                           m_config.bounding_box.Height()));
            break;
        /// \todo Uh... what about a default?
        }
        featureCounts.push_back(m_features.back()->GetCount());

        switch (m_config.features[i].kernel)
        {
        case Config::kKernelTypeLinear:
            m_kernels.push_back(new LinearKernel());
            break;
        case Config::kKernelTypeGaussian:
            m_kernels.push_back(new GaussianKernel(m_config.features[i].params[0]));
            break;
        case Config::kKernelTypeIntersection:
            m_kernels.push_back(new IntersectionKernel());
            break;
        case Config::kKernelTypeChi2:
            m_kernels.push_back(new Chi2Kernel());
            break;
        /// \todo Uh... what about a default?
        }
    }

    if (numFeatures > 1)
    {
        MultiFeatures* f = new MultiFeatures(m_features);
        m_features.push_back(f);

        MultiKernel* k = new MultiKernel(m_kernels, featureCounts);
        m_kernels.push_back(k);
    }

    m_pLearner = new SVM_type(m_config, *m_features.back(), *m_kernels.back());
}
#endif
