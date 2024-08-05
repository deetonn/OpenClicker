#pragma once

#include <string>
#include <vector>

inline std::vector<std::string_view> string_split(
    const std::string_view str, 
    const std::string_view delimiter
)
{
    std::vector<std::string_view> v;
    if (!str.empty()) {
        int start = 0;
        do {
            // Find the index of occurrence
            int idx = str.find(delimiter, start);
            if (idx == std::string::npos) {
                break;
            }

            // If found add the substring till that
            // occurrence in the vector
            auto length = static_cast<std::size_t>(idx - start);
            v.push_back(str.substr(start, length));
            start += (length + delimiter.size());
        } while (true);
        v.push_back(str.substr(start));
    }

    return v;
}

template <class Type>
using NonOwnedPtr = Type*;