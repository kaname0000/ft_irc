#include "utils.hpp"

bool isValidName(const std::string &name, const std::string allowedSymbols, size_t maxNameSize)
{
    if (name.empty())
        return false;
    // The first character is an alphabet
    if (!isalpha(name[0]))
        return false;
    for (size_t i = 0; i < name.size(); i++)
    {
        if (!isalnum(name[i]) && allowedSymbols.find(name[i]) == std::string::npos)
            return false;
    }
    if (name.size() > maxNameSize)
        return false;
    return true;
}
