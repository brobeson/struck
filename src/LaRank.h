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

#ifndef LARANK_H
#define LARANK_H

#include "Rect.h"
#include "Sample.h"
#include <memory>
#include <vector>
#include <Eigen/Core>
#include <opencv/cv.h>
#include "Config.h"
#include "Features.h"
#include "Kernels.h"
#include "loss_function.h"
#include "loss_manipulator.h"

/**
 * \brief       A structured output SVM.
 * \details     This is the primary component of the Struck application. It is responsible for
 *              classifying which sample is most likely to be the target, and for updating itself
 *              with new image samples.
 */
class LaRank
{
    public:
        /**
         * \brief       Construct a LaRank structured output SVM.
         * \param[in]   conf        The system wide configuration.
         * \param[in]   features    The list of types of features to extract.
         * \param[in]   kernel      The list of kernel functions to use to evaluate the discriminant
         *                          function.
         * \throws      Unknown
         * \details     This sets up the SVM for tracking and learning. The loss function and loss
         *              manipulator are set up, as is the debug image
         */
        LaRank(const Config& conf, const Features& features, const Kernel& kernel);

        /**
         * \brief   Destroy a LaRank SVM object.
         * \throws  None
         */
        virtual ~LaRank();

        /**
         * \brief       Evaluate the discriminant function for a list of image samples.
         * \param[in]   x           The list of image samples for which to evaluate the discriminant
         *                          function.
         * \param[out]  results     The results of evaluating the discriminant function. This list
         *                          is cleared before evaluating the function.
         * \throws      Unknown
         * \details     This function extracts feature vectors for each image sample. It then
         *              evaluates the discriminant function using the feature vector and translation
         *              for the sample.
         */
        virtual void Eval(const MultiSample& x, std::vector<double>& results);

        /**
         * \brief       Update the SVM with new tracking data.
         * \param[in]   x   The list of image samples to use for updating the SVM.
         * \param[in]   y   The index in the list of samples to the center sample.
         * \throws      Unknown
         */
        virtual void Update(const MultiSample& x, int y);

        /**
         * \brief       Output debugging information.
         * \throws      Unknown
         * \details     This prints the number of support vectors to standard output, updates the
         *              visualization image, and shows it.
         */
        virtual void Debug();

    protected:
        struct SupportPattern
        {
            std::vector<Eigen::VectorXd> x;
            std::vector<FloatRect> yv;
            std::vector<cv::Mat> images;
            int y;
            int refCount;
        };

        struct SupportVector
        {
            SupportPattern* x;
            int y;
            double b;
            double g;
            cv::Mat image;
            float m_fuzziness = 1.0f; ///< The fuzziness factor for use in a fuzzy SVM.
        };

        /// The system wide configuration.
        const Config& m_config;

        /// The list of feature types to extract from frames.
        const Features& m_features;

        /// The kernel function to use for evaluating the discriminant function.
        const Kernel& m_kernel;

        /// The loss function to use with this SVM.
        std::shared_ptr<struck::loss_function> m_pLoss = nullptr;

        /// The loss function manipulator to use with this SVM.
        std::shared_ptr<struck::loss_manipulator> m_pManipulator = nullptr;

        /// The list of support patterns in the SVM.
        std::vector<SupportPattern*> m_sps;

        /// The list of support vectors in the SVM.
        std::vector<SupportVector*> m_svs;

        /// An image visualizing the support vectors.
        cv::Mat m_debugImage;

        /**
         * \brief   The SVM C parameter.
         * \details This is set by the user in the system configuration.
         */
        double m_C;

        /**
         * \brief       A look up table of kernel values.
         * \details     The table is an NxN matrix, for N support patterns. For column i, and row j,
         *              the ij'th entry is the result of evaluating \f$ K(x_i, x_j) \f$.
         */
        Eigen::MatrixXd m_K;

        /**
         * \brief       Evaluate the SVM using Lagrangian duality.
         * \return      The SVM dual function value.
         * \throws      Unknown
         * \details     This function corresponds to equation (9) in Hare et. al.
         *              \f$ & -\sum_{i,y}\delta(y, y_i)\beta_i^y -\frac{1}{2} \sum_{i,y,j,\bar{y}}
         *              \beta_i^y \beta_j^\bar{y} \langle \Phi(x_i, y), \Phi(x_j, \bar{y}) \rangle
         *              \f$
         */
        double ComputeDual() const;

        /**
         * \brief       Run the SMO step to update the gradients and β coefficients for a support
         *              vector.
         * \param[in]   ipos    The index for the positive support vector to update.
         * \param[in]   ineg    The index for the negative support vector to update.
         * \throws      Unknown
         * \details     See the Algorithm 1 in Hare et. al. (page 266) for details.
         */
        void SMOStep(int ipos, int ineg);

        /**
         * \brief       Calculate the minimum gradient for the all the support vectors for a
         *              specific support pattern.
         * \param[in]   ind     The index of the support pattern for which to calculate the minimum
         *                      gradient.
         * \return      A pair consisting of the index in the pattern's list of support vectors for
         *              the vector with the minimum gradient, and that minimum gradient.
         * \throws      Unknown
         * \details     The gradient math is provided by equation (10) in Hare et. al. (page 267).
         *              \f$ g_i(y) = -\delta(y, y_i) - F(x_i, y) \f$
         */
        std::pair<int, double> MinGradient(int ind);

        /**
         * \brief       Run the 'process new' step as described in Hare et. al. (page 267).
         * \param[in]   ind     The index of the support pattern on which to run 'process new'.
         * \throws      Unknown
         * \details     Add two new support vectors for the specified support pattern. One vector is
         *              negative, and the other is positive. The positive support vector is simply
         *              the pattern, and its translation. The negative support vector is the
         *              pattern, and the translation which has minimum gradient.
         * \note        This function will run SMOStep() on the new vectors.
         * \see         MinGradient()
         */
        virtual void ProcessNew(int ind);

        /**
         * \brief       Process existing support vectors.
         * \throws      Unknown
         * \details     This function runs the 'process old' and 'optimize' steps described in Hare
         *              et. al. (page 267). 'Process old' is run once, and 'optimize' is run 10
         *              times.
         * \todo        Why is optimize run 10 times? The paper implies it should be performed once.
         */
        void Reprocess();

        /**
         * \brief       Run the 'process old' step described in Hare et. al. (page 267).
         * \throws      Unknown
         * \details     Process an existing support pattern, chosen at random. The positive support
         *              vector is that with the maximum gradient, and β not equal to 0. The negative
         *              support vector is that with the minimum gradient.
         */
        virtual void ProcessOld();

        /**
         * \brief       Run the 'optimize' step described in Hare et. al (page 267).
         * \throws      Unknown
         * \details     This function optimizes an existing support pattern chosen at random. The
         *              positive support vector is that with the maximum gradient, and β not equal
         *              to 0. The negative support vector is that with the minimum gradient, and β
         *              not equal to 0.
         */
        void Optimize();

        /**
         * \brief       Construct a new support vector and add it to the list of support vectors.
         * \param[in]   x   The support pattern for which to add a support vector.
         * \param[in]   y   The index to the pattern's translation which should be part of the new
         *                  vector.
         * \param[in]   g   The new support vector's gradient.
         * \return      The index of the new support vector within the list of support vectors.
         * \throws      Unknown
         */
        virtual int AddSupportVector(SupportPattern* x, int y, double g);

        /**
         * \brief       Remove a support vector from the SVM.
         * \param[in]   ind     The index of the vector to remove.
         * \throws      Unknown
         * \details     If the vector's pattern reference count is reduced to 0, the pattern is also
         *              removed.
         */
        void RemoveSupportVector(int ind);

        /**
         * \brief       Swap two support vectors within the list.
         * \param[in]   ind1,ind2   The indices of the two vectors to swap.
         * \throws      Unknown
         */
        void SwapSupportVectors(int ind1, int ind2);

        /**
         * \brief   Remove support vectors until the number is within the budget.
         * \throws  Unknown
         */
        void BudgetMaintenance();

        /**
         * \brief       Remove one or two specific support vector from the SVM.
         * \throws      Unknown
         * \details     This locates the negative support vector with the least effect on the
         *              discriminant function and removes it. It adjusts the β of the corresponding
         *              positive support vector. If the new β is 0, the positive support vector is
         *              also removed. Finally, the gradients of the remaining support vectors are
         *              updated.
         */
        void BudgetMaintenanceRemove();

        /**
         * \brief       Evaluate the discriminant function for a given feature vector and
         *              translation.
         * \param[in]   x   The feature vector for which to evaluate the function.
         * \param[in]   y   The translation for which to evaluate the function.
         * \return      The value of the discriminant function:
         *              \f$ F(\mathbf{x}, \mathbf{y}) = \sum_i \beta \kappa \left(\mathbf{x}, \mathbf{x}_i \right) \f$
         * \throws      Unknown
         */
        virtual double Evaluate(const Eigen::VectorXd& x, const FloatRect& y) const;

        void UpdateDebugImage();
};

#endif
