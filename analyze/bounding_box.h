#ifndef IOU_BOUNDING_BOX_H
#define IOU_BOUNDING_BOX_H

#include <cmath>

namespace analyze
{
    /**
     * \brief       Represents a bounding box on an image.
     * \tparam      T   The data type representing the box coordinates.
     * \details     The bounding box assumes image coordinates increase left to right, and top to
     *              bottom.
     */
    template <class T>
    class bounding_box final
    {
        public:
            /// The data type representing the bounding box coordinates.
            using value_type = T;

            /**
             * \brief       Construct a default bounding box.
             * \throws      None
             * \details     The box coordinates are all set to 0.
             */
            bounding_box() = default;

            /**
             * \brief       Construct a bounding box.
             * \param[in]   left,right,top,bottom   The coordinates representing the sides of the box.
             * \throws      None
             */
            bounding_box(const value_type& left,
                         const value_type& right,
                         const value_type& top,
                         const value_type& bottom) noexcept:
                m_left(left),
                m_right(right),
                m_top(top),
                m_bottom(bottom)
            {}

            /**
             * \brief   Copy a bounding box.
             * \throws  None
             */
            bounding_box(const bounding_box&) = default;

            /**
             * \brief   Move a bounding box.
             * \throws  None
             */
            bounding_box(bounding_box&&) = default;

            /**
             * \brief   Destroy a bounding box.
             * \throws  None
             */
            ~bounding_box() noexcept = default;

            /**
             * \brief   Copy a bounding box.
             * \return  A reference to this bounding box.
             * \throws  None
             */
            bounding_box& operator=(const bounding_box&) = default;

            /**
             * \brief   Move a bounding box.
             * \return  A reference to this bounding box.
             * \throws  None
             */
            bounding_box& operator=(bounding_box&&) = default;

            /**
             * \brief   Query the box's left side coordinate.
             * \return  The coordinate of the left side of the box.
             * \throws  None
             */
            value_type left() const noexcept { return m_left; }

            /**
             * \brief   Query the box's right side coordinate.
             * \return  The coordinate of the right side of the box.
             * \throws  None
             */
            value_type right() const noexcept { return m_right; }

            /**
             * \brief   Query the box's top side coordinate.
             * \return  The coordinate of the top side of the box.
             * \throws  None
             */
            value_type top() const noexcept { return m_top; }

            /**
             * \brief   Query the box's bottom side coordinate.
             * \return  The coordinate of the bottom side of the box.
             * \throws  None
             */
            value_type bottom() const noexcept { return m_bottom; }

        private:
            value_type m_left   = 0, ///< The coordinate for the box's left side.
                       m_right  = 0, ///< The coordinate for the box's right side.
                       m_top    = 0, ///< The coordinate for the box's top.
                       m_bottom = 0; ///< The coordinate for the box's bottom.
    };

    /**
     * \brief       Calculate the area of a bounding box.
     * \tparam      T       The data type of the bounding box coordinates.
     * \param[in]   box     The bounding box for which to calculate the area.
     * \return      The area of the bounding box, measured in pixel coordinates.
     * \throws      None
     */
    template <class T>
        T area(const bounding_box<T>& box) noexcept
        {
            return std::fabs(box.left() - box.right()) * std::fabs(box.top() - box.bottom());
        }

    /**
     * \brief       Calculate the intersection of two bounding boxes.
     * \tparam      T           The data type of the bounding box coordinates.
     * \param[in]   box1,box2   The boxes for which to calculate an intersection.
     * \return      A bounding box which represents the intersection of \a box1 and \a box2.
     * \throws      std::runtime_error  This is thrown if the two boxes have no intersection.
     */
    template <class T>
        bounding_box<T> intersection(const bounding_box<T>& box1, const bounding_box<T>& box2)
        {
            if ((box1.bottom() < box2.top())  ||
                (box2.bottom() < box2.top())  ||
                (box1.right()  < box2.left()) ||
                (box2.right()  < box2.left()))
            {
                throw std::runtime_error("box 1 and box 2 do not intersect");
            }

            return bounding_box<T>(std::max(box1.left(),   box2.left()),
                                   std::min(box1.right(),  box2.right()),
                                   std::max(box1.top(),    box2.top()),
                                   std::min(box1.bottom(), box2.bottom()));
        }
}

#endif

