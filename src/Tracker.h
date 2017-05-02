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

class Tracker
{
public:
    Tracker(const Config& conf);
    ~Tracker();

    void Initialise(const cv::Mat& frame, FloatRect bb);
    template <class SVM_type>
        void Reset();
    void Track(const cv::Mat& frame);
    void Debug();

    inline const FloatRect& GetBB() const { return m_bb; }
    inline bool IsInitialised() const { return m_initialised; }

    /**
     * \brief   Get the set of samples used for updating the learner.
     * \return  A copy of the update samples.
     * \throws  None
     */
    std::vector<FloatRect> update_samples() const noexcept { return m_update_samples; }

private:
    const Config& m_config;
    bool m_initialised;
    std::vector<Features*> m_features;
    std::vector<Kernel*> m_kernels;
    LaRank* m_pLearner;
    FloatRect m_bb;
    cv::Mat m_debugImage;
    bool m_needsIntegralImage;
    bool m_needsIntegralHist;
    std::vector<FloatRect> m_update_samples; ///< The list of samples used to update the learner.

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
