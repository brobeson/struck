#ifndef STRUCK_LOSS_FUNCTION_H
#define STRUCK_LOSS_FUNCTION_H

/**
 * \file
 * \brief   Provide a set of loss functions for the SVM.
 * \author  Brendan Robeson
 */

namespace struck
{
    /**
     * \brief   An interface specifying how to use a structured output SVM loss function.
     * \details To use a loss function, construct a function object, and use the evaluate()
     *          function:
     * \code
     * std::shared_ptr<loss_function> pLoss = std::make_shared<iou_loss>();
     * FloatRect some_box;
     * FloatRect another_box;
     *
     * const auto loss_value = pLoss->evaluate(some_box, another_box);
     * \endcode
     */
    class loss_function
    {
        public:
            /**
             * \brief   Construct a loss function object.
             * \throws  None
             */
            loss_function() = default;

            /**
             * \brief   Copy a loss function object.
             * \throws  None
             */
            loss_function(const loss_function&) = default;

            /**
             * \brief   Move a loss function object.
             * \throws  None
             */
            loss_function(loss_function&&) = default;

            /**
             * \brief   Destroy a loss function object.
             * \throws  None
             */
            virtual ~loss_function() noexcept = default;

            /**
             * \brief   Copy a loss function object.
             * \return  A reference to this loss function object.
             * \throws  None
             */
            loss_function& operator=(const loss_function&) = default;

            /**
             * \brief   Move a loss function object.
             * \return  A reference to this loss function object.
             * \throws  None
             */
            loss_function& operator=(loss_function&&) = default;

            /**
             * \brief       Evaluate the loss function for two bounding boxes.
             * \param[in]   current     The potential bounding box of the new target position.
             * \param[in]   potential   The current bounding box of the target.
             * \return      The loss for the given bounding boxes.
             * \throws      None
             */
            virtual float evaluate(const FloatRect& current, const FloatRect& potential) const noexcept = 0;
    };

    /**
     * \brief   A loss function evaluated as bounding box intersection over union.
     * \details     \f$ f \left( \mathbf{p}, \mathbf{p}_0 \right) = 1 -
     *                  \frac{\mathbf{p} \cap \mathbf{p}_0}
     *                       {\mathbf{p} \cup \mathbf{p}_0}
     *              \f$
     */
    class iou_loss final: public loss_function
    {
        public:
            /**
             * \brief   Construct an IoU loss function object.
             * \throws  None
             */
            iou_loss() = default;

            /**
             * \brief   Copy an IoU loss function object.
             * \throws  None
             */
            iou_loss(const iou_loss&) = default;

            /**
             * \brief   Move an IoU loss function object.
             * \throws  None
             */
            iou_loss(iou_loss&&) = default;

            /**
             * \brief   Destroy an IoU loss function object.
             * \throws  None
             */
            virtual ~iou_loss() noexcept = default;

            /**
             * \brief   Copy an IoU loss function object.
             * \return  A reference to this IoU loss function object.
             * \throws  None
             */
            iou_loss& operator=(const iou_loss&) = default;

            /**
             * \brief   Move an IoU loss function object.
             * \return  A reference to this IoU loss function object.
             * \throws  None
             */
            iou_loss& operator=(iou_loss&&) = default;

            /**
             * \brief       Evaluate IoU loss for a pair of bounding boxes.
             * \param[in]   current     The current bounding box of the target.
             * \param[in]   potential   The potential new bounding box.
             * \return      One minus the IoU value for the two bounding boxes.
             * \throws      None
             */
            virtual float evaluate(const FloatRect& current, const FloatRect& potential) const noexcept override
            {
                return 1.0 - potential.Overlap(current);
            }
    };

    /**
     * \brief       A loss function evaluated as amount of displacement between two bounding boxes.
     * \details     \f$ f(\mathbf{p}) = \frac{\|\mathbf{p}\|}{\|mathbf{p}_{max}} \f$
     */
    class distance_loss final: public loss_function
    {
        public:
            /**
             * \brief       Construct a distance loss function object.
             * \param[in]   maximum_distance    The maximum distance a translation can be. This must
             *                                  be greater than 0.
             * \throws      std::invalid_argument   This is thrown if \a maximum_distance is 0.
             */
            distance_loss(const float maximum_distance):
                m_max_distance(maximum_distance)
            {
                if (std::fabs(m_max_distance) <= 0.00001f)
                    throw std::invalid_argument("maximum distance in distance_loss must be greater than 0");
            }

            /**
             * \brief   Copy a distance loss function object.
             * \throws  None
             */
            distance_loss(const distance_loss&) = default;

            /**
             * \brief   Move a distance loss function object.
             * \throws  None
             */
            distance_loss(distance_loss&&) = default;

            /**
             * \brief   Destroy a distance loss function object.
             * \throws  None
             */
            virtual ~distance_loss() noexcept = default;

            /**
             * \brief   Copy a distance loss function object.
             * \return  A reference to this distance loss function.
             * \throws  None
             */
            distance_loss& operator=(const distance_loss&) = default;

            /**
             * \brief   Move a distance loss function object.
             * \return  A reference to this distance loss function.
             * \throws  None
             */
            distance_loss& operator=(distance_loss&&) = default;

            /**
             * \brief       Evaluate IoU loss for a pair of bounding boxes.
             * \param[in]   potential   The potential new bounding box.
             * \return      The ratio of the bounding box's translation distance to the maximum
             *              distance.
             * \throws      None
             */
            virtual float evaluate(const FloatRect& /*current*/, const FloatRect& potential) const noexcept override
            {
                auto d = std::sqrt(potential.XMin() * potential.XMin() +
                                   potential.YMin() * potential.YMin());
                return d / m_max_distance;
            }

        private:
            float m_max_distance = 0.0f; ///< The maximum distance a translation can be.
    };
}

#endif
