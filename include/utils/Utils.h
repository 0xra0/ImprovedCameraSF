#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

namespace IC::Utils {

    inline std::vector<std::string> Split(const std::string& str, char delimiter)
    {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    inline std::string ToLower(const std::string& str)
    {
        std::string result = str;
        std::ranges::transform(result, result.begin(), ::tolower);
        return result;
    }

    inline std::string ToUpper(const std::string& str)
    {
        std::string result = str;
        std::ranges::transform(result, result.begin(), ::toupper);
        return result;
    }

    inline bool Contains(const std::string& str, const std::string& substr)
    {
        return str.find(substr) != std::string::npos;
    }

    inline std::string Replace(const std::string& str, const std::string& from, const std::string& to)
    {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }
}
