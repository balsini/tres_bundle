/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2014,2015, ReTiS Lab., Scuola Superiore Sant'Anna.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the ReTiS Lab. nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/**
 * \file ParseUtils.hpp
 *
 * \note This file is taken from the MetaSim distribution and adapted for
 * use with tres_base. The original author is gratefully aknowledged for
 * sharing their code under the 3-clause BSD license.
 *
 * \author Giuseppe Lipari (copyright holder)
 */

#ifndef TRES_PARSEUTILS_HDR
#define TRES_PARSEUTILS_HDR
#include <vector>
#include "../../src/BaseExc.hpp"

/**
 * \addtogroup tres_utils
 * @{
 */
/**
 * This namespace contains useful functions for parsing strings. Such
 * functions are used by the abstract factory that creates random variables
 * and by developers who want to use the factory machinery provided by tres::Factory
 */
namespace tres_parse_utils
{
    /**
     * \brief Removes trailing spaces from the beginning and from the end of
     * the input string
     */
    std::string remove_spaces(const std::string &);

    /**
     * \brief Given a string consisting of many substrings separated
     * by a sequence, returns a vector containing the substrings.
     */
    std::vector<std::string> split(const std::string &, const std::string &, const std::string &, const std::string &);

    /**
     * \brief Given a string consisting of many substrings separated by ';',
     * returns a vector containing each substring
     *
     * \note The last substring \em must include a ';', otherwise it is ignored
     * \todo Add sintax error checking
     */
    std::vector<std::string> split_instr(const std::string &);

    /**
     * \brief Given a string of the form "token(p1)" returns the token.
     */
    std::string get_token(const std::string &, const std::string &open_par = "(");

    /**
     * \brief Given a string of the form "token(p1,p2)" returns the parameters "p1,p2".
     */
    std::string get_param(const std::string &, const std::string &open_par = "(",
                     const std::string &close_par = ")");

    /**
     * \brief Given a string of the form "token = param1, param2" returns the parameters
     * "param1, param2" (i.e. it returns everything after the separation symbol)
     */
    std::string get_param2(const std::string &, const std::string &open_par = "=");

    /**
     * \brief Given a string containing the parameters "(p1,p2)", returns a
     * vector of strings containing the parameters p1 and p2.
     */
    std::vector<std::string> split_param(const std::string &, const std::string &sep = ",",
                               char open_par = '(', char close_par = ')');
  
    /**
     * \brief Given a string of the form "123.75ms" returns the double number
     * at the beginning of the string in \c res and the unit of measure in
     * \c unit.
     */
    void parse_double(const std::string &nums, double &res, std::string &unit);

    /**
     * \brief Exception raised by the above functions
     */
    class ParseExc : public tres::BaseExc
    {

    public:

        /**
         * \brief Constructor
         */
        ParseExc(const std::string &cl, const std::string &par);

        /**
         * \brief Destructor
         */
        virtual ~ParseExc() throw () {}

    };
}
#endif // TRES_PARSEUTILS_HDR
