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

#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>
#include <ostream>
#include "Rect.h"

#define VERBOSE (0)

/// The types of support vector machines available.
enum class SvmType
{
    larank, ///< LaRank structured output SVM.
    fuzzy   ///< Fuzzy (weighted) LaRank structured output SVM.
};

/// The type of loss functions available.
enum class LossFunctionType
{
    IOU,     ///< The default function: \f$ 1 - s_{\mathbf{p}_t}^o(\mathbf{y}, \mathbf{\bar{y}}) \f$.
    DISTANCE ///< A loss function based on distance: \f$ 1 - \frac{\|\mathbf{y}\|}{\|\mathbf{y}_max\|} \f$.
};

class Config
{
public:
	Config() { SetDefaults(); }
	Config(const std::string& path);

	enum FeatureType
	{
		kFeatureTypeHaar,
		kFeatureTypeRaw,
		kFeatureTypeHistogram,
        kFeatureTypeSift
	};

	enum KernelType
	{
		kKernelTypeLinear,
		kKernelTypeGaussian,
		kKernelTypeIntersection,
		kKernelTypeChi2
	};

	struct FeatureKernelPair
	{
		FeatureType feature;
		KernelType kernel;
		std::vector<double> params;
	};

	bool							quietMode;
	bool							debugMode;

	std::string						sequenceBasePath;
	std::string						sequenceName;
	std::string						resultsPath;

	int								frameWidth;
	int								frameHeight;

	int								seed;
	int								searchRadius;
	double							svmC;
	int								svmBudgetSize;
	std::vector<FeatureKernelPair>	features;
	FloatRect                       bounding_box;            ///< The bounding box used for tracking.
    SvmType                         m_svm = SvmType::larank; ///< The type of SVM to use for tracking.
    bool                            m_processOld = true;     ///< True -> run the process old step.
    bool                            m_optimizeAll = false;   ///< True -> run optimize step on all support vectors

    /// The type of loss function to use.
    LossFunctionType                m_loss = LossFunctionType::IOU; 

	friend std::ostream& operator<< (std::ostream& out, const Config& conf);

private:
	void SetDefaults();
	static std::string FeatureName(FeatureType f);
	static std::string KernelName(KernelType k);
};

#endif
