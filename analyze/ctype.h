#ifndef IOU_CTYPE_H
#define IOU_CTYPE_H

#include <algorithm>
#include <locale>

namespace analyze
{
    /**
     * \brief       Custom character classification to consider commas as delimiters.
     * \details     See [std::ctype](http://en.cppreference.com/w/cpp/locale/ctype). The example
     *              there is a bit different than this; it may be the more appropriate approach.
     *              This implementation is taken from [this SO question]
     *              (http://stackoverflow.com/questions/10376199/how-can-i-use-non-default-delimiters-when-reading-a-text-file-with-stdfstream)
     */
    class ctype final: public std::ctype<char>
    {
        mask m_table[table_size];

        public:
        ctype(const size_t refs = 0):
            std::ctype<char>(&m_table[0], false, refs)
        {
            std::copy_n(classic_table(), table_size, m_table);
            m_table[','] = static_cast<mask>(space);
        }
    };
}

#endif

