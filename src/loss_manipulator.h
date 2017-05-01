#ifndef STRUCK_LOSS_MANIPULATOR_H
#define STRUCK_LOSS_MANIPULATOR_H

/**
 * \file
 * \brief   Provide a set of functions to manipulate the loss function.
 * \author  Brendan Robeson
 */

namespace struck
{
    /**
     * \brief       An interface for every loss function manipulator.
     * \details     Any loss function manipulator must accept as input a floating point value on
     *              [0, 1], and return a floating point value on [0, 1].
     */
    class loss_manipulator
    {
        public:
            /**
             * \brief   Construct a loss function manipulator.
             * \throws  None
             */
            loss_manipulator() = default;

            /**
             * \brief   Copy a loss manipulator.
             * \throws  None
             */
            loss_manipulator(const loss_manipulator&) = default;

            /**
             * \brief   Move a loss manipulator.
             * \throws  None
             */
            loss_manipulator(loss_manipulator&&) = default;

            /**
             * \brief   Destroy a loss manipulator.
             * \throws  None
             */
            virtual ~loss_manipulator() noexcept = default;

            /**
             * \brief   Copy a loss manipulator.
             * \return  A reference to this loss manipulator.
             * \throws  None
             */
            loss_manipulator& operator=(const loss_manipulator&) = default;

            /**
             * \brief   Move a loss manipulator.
             * \return  A reference to this loss manipulator.
             * \throws  None
             */
            loss_manipulator& operator=(loss_manipulator&&) = default;

            /**
             * \brief       Evaluate the loss manipulator.
             * \param[in]   t   The original loss function result to manipulate. This must be on
             *                  [0, 1].
             * \return      A value on [0, 1] that is the adjustment of \a t.
             * \throws      None
             */
            virtual float evaluate(const float t) const noexcept = 0;
    };

    /**
     * \brief       The identity loss manipulator.
     * \details     \f$ f(t) = t \f$.
     */
    class identity final: public loss_manipulator
    {
        public:
            /**
             * \brief   Construct an identity loss manipulator.
             * \throws  None
             */
            identity() = default;

            /**
             * \brief   Copy an identity loss manipulator.
             * \throws  None
             */
            identity(const identity&) = default;

            /**
             * \brief   Move an identity loss manipulator.
             * \throws  None
             */
            identity(identity&&) = default;

            /**
             * \brief   Destroy an identity loss manipulator.
             * \throws  None
             */
            virtual ~identity() noexcept = default;

            /**
             * \brief   Copy an identity loss manipulator.
             * \return  A reference to this identity loss manipulator.
             * \throws  None
             */
            identity& operator=(const identity&) = default;

            /**
             * \brief   Move an identity loss manipulator.
             * \return  A reference to this identity loss manipulator.
             * \throws  None
             */
            identity& operator=(identity&&) = default;

            /**
             * \brief       Evaluate the identity loss manipulator.
             * \param[in]   t   The original loss function result to manipulate. This must be on
             *                  [0, 1].
             * \return      \a t.
             * \throws      None
             */
            virtual float evaluate(const float t) const noexcept override { return t; }
    };

    /**
     * \brief       A loss manipulator based on the smooth step function.
     * \details     \f$ f(t) = 3t^2 - 2t^3 \f$
     */
    class smooth_step final: public loss_manipulator
    {
        public:
            /**
             * \brief   Construct a smooth step manipulator.
             * \throws  None
             */
            smooth_step() = default;

            /**
             * \brief   Copy a smooth step manipulator.
             * \throws  None
             */
            smooth_step(const smooth_step&) = default;

            /**
             * \brief   Move a smooth step manipulator.
             * \throws  None
             */
            smooth_step(smooth_step&&) = default;

            /**
             * \brief   Destroy a smooth step manipulator.
             * \throws  None
             */
            virtual ~smooth_step() noexcept = default;

            /**
             * \brief   Copy a smooth step manipulator.
             * \return  A reference to this smooth step manipulator.
             * \throws  None
             */
            smooth_step& operator=(const smooth_step&) = default;

            /**
             * \brief   Move a smooth step manipulator.
             * \return  A reference to this smooth step manipulator.
             * \throws  None
             */
            smooth_step& operator=(smooth_step&&) = default;

            /**
             * \brief       Evaluate the smooth step loss manipulator.
             * \param[in]   t   The original loss function result to manipulate. This must be on
             *                  [0, 1].
             * \return      \f$ 3t^2 - 2t^3 \f$
             * \throws      None
             */
            virtual float evaluate(const float t) const noexcept override
            {
                // 3tt - 2ttt = (3 - 2t)tt
                // this transformation of the function has fewer multiplication operations.
                return (3.0f - 2.0f * t) * t * t;
            }
    };
}

#endif
