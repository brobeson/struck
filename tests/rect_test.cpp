#include <QtTest/QtTest>
#include "Rect.h"

// this allows Rect<int> objects to be added to the data table
Q_DECLARE_METATYPE(Rect<int>)

namespace struck
{
    /// A set of unit tests for the Rect class and associated functions.
    class rectangle_test final: public QObject
    {
        Q_OBJECT

        public:
            /**
             * \brief   Construct a set of rectangle unit tests.
             * \throws  None
             */
            rectangle_test() = default;

            /**
             * \brief   Copy a set of rectangle unit tests.
             * \throws  None
             */
            rectangle_test(const rectangle_test&) = default;

            /**
             * \brief   Move a set of rectangle unit tests.
             * \throws  None
             */
            rectangle_test(rectangle_test&&) = default;

            ~rectangle_test() noexcept = default;

            /**
             * \brief   Copy a set of rectangle unit tests.
             * \return  A reference to this set of unit tests.
             * \throws  None
             */
            rectangle_test& operator=(const rectangle_test&) = default;

            /**
             * \brief   Move a set of rectangle unit tests.
             * \return  A reference to this set of unit tests.
             * \throws  None
             */
            rectangle_test& operator=(rectangle_test&&) = default;

        private slots:
            /**
             * \brief   Generate test data for testing diagonal length.
             * \throws  None
             */
            void diagonal_length_data() noexcept;

            /**
             * \brief   Test the diagonal length function for rectangles.
             * \throws  None
             */
            void diagonal_length() noexcept;
    };

    void rectangle_test::diagonal_length_data() noexcept
    {
        QTest::addColumn<Rect<int>>("rectangle");
        QTest::addColumn<int>("length");

        QTest::newRow("0_width")                   << Rect<int>(0, 0,   0,  10) << 10;
        QTest::newRow("0_height")                  << Rect<int>(0, 0,  10,   0) << 10;
        QTest::newRow("0_area")                    << Rect<int>(0, 0,   0,   0) <<  0;
        QTest::newRow("negative_width")            << Rect<int>(0, 0, -10,  10) << 14;
        QTest::newRow("negative_height")           << Rect<int>(0, 0,  10, -10) << 14;
        QTest::newRow("negative_width_and_height") << Rect<int>(0, 0, -10, -10) << 14;
        QTest::newRow("positive_width_and_height") << Rect<int>(0, 0,  10,  10) << 14;
    }

    void rectangle_test::diagonal_length() noexcept
    {
        QFETCH(Rect<int>, rectangle);
        QTEST(::diagonal_length<int>(rectangle), "length");
    }
}

QTEST_MAIN(struck::rectangle_test);
#include "rect_test.moc"
