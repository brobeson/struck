#ifndef IOU_BOUNDING_BOX_H
#define IOU_BOUNDING_BOX_H

#include <cmath>

namespace analyze
{
    template <class T>
    class bounding_box final
    {
        public:
            using value_type = T;

            bounding_box() = default;

            bounding_box(const value_type& left,
                         const value_type& right,
                         const value_type& top,
                         const value_type& bottom):
                m_left(left),
                m_right(right),
                m_top(top),
                m_bottom(bottom)
            {}

            bounding_box(const bounding_box&) = default;

            bounding_box(bounding_box&&) = default;

            ~bounding_box() noexcept = default;

            bounding_box& operator=(const bounding_box&) = default;

            bounding_box& operator=(bounding_box&&) = default;

            value_type left() const noexcept { return m_left; }
            value_type right() const noexcept { return m_right; }
            value_type top() const noexcept { return m_top; }
            value_type bottom() const noexcept { return m_bottom; }

        private:
            value_type m_left   = 0, ///< The coordinate for the box's left side.
                       m_right  = 0, ///< The coordinate for the box's right side.
                       m_top    = 0, ///< The coordinate for the box's top.
                       m_bottom = 0; ///< The coordinate for the box's bottom.
    };

    template <class T>
        T area(const bounding_box<T>& box)
        {
            return std::fabs(box.left() - box.right()) * std::fabs(box.top() - box.bottom());
        }

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

