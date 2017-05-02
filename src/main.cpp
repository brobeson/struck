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

#include <chrono>
#include <iostream>
#include <fstream>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "sift_features.h"
#include "fuzzy_larank.h"

using namespace std;
using namespace cv;

static const int kLiveBoxWidth = 80;
static const int kLiveBoxHeight = 80;

void rectangle(Mat& rMat, const FloatRect& rRect, const Scalar& rColour, int thickness = 1)
{
    IntRect r(rRect);
    rectangle(rMat, Point(r.XMin(), r.YMin()), Point(r.XMax(), r.YMax()), rColour, thickness);
}

namespace struck
{
    void write_sample_output(const std::string& sequence_name,
                             const int frame_number,
                             const cv::Mat& image,
                             const FloatRect& bounding_box,
                             const FloatRect& fuzzy_bounding_box)
    {
        // add the bounding boxes to the image
        cv::Mat result = image.clone();
        rectangle(result, bounding_box, CV_RGB(255, 0, 0), 1);
        rectangle(result, fuzzy_bounding_box, CV_RGB(0, 255, 0), 1);

        cv::imwrite(sequence_name + "_" + std::to_string(frame_number) + ".png", result);
    }

    void write_sample_output(const std::string& sequence_name,
                             const cv::Mat& image,
                             const FloatRect& bounding_box,
                             const std::vector<FloatRect>& samples)
    {
        // calculate the length of the image diagonal
        const auto size = image.size();
        const auto max_length = std::sqrt(size.width * size.width + size.height * size.height);

        // add the bounding box to the image
        cv::Mat result = image.clone();
        rectangle(result, bounding_box, CV_RGB(0, 255, 0));

        // figure out the center point of the bounding box
        cv::Point center(0.5 * bounding_box.Width()  + bounding_box.XMin(),
                         0.5 * bounding_box.Height() + bounding_box.YMin());

        // calculate fuzziness values, then rescale to the range [0.1, 1.0]. this is exaggerate the
        // effect of the fuzziness for visualization purposes.
        std::vector<float> s_values(samples.size(), 1.0f);
        for (std::size_t i = 0; i < samples.size(); ++i)
        {
            cv::Point p(0.5 * samples[i].Width()  + samples[i].XMin(),
                        0.5 * samples[i].Height() + samples[i].YMin());
            const auto distance = std::sqrt(std::pow(p.x - center.x, 2.0f) + std::pow(p.y - center.y, 2.0f));
            s_values[i] = fuzziness(distance, max_length);
        }
        const auto min_s = *std::min_element(s_values.cbegin(), s_values.cend());
        constexpr float remapped_min_s = 0.3f;
        std::transform(s_values.begin(), s_values.end(), s_values.begin(),
                       [min_s, remapped_min_s](const float s)
                       {
                           return remapped_min_s + (s - min_s) * (1.0f - remapped_min_s) / (1.0f - min_s);
                       });

        // add filled circles at the sample points
        constexpr int filled_circle = -1; // specify to OpenCV to fill the circle
        for (std::size_t i = 0; i < samples.size(); ++i)
        {
            cv::Point p(0.5 * samples[i].Width()  + samples[i].XMin(),
                        0.5 * samples[i].Height() + samples[i].YMin());

            //cv::circle(result, p, 1, CV_RGB(s_values[i] * 255, 0, 0), filled_circle);
            cv::circle(result, p, 1, CV_RGB(255, 0, 0), filled_circle);
        }

        cv::imwrite(sequence_name + ".png", result);
    }
}

int main(int argc, char* argv[])
{
    // read config file
    string configPath = "config.txt";
    if (argc > 1)
    {
        configPath = argv[1];
    }
    Config conf(configPath);
    cout << conf << endl;

    if (conf.features.size() == 0)
    {
        cout << "error: no features specified in config" << endl;
        return EXIT_FAILURE;
    }

    ofstream outFile;
    if (conf.resultsPath != "")
    {
        outFile.open(conf.resultsPath.c_str(), ios::out);
        if (!outFile)
        {
            cout << "error: could not open results file: " << conf.resultsPath << endl;
            return EXIT_FAILURE;
        }
    }

    // if no sequence specified then use the camera
    bool useCamera = (conf.sequenceName == "");

    VideoCapture cap;

    int startFrame = -1;
    int endFrame = -1;
    string imgFormat;
    float scaleW = 1.f;
    float scaleH = 1.f;
    auto start_time = std::chrono::system_clock::now();

    if (useCamera)
    {
        if (!cap.open(0))
        {
            cout << "error: could not start camera capture" << endl;
            return EXIT_FAILURE;
        }
        startFrame = 0;
        endFrame = INT_MAX;
        Mat tmp;
        cap >> tmp;
        scaleW = (float)conf.frameWidth/tmp.cols;
        scaleH = (float)conf.frameHeight/tmp.rows;

        conf.bounding_box = IntRect(conf.frameWidth/2-kLiveBoxWidth/2, conf.frameHeight/2-kLiveBoxHeight/2, kLiveBoxWidth, kLiveBoxHeight);
        //cout << "press 'i' to initialise tracker" << endl;
        std::cout << "tracker will initialize in 10 seconds\n";
    }
    else
    {
        // parse frames file
        string framesFilePath = conf.sequenceBasePath+"/"+conf.sequenceName+"/"+conf.sequenceName+"_frames.txt";
        ifstream framesFile(framesFilePath.c_str(), ios::in);
        if (!framesFile)
        {
            cout << "error: could not open sequence frames file: " << framesFilePath << endl;
            return EXIT_FAILURE;
        }
        string framesLine;
        getline(framesFile, framesLine);
        sscanf(framesLine.c_str(), "%d,%d", &startFrame, &endFrame);
        if (framesFile.fail() || startFrame == -1 || endFrame == -1)
        {
            cout << "error: could not parse sequence frames file" << endl;
            return EXIT_FAILURE;
        }

        imgFormat = conf.sequenceBasePath+"/"+conf.sequenceName+"/imgs/img%05d.png";

        // read first frame to get size
        char imgPath[256];
        sprintf(imgPath, imgFormat.c_str(), startFrame);
        Mat tmp = cv::imread(imgPath, 0);
        scaleW = (float)conf.frameWidth/tmp.cols;
        scaleH = (float)conf.frameHeight/tmp.rows;

        // read init box from ground truth file
        string gtFilePath = conf.sequenceBasePath+"/"+conf.sequenceName+"/"+conf.sequenceName+"_gt.txt";
        ifstream gtFile(gtFilePath.c_str(), ios::in);
        if (!gtFile)
        {
            cout << "error: could not open sequence gt file: " << gtFilePath << endl;
            return EXIT_FAILURE;
        }
        string gtLine;
        getline(gtFile, gtLine);
        float xmin = -1.f;
        float ymin = -1.f;
        float width = -1.f;
        float height = -1.f;
        sscanf(gtLine.c_str(), "%f,%f,%f,%f", &xmin, &ymin, &width, &height);
        if (gtFile.fail() || xmin < 0.f || ymin < 0.f || width < 0.f || height < 0.f)
        {
            cout << "error: could not parse sequence gt file" << endl;
            return EXIT_FAILURE;
        }
        conf.bounding_box = FloatRect(xmin*scaleW, ymin*scaleH, width*scaleW, height*scaleH);
    }

    Tracker tracker(conf);
    if (!conf.quietMode)
    {
        namedWindow("result");
    }

    Mat result(conf.frameHeight, conf.frameWidth, CV_8UC3);
    bool paused = false;
    bool doInitialise = false;
    srand(conf.seed);
    //sift::feature_list sf(conf.bounding_box.Width(), conf.bounding_box.Height());
    for (int frameInd = startFrame; frameInd <= endFrame; ++frameInd)
    {
        Mat frame;
        if (useCamera)
        {
            Mat frameOrig;
            cap >> frameOrig;
            resize(frameOrig, frame, Size(conf.frameWidth, conf.frameHeight));
            flip(frame, frame, 1);
            frame.copyTo(result);
            if (doInitialise)
            {
                if (tracker.IsInitialised())
                {
                    if (conf.m_svm == SvmType::larank)
                        tracker.Reset<LaRank>();
                    else if (conf.m_svm == SvmType::fuzzy)
                        tracker.Reset<struck::fuzzy_larank>();
                    else
                    {
                        std::cerr << "error: the configuration SVM type is invalid\n";
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    tracker.Initialise(frame, conf.bounding_box);
                }
                doInitialise = false;
            }
            else if (!tracker.IsInitialised())
            {
                rectangle(result, conf.bounding_box, CV_RGB(255, 255, 255));
                auto d = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start_time);
                if (d.count() < 5)
                {
                    std::cout << "tracker will initialize in " << 5 - d.count() << " seconds\n";
                }
                else
                    doInitialise = true;
            }
        }
        else
        {
            char imgPath[256];
            sprintf(imgPath, imgFormat.c_str(), frameInd);
            Mat frameOrig = cv::imread(imgPath, 0);
            if (frameOrig.empty())
            {
                cout << "error: could not read frame: " << imgPath << endl;
                return EXIT_FAILURE;
            }
            resize(frameOrig, frame, Size(conf.frameWidth, conf.frameHeight));
            cvtColor(frame, result, CV_GRAY2RGB);

            if (frameInd == startFrame)
            {
                tracker.Initialise(frame, conf.bounding_box);
            }
        }

        if (tracker.IsInitialised())
        {
//            if (frameInd == startFrame)
//            {
//                cv::Rect bb(conf.bounding_box.XMin(), conf.bounding_box.YMin(), conf.bounding_box.Width(), conf.bounding_box.Height());
//                //cv::Mat first_frame = Mat(frame, bb).clone();
//                cv::Mat first_frame;
//                cv::cvtColor(Mat(frame, bb), first_frame, cv::COLOR_GRAY2RGB);
//
//                //auto first_frame = frame.clone();
//
//                sf.evaluate(first_frame);
//                std::ofstream sf_file("sift.txt");
//                if (sf_file)
//                    sf_file << sf;
//
//                const auto list = sf.list();
//                for (const auto& feature : list)
//                {
//                    cv::circle(first_frame,
//                               cv::Point(feature.keypoint().x(), feature.keypoint().y()),
//                               feature.keypoint().scale(),
//                               Scalar(0, 0, 255, 255));
//                }
//
//                cv::imwrite((conf.sequenceName + ".png").c_str(), first_frame);
//            }

            tracker.Track(frame);

            if (!conf.quietMode && conf.debugMode)
            {
                tracker.Debug();
            }
            else
            {
                std::cout << ".";
                cout.flush();
            }

            //if (frameInd == startFrame + 23)
                //struck::write_sample_output(conf.sequenceName, frameInd, result, tracker.GetBB(), fuzzy_tracker.GetBB());
            //struck::write_sample_output(conf.sequenceName, result, tracker.GetBB(), tracker.update_samples());
            //rectangle(result, tracker.GetBB(), CV_RGB(0, 255, 0));
            //cv::imwrite(conf.sequenceName + ".png", result);

            if (outFile)
            {
                const FloatRect& bb = tracker.GetBB();
                outFile << bb.XMin()/scaleW << "," << bb.YMin()/scaleH << "," << bb.Width()/scaleW << "," << bb.Height()/scaleH << endl;
            }
        }

        if (!conf.quietMode)
        {
            imshow("result", result);
            int key = waitKey(paused ? 0 : 1);
            if (key != -1)
            {
                if (key == 27 || key == 113) // esc q
                {
                    break;
                }
                else if (key == 112) // p
                {
                    paused = !paused;
                }
                else if (key == 105 && useCamera)
                {
                    doInitialise = true;
                }
            }
            if (conf.debugMode && frameInd == endFrame)
            {
                cout << "\n\nend of sequence, press any key to exit" << endl;
                waitKey();
            }
        }
    }

    std::cout << std::endl;
    if (outFile.is_open())
    {
        outFile.close();
    }

    return EXIT_SUCCESS;
}
