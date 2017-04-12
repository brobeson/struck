#ifndef ANALYZE_IOU_H
#define ANALYZE_IOU_H

#include "bounding_box.h"

namespace analyze
{
    /**
     * \brief       Represents an intersection-over-union (IoU) metric between two bounding boxes.
     * \details     The formula for an IoU is:
     *              \f$ IoU(B,G) = \frac{B \cap G}{B \cup G} \f$
     *              This is implemented as
     *              \f$ IoU(B,G) = \frac{A(B \cap G)}{A(B) + A(G) - A(B \cap G)} \f$
     *              where \f$ A(\dot) \f$ is the area of \f$ \dot \f$.
     */
    class iou final
    {
        public:
            /// The type of the IoU value.
            using iou_type = float;

            /**
             * \brief   Construct a default IoU.
             * \throws  None
             * \details The IoU value is 0.
             */
            iou() = default;

            /**
             * \brief       Construct an IoU object with the specified value.
             * \param[in]   value   The value to assign to the IoU.
             * \throws      None
             */
            explicit iou(const float& value) noexcept : m_value(value) {}

            /**
             * \brief       Construct an IoU for two bounding boxes.
             * \tparam      T           The data type of the bounding box coordinates.
             * \param[in]   box1,box2   The two bounding boxes for which to calculate the IoU.
             * \throws      None
             * \todo        Handle the case of division by 0
             */
            template <class T>
                iou(const bounding_box<T>& box1, const bounding_box<T>& box2) noexcept
                {
                    try
                    {
                        const float intersection_area = area(intersection(box1, box2));
                        const float area1 = area(box1);
                        const float area2 = area(box2);
                        m_value = intersection_area / (area1 + area2 - intersection_area);
                    }
                    catch (...)
                    {
                        m_value = 0.0f;
                    }
                }

            /**
             * \brief   Copy an IoU object.
             * \throws  None
             */
            iou(const iou&) = default;

            /**
             * \brief   Move an IoU object.
             * \throws  None
             */
            iou(iou&&) = default;

            /**
             * \brief   Destroy an IoU object.
             * \throws  None
             */
            ~iou() noexcept = default;

            /**
             * \brief   Copy an IoU object.
             * \return  A reference to this IoU object.
             * \throws  None
             */
            iou& operator=(const iou&) = default;

            /**
             * \brief   Move an IoU object.
             * \return  A reference to this IoU object.
             * \throws  None
             */
            iou& operator=(iou&&) = default;

            /**
             * \brief   Query the IoU value.
             * \return  The IoU value. This is a proportion, on [0, 1].
             * \throws  None
             */
            iou_type value() const noexcept { return m_value; }

        private:
            float m_value = 0.0f; ///< The value of the IoU.
    };

    bool operator<(const iou& a, const iou& b) noexcept
    {
        return a.value() < b.value();
    }

    iou operator+(const iou& a, const iou& b) noexcept
    {
        return iou(a.value() + b.value());
    }
}

#endif

