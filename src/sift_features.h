#ifndef STRUCK_SIFT_H
#define STRUCK_SIFT_H

#include <ostream>
#include <opencv/cv.h>
#include "Features.h"
extern "C" {
    #include <vl/sift.h>
}

namespace sift
{
    class keypoint_t final
    {
        public:
            keypoint_t() = default;

            keypoint_t(const VlSiftKeypoint& keypoint): m_keypoint(keypoint) {}

            keypoint_t(const keypoint_t&) = default;

            keypoint_t(keypoint_t&&) = default;

            ~keypoint_t() noexcept = default;

            keypoint_t& operator=(const keypoint_t&) = default;

            keypoint_t& operator=(keypoint_t&&) = default;

            int octave() const noexcept { return m_keypoint.o; }
            int x_as_integer() const noexcept { return m_keypoint.ix; }
            int y_as_integer() const noexcept { return m_keypoint.iy; }
            int s_as_integer() const noexcept { return m_keypoint.is; }
            float x() const noexcept { return m_keypoint.x; }
            float y() const noexcept { return m_keypoint.y; }
            float s() const noexcept { return m_keypoint.s; }
            float scale() const noexcept { return m_keypoint.sigma; }

        private:
            VlSiftKeypoint m_keypoint { 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f };
    };

    class descriptor_t final
    {
        public:
            /// Size of a VLFeat SIFT descriptor.
            static constexpr std::size_t length = 128u;

            using size_type = std::size_t;

            descriptor_t() = default;

            descriptor_t(const vl_sift_pix* const d)
            {
                std::copy_n(d, length, m_descriptor.begin());
            }

            descriptor_t(const descriptor_t&) = default;

            descriptor_t(descriptor_t&&) = default;

            ~descriptor_t() noexcept = default;

            descriptor_t& operator=(const descriptor_t&) = default;

            descriptor_t& operator=(descriptor_t&&) = default;

            float operator[](const std::size_t i) const
            {
                if (i < length)
                    return m_descriptor[i];
                throw std::out_of_range(std::to_string(i) +
                                        " exceeds the length (" +
                                        std::to_string(length) +
                                        ") of a SIFT descriptor.");
            }

            float& operator[](const std::size_t i)
            {
                if (i < length)
                    return m_descriptor[i];
                throw std::out_of_range(std::to_string(i) +
                                        " exceeds the length (" +
                                        std::to_string(length) +
                                        ") of a SIFT descriptor.");
            }

        private:
            std::array<float, length> m_descriptor;
    };

    class feature_t final
    {
        public:
            feature_t() = default;

            feature_t(const keypoint_t& k, const descriptor_t& d):
                m_keypoint(k),
                m_descriptor(d)
            {}

            feature_t(const feature_t&) = default;

            feature_t(feature_t&&) = default;

            ~feature_t() noexcept = default;

            feature_t& operator=(const feature_t&) = default;

            feature_t& operator=(feature_t&&) = default;

            keypoint_t keypoint() const noexcept { return m_keypoint; }
            descriptor_t descriptor() const noexcept { return m_descriptor; }

        private:
            keypoint_t   m_keypoint;   ///< The SIFT feature's key point (geometric data)
            descriptor_t m_descriptor; ///< The SIFT feature's descriptor
    };

    /// Encapsulate a set of SIFT features.
    class feature_list final: public Features
    {
        public:
            /// Default construction of SIFT features is not allowed.
            feature_list() = delete;

            /**
             * \brief       Construct a set of SIFT features.
             * \param[in]   width,height    The dimensions of the images for which SIFT features will be
             *                              extracted.
             * \throws      std::bad_alloc  This will be thrown if allocation of a SIFT filter fails.
             */
            feature_list(const int width, const int height);

            /**
             * \copydoc     feature_list(const int, const int)
             * \param[in]   width,height    The dimensions of the images for which SIFT features will be
             *                              extracted.
             * \param[in]   octaves         The number of octaves to use. This is clamped in such to
             *                              \f$ \log_2 \left( \min(width, height ) \right) \f$.
             * \throws      std::bad_alloc  This will be thrown if allocation of a SIFT filter fails.
             */
            feature_list(const int width, const int height, const int octaves);

            /**
             * \brief   Copy a set of SIFT features.
             * \throws  Unknown
             */
            feature_list(const feature_list&) = default;

            /**
             * \brief   Move a set of SIFT features.
             * \throws  Unknown
             */
            feature_list(feature_list&&) = default;

            /**
             * \brief   Destroy a set of SIFT features.
             * \throws  None
             */
            ~feature_list() noexcept;

            /**
             * \brief   Copy a set of SIFT features.
             * \return  A reference to this set of SIFT features.
             * \throws  Unknown
             */
            feature_list& operator=(const feature_list&) = default;

            /**
             * \brief   Move a set of SIFT features.
             * \return  A reference to this set of SIFT features.
             * \throws  Unknown
             */
            feature_list& operator=(feature_list&&) = default;

            void push_back(const feature_t& f);

            std::vector<feature_t> list() const { return m_features; }

            void evaluate(const cv::Mat& cvImage);

        protected:
            virtual void UpdateFeatureVector(const Sample& s) override;

        private:
            VlSiftFilt* m_pFilter = nullptr; ///< The filter used by VLFeat to extract SIFT features.
            std::vector<feature_t> m_features;
            cv::Mat m_patchImage;
    };

    std::ostream& operator<<(std::ostream& output, const feature_list& f);
    std::ostream& operator<<(std::ostream& output, const feature_t& f);
    std::ostream& operator<<(std::ostream& output, const keypoint_t& k);
    std::ostream& operator<<(std::ostream& output, const descriptor_t& d);
}

#endif
