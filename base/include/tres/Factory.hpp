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
 * \file Factory.hpp
 *
 * \note This file is taken from the MetaSim distribution and adapted for
 * use with tres_base. The original authors are gratefully aknowledged for
 * sharing their code under the 3-clause BSD license.
 *
 * \authors Giuseppe Lipari (copyright holder), Cesare Bartolini
 */

#ifndef TRES_FACTORY_HDR
#define TRES_FACTORY_HDR
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

/**
 * \addtogroup tres_factories
 * @{
 */
/**
 * \brief Type of the product (class) ID key
 */
typedef std::string defaultIDKeyType;

/**
 * \brief The abstract factory itself
 *
 * Implemented using the Singleton pattern
 */
template <class manufacturedObj, typename classIDKey=defaultIDKeyType>
class Factory 
{
    public:

        /**
         * \brief A base function for object creation
         * 
         * A BASE_CREATE_FN is a function that takes a description of a specialized
         * implementation of a manufacturedObj as a vector of strings, and returns an
         * unique_ptr to a manufacturedObj.
         */
        typedef std::unique_ptr<manufacturedObj> (*BASE_CREATE_FN)(std::vector<std::string> &par);

        /**
         * \brief FN_REGISTRY is the registry of all the BASE_CREATE_FN
         * pointers registere
         *
         * Functions are registered using the regCreateFn member function (see below).
         */
        typedef std::map<classIDKey, BASE_CREATE_FN> FN_REGISTRY;

        /**
         * \brief Singleton access
         */
        static Factory &instance();

        /**
         * \brief Function registration
         *
         * Classes derived from manufacturedObj call this function once
         * per program to register the class ID key, and a pointer to
         * the function that creates the class
         */
        void regCreateFn(const classIDKey &, BASE_CREATE_FN);

        /**
         * \brief Create a new class of the type specified by className
         */
        std::unique_ptr<manufacturedObj> create(const classIDKey &className, std::vector<std::string> &parms) const;

    private:

        /**
         * \brief Private constructor (Singleton implementation)
         */
        Factory();

        /**
         * \brief Private copying (Singleton implementation)
         *
         * No implementation on the copying.
         */
        Factory(const Factory&);

        /**
         * \brief Private copying (Singleton implementation)
         *
         * Assignment operator is also not implemented
         */
        Factory &operator=(const Factory&);

    private:

        /**
         * \brief The actual FN_REGISTRY instance
         */
        FN_REGISTRY registry;
};

/**
 * \brief Helper template to make registration painless and simple
 */
template <class ancestorType,
          class manufacturedObj,
          typename classIDKey=defaultIDKeyType>
class registerInFactory
{
    public:
        static std::unique_ptr<ancestorType> createInstance(std::vector<std::string> &par)
        {
            return std::unique_ptr<ancestorType>(manufacturedObj::createInstance(par));
        }
        registerInFactory(const classIDKey &id)
        {
            Factory<ancestorType>::instance().regCreateFn(id, createInstance);
        }
};

/**
 * \brief Helper macro to get the (singleton) instance of the factory
 */
#define FACT(xxx) Factory<xxx>::instance()
/** @} */

template <class manufacturedObj, typename classIDKey>
Factory<manufacturedObj, classIDKey>::Factory()
{
}

template <class manufacturedObj, typename classIDKey>
Factory<manufacturedObj, classIDKey> &Factory<manufacturedObj, classIDKey>::instance()
{
    // Note that this is not thread-safe!
    static Factory theInstance;
    return theInstance;
}

// Register the creation function.  This simply associates the classIDKey
// with the function used to create the class.  The return value is a dummy
// value, which is used to allow static initialization of the registry.
template <class manufacturedObj, typename classIDKey>
void Factory<manufacturedObj, classIDKey>::regCreateFn(const classIDKey &clName, BASE_CREATE_FN func)
{
    registry[clName]=func;
}

// The create function simple looks up the class ID, and if it's in the list,
// the statement "(*i).second();" calls the function.
template <class manufacturedObj, typename classIDKey>
std::unique_ptr<manufacturedObj> Factory<manufacturedObj, classIDKey>::create(const classIDKey &className, std::vector<std::string> &parms) const
{
    std::unique_ptr<manufacturedObj> ret(nullptr);
    typename FN_REGISTRY::const_iterator regEntry=registry.find(className);
    if (regEntry != registry.end())
        return (*regEntry).second(parms);
    return ret;
}
#endif
