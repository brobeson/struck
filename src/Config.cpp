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

#include "Config.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

SvmType svm_from_string(const std::string& str)
{
    if (str == "larank")
        return SvmType::larank;
    else if (str == "fuzzy")
        return SvmType::fuzzy;
    throw std::invalid_argument(str + " is not a valid type of SVM.");
}

Config::Config(const std::string& path)
{
	SetDefaults();

	ifstream f(path.c_str());
	if (!f)
	{
		cout << "error: could not load config file: " << path << endl;
		return;
	}

	string line, name, tmp;
	while (getline(f, line))
	{
		istringstream iss(line);
		iss >> name >> tmp;

		// skip invalid lines and comments
		if (iss.fail() || tmp != "=" || name[0] == '#') continue;

		if      (name == "seed") iss >> seed;
		else if (name == "quietMode") iss >> quietMode;
		else if (name == "debugMode") iss >> debugMode;
		else if (name == "sequenceBasePath") iss >> sequenceBasePath;
		else if (name == "sequenceName") iss >> sequenceName;
		else if (name == "resultsPath") iss >> resultsPath;
		else if (name == "frameWidth") iss >> frameWidth;
		else if (name == "frameHeight") iss >> frameHeight;
		else if (name == "seed") iss >> seed;
		else if (name == "searchRadius") iss >> searchRadius;
		else if (name == "svmC") iss >> svmC;
		else if (name == "svmBudgetSize") iss >> svmBudgetSize;
        else if (name == "processOld")
        {
            std::string on;
            iss >> on;
            if (on == "off")
                m_processOld = false;
            else if (on != "on")
                std::cerr << "error reading processOld: '" << on << "' is not a valid option. defaulting to on\n";
        }
        else if (name == "optimizeAll")
        {
            std::string on;
            iss >> on;
            if (on == "on")
                m_optimizeAll = true;
            else if (on != "off")
                std::cerr << "error reading optimizeAll: '" << on << "' is not a valid option. defaulting to off\n";
        }
        else if (name == "svm")
        {
            std::string svm_string;
            iss >> svm_string;
            try
            {
                m_svm = svm_from_string(svm_string);
            }
            catch (std::exception& e)
            {
                std::cerr << "error: " << e.what() << "\n"
                          << "       defaulting to LaRank\n";
            }
        }
		else if (name == "feature")
		{
			string featureName, kernelName;
			double param;
			iss >> featureName >> kernelName >> param;

			FeatureKernelPair fkp;

			if      (featureName == FeatureName(kFeatureTypeHaar)) fkp.feature = kFeatureTypeHaar;
			else if (featureName == FeatureName(kFeatureTypeRaw)) fkp.feature = kFeatureTypeRaw;
			else if (featureName == FeatureName(kFeatureTypeHistogram)) fkp.feature = kFeatureTypeHistogram;
            else if (featureName == FeatureName(kFeatureTypeSift)) fkp.feature = kFeatureTypeSift;
			else
			{
				cout << "error: unrecognised feature: " << featureName << endl;
				continue;
			}

			if      (kernelName == KernelName(kKernelTypeLinear)) fkp.kernel = kKernelTypeLinear;
			else if (kernelName == KernelName(kKernelTypeIntersection)) fkp.kernel = kKernelTypeIntersection;
			else if (kernelName == KernelName(kKernelTypeChi2)) fkp.kernel = kKernelTypeChi2;
			else if (kernelName == KernelName(kKernelTypeGaussian))
			{
				if (iss.fail())
				{
					cout << "error: gaussian kernel requires a parameter (sigma)" << endl;
					continue;
				}
				fkp.kernel = kKernelTypeGaussian;
				fkp.params.push_back(param);
			}
			else
			{
				cout << "error: unrecognised kernel: " << kernelName << endl;
				continue;
			}

			features.push_back(fkp);
		}
	}
}

void Config::SetDefaults()
{

	quietMode = false;
	debugMode = false;

	sequenceBasePath = "";
	sequenceName = "";
	resultsPath = "";

	frameWidth = 320;
	frameHeight = 240;

	seed = 0;
	searchRadius = 30;
	svmC = 1.0;
	svmBudgetSize = 0;

	features.clear();
}

std::string Config::FeatureName(FeatureType f)
{
	switch (f)
	{
	case kFeatureTypeRaw:
		return "raw";
	case kFeatureTypeHaar:
		return "haar";
	case kFeatureTypeHistogram:
		return "histogram";
    case kFeatureTypeSift:
        return "sift";
	default:
		return "";
	}
}

std::string Config::KernelName(KernelType k)
{
	switch (k)
	{
	case kKernelTypeLinear:
		return "linear";
	case kKernelTypeGaussian:
		return "gaussian";
	case kKernelTypeIntersection:
		return "intersection";
	case kKernelTypeChi2:
		return "chi2";
	default:
		return "";
	}
}

ostream& operator<< (ostream& out, const Config& conf)
{
	out << "config:" << endl;
	out << "  quietMode          = " << conf.quietMode << endl;
	out << "  debugMode          = " << conf.debugMode << endl;
	out << "  sequenceBasePath   = " << conf.sequenceBasePath << endl;
	out << "  sequenceName       = " << conf.sequenceName << endl;
	out << "  resultsPath        = " << conf.resultsPath << endl;
	out << "  frameWidth         = " << conf.frameWidth << endl;
	out << "  frameHeight        = " << conf.frameHeight << endl;
	out << "  seed               = " << conf.seed << endl;
	out << "  searchRadius       = " << conf.searchRadius << endl;
	out << "  svmC               = " << conf.svmC << endl;
	out << "  svmBudgetSize      = " << conf.svmBudgetSize << endl;
    out << "  svmType            = " << (conf.m_svm == SvmType::fuzzy ? "fuzzy" : "larank") << std::endl
        << "  processOld         = " << (conf.m_processOld ? "on" : "off") << std::endl
        << "  optimizeAll        = " << (conf.m_optimizeAll ? "on" : "off") << std::endl;

	for (int i = 0; i < (int)conf.features.size(); ++i)
	{
		out << "  feature " << i << endl;
		out << "    feature: " << Config::FeatureName(conf.features[i].feature) << endl;
		out << "    kernel:  " << Config::KernelName(conf.features[i].kernel) <<endl;
		if (conf.features[i].params.size() > 0)
		{
			out << "    params: ";
			for (int j = 0; j < (int)conf.features[i].params.size(); ++j)
			{
				out << " " << conf.features[i].params[j];
			}
			out << endl;
		}
	}

	return out;
}
