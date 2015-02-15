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
 * \file RandExecSegment.hpp
 */

#ifndef TRES_RANDEXECSEGMENT_HDR
#define TRES_RANDEXECSEGMENT_HDR
#include <memory>
#include <vector>
#include "RandomVar.hpp"
#include "Segment.hpp"

namespace tres
{
    /**
     * \addtogroup tres_base_rtos
     * @{
     */
    /**
     * \brief Model a time-consuming computation taking a random time to execute
     *
     * Execution times are modeled by random variables that follow
     * a number of different probability distributions (uniform, exponential, etc.)
     */
    class RandExecSegment : public Segment
    {

    public:

        /**
         * \brief Constructor
         */
        RandExecSegment(unique_ptr<RandomVar>&);

        /**
         * \brief Constructor (use a raw pointer)
         */
        RandExecSegment(RandomVar*);

        /**
         * \brief Instance creator
         */
        static Segment* createInstance(std::vector<std::string>&);

        /**
         * \brief The virtual destructor
         */
        virtual ~RandExecSegment() = default;

        /**
         * Get the worst-case execution time for this instruction
         */
        virtual double getWCET() const;

        /**
         * Get the total computation time of the instruction 
         */
        virtual double getDuration() const;

    protected:

        /** RandomVar instance representing the instruction cost/duration */
        unique_ptr<RandomVar> cost;

    };
    /** @} */
}
#endif // TRES_RANDEXECSEGMENT_HDR
