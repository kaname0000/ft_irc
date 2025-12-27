/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:26:48 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:26:49 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

bool isValidName(const std::string &name, const std::string allowedSymbols, size_t maxNameSize)
{
    if (name.empty())
        return false;
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
