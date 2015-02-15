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
 * \file BaseExc.hpp
 *
 * \note This file is taken from the MetaSim distribution and adapted for
 * use with tres_base. The original authors are gratefully aknowledged for
 * sharing their code under the 3-clause BSD license.
 *
 * \authors Antonino Casile, Luigi Palopoli, Giuseppe Lipari (copyright holder)
 */

#ifndef TRES_BASEEXC_HDR
#define TRES_BASEEXC_HDR
#include <exception>
#include <string>

namespace tres
{

    /**
     * \addtogroup tres_utils
     * @{
     */
    /**
     * \brief Basic exception class.
     *
     * This class is the base used for handling exceptions. Any
     * exception of the simulator has to be derived from this class.
     *
     */
    class BaseExc : public std::exception {

    public:

        /**
         * \brief Constructor
         * \param[in] message contains the error message
         * \param[in] cl contains the name of the class where the exception
         * has been raised
         * \param[in] md contains the name of the module where the exception
         * has been raised. 
         */
        BaseExc(const std::string &message,
                const std::string &cl="unknown",
                const std::string &md="unknown") :
            _what("Class=" + cl + 
                  " Module=" + md + 
                  " Message:" + message)
            {}

        /**
         * \brief Returns the error string
         *
         * This is the standard diagnostic behavior, since it is
         * virtual it can be overidden
         */
        virtual const char* what() const throw()
        {
            return _what.c_str();    
        }

        /**
         * \brief Destructor
         */
        virtual ~BaseExc() throw() {}

    protected:

        /** Contains a brief description of the exception */
        std::string _what;
    };


/**
 * \brief Helper macro for the declaration of specialized exceptions
 */
#define DECL_EXC(EXC, CLASS) \
    class EXC : public BaseExc { public: \
            EXC(const std::string &m) : BaseExc(m, CLASS, __FILE__) {} }

/**
 * \brief Helper macro for throwing exceptions
 */
#define THROW_EXC(EXC, MSG) throw EXC(MSG  ":" __LINE__)
    /** @} */
}
#endif // TRES_BASEEXC_HDR
