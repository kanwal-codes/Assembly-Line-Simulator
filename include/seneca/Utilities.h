#ifndef SENECA_UTILITIES_H
#define SENECA_UTILITIES_H

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <numeric>

namespace seneca
{
    class Utilities {
        size_t m_widthField{1};
        static char m_delimiter;

        public:
            Utilities() = default;
            void setFieldWidth(size_t newWidth);
            size_t getFieldWidth() const;
            std::string extractToken(const std::string& str, size_t& next_pos, bool& more);
            static void setDelimiter(char newDelimiter);
            static char getDelimiter();
    };
} // namespace seneca

#endif