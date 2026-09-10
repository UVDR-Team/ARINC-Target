// SPDX-License-Identifier: MPL-2.0
#ifndef ARINC_SUPPORT_INPUT_ERROR_HPP
#define ARINC_SUPPORT_INPUT_ERROR_HPP
#if ARINC_ENABLE_COMMAND_LINE
#include <boost/program_options/errors.hpp>
namespace ArincSupport { using InvalidOptionValue = boost::program_options::invalid_option_value; }
#else
#include <stdexcept>
#include <string>
namespace ArincSupport {
// Stream parsing remains available without linking a desktop option parser.
class InvalidOptionValue : public std::invalid_argument
{
public:
  explicit InvalidOptionValue(const std::string &value) :
    std::invalid_argument("the argument ('" + value + "') for option is invalid") {}
};
}
#endif
#endif
