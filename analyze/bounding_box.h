#ifndef IOU_BOUNDING_BOX_H
#define IOU_BOUNDING_BOX_H

namespace iou
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
            return std::abs(box.left() - box.right()) * std::abs(box.top() - box.bottom());
        }
}

#endif

