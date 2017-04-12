#ifndef ANALYZE_IOU_H
#define ANALYZE_IOU_H

#include "bounding_box.h"

namespace analyze
{
    template <class T>
    class iou final
    {
        public:
            using box_type = T;

            using iou_type = float;

            iou() = default;

            iou(const bounding_box<box_type>& box1, const bounding_box<box_type>& box2) noexcept
            {
                try
                {
                    const float intersection_area = area(intersection(box1, box2));
                    const float area1 = area(box1);
                    const float area2 = area(box2);

                    m_iou = intersection_area / (area1 + area2 - intersection_area);
                }
                catch (...)
                {
                    m_iou = 0.0f;
                }
            }

            iou(const iou&) = default;

            iou(iou&&) = default;

            ~iou() noexcept = default;

            iou& operator=(const iou&) = default;

            iou& operator=(iou&&) = default;

            iou_type value() const noexcept { return m_iou; }

        private:
            float m_iou = 0.0f;
    };
}

#endif

