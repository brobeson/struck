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

#include "Tracker.h"
#include "Config.h"
#include "ImageRep.h"
#include "Sampler.h"
#include "Sample.h"
#include "GraphUtils/GraphUtils.h"


#include "Kernels.h"

#include "LaRank.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <Eigen/Core>

#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;
using namespace Eigen;

Tracker::Tracker(const Config& conf) :
	m_config(conf),
	m_initialised(false),
	m_pLearner(0),
	m_debugImage(2*conf.searchRadius+1, 2*conf.searchRadius+1, CV_32FC1),
	m_needsIntegralImage(false)
{
	Reset<LaRank>();
}

Tracker::~Tracker()
{
	delete m_pLearner;
	for (int i = 0; i < (int)m_features.size(); ++i)
	{
		delete m_features[i];
		delete m_kernels[i];
	}
}


void Tracker::Initialise(const cv::Mat& frame, FloatRect bb)
{
	m_bb = IntRect(bb);
	ImageRep image(frame, m_needsIntegralImage, m_needsIntegralHist);
	for (int i = 0; i < 1; ++i)
	{
		UpdateLearner(image);
	}
	m_initialised = true;
}

void Tracker::Track(const cv::Mat& frame)
{
	assert(m_initialised);

	ImageRep image(frame, m_needsIntegralImage, m_needsIntegralHist);

	vector<FloatRect> rects = Sampler::PixelSamples(m_bb, m_config.searchRadius);

	vector<FloatRect> keptRects;
	keptRects.reserve(rects.size());
	for (int i = 0; i < (int)rects.size(); ++i)
	{
		if (!rects[i].IsInside(image.GetRect())) continue;
		keptRects.push_back(rects[i]);
	}

	MultiSample sample(image, keptRects);

	vector<double> scores;
	m_pLearner->Eval(sample, scores);

	double bestScore = -DBL_MAX;
	int bestInd = -1;
	for (std::vector<double>::size_type i = 0; i < scores.size(); ++i)
	{
		if (scores[i] > bestScore)
		{
			bestScore = scores[i];
			bestInd = i;
		}
	}

	UpdateDebugImage(keptRects, m_bb, scores);

	if (bestInd != -1)
	{
		m_bb = keptRects[bestInd];
		UpdateLearner(image);
#if VERBOSE
		cout << "track score: " << bestScore << endl;
#endif
	}
}

void Tracker::UpdateDebugImage(const vector<FloatRect>& samples, const FloatRect& centre, const vector<double>& scores)
{
	double mn = VectorXd::Map(&scores[0], scores.size()).minCoeff();
	double mx = VectorXd::Map(&scores[0], scores.size()).maxCoeff();
	m_debugImage.setTo(0);
	for (int i = 0; i < (int)samples.size(); ++i)
	{
		int x = (int)(samples[i].XMin() - centre.XMin());
		int y = (int)(samples[i].YMin() - centre.YMin());
		m_debugImage.at<float>(m_config.searchRadius+y, m_config.searchRadius+x) = (float)((scores[i]-mn)/(mx-mn));
	}
}

void Tracker::Debug()
{
	imshow("tracker", m_debugImage);
	m_pLearner->Debug();
}

void Tracker::UpdateLearner(const ImageRep& image)
{
	// note these return the centre sample at index 0
	vector<FloatRect> rects = Sampler::RadialSamples(m_bb, 2*m_config.searchRadius, 5, 16);
	//vector<FloatRect> rects = Sampler::PixelSamples(m_bb, 2*m_config.searchRadius, true);

	vector<FloatRect> keptRects;
	keptRects.push_back(rects[0]); // the true sample
	for (int i = 1; i < (int)rects.size(); ++i)
	{
		if (!rects[i].IsInside(image.GetRect())) continue;
		keptRects.push_back(rects[i]);
	}

#if VERBOSE
	cout << keptRects.size() << " samples" << endl;
#endif

	MultiSample sample(image, keptRects);
	m_pLearner->Update(sample, 0);
    m_update_samples = sample.GetRects();
}
