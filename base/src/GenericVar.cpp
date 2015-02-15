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
 * \file GenericVar.cpp
 */

#include <cmath>
#include <tres/ParseUtils.hpp>
#include "GenericVar.hpp"

namespace tres
{
    using namespace std;
    using namespace tres_parse_utils;

    static const double PDF_ERR = 0.00000000001;

    void GenericVar::readPDF(ifstream &f, int mode)
    {
        int n;
        double p;
        double sum;

        sum = 0;
        while(!f.eof())
        {
            f >> n;
            f >> p;
            cout << n << "\t" << p << "\n";
            if (!f.eof())
            {
                if (_pdf[n] != 0)
                {
                    string errMsg = Exc::_WRONGPDF + string("\n");
                    throw Exc(errMsg, "GenericVar");
                }
                sum += p;
                _pdf[n] = p;
            }
        }

        if (sum > 1)
        {
            string errMsg = Exc::_WRONGPDF + string("\n");
            throw Exc(errMsg, "GenericVar");
        }

        if (sum < (1.0 - PDF_ERR))
        {
            cerr << "Warning: PDF values sum to " << sum << " < 1\n";
            if (mode == 0)
                _pdf[n] += (1 - sum);
            else
                _pdf[1] += (1 - sum);
        }
    }    

    GenericVar::GenericVar(const std::string &fileName) : 
        UniformVar(0, 1, NULL)
    {
        ifstream inFile(fileName.c_str());

        if (!inFile.is_open())
        {
            string errMsg = Exc::_FILEOPEN  + string(fileName) + "\n";
            throw Exc(errMsg, "GenericVar");
        }

        readPDF(inFile);
    }

    double GenericVar::get()
    {
        double v, CDF;
        map<int, double>::iterator i;

        v = UniformVar::get();
        CDF = 0;

        for(i = _pdf.begin(); i != _pdf.end(); i++)
        {
            CDF = CDF + i->second;
            if (CDF > v)
                return i->first;
        }

        /* We cannot arrive here... */
        /* It means that v > 1 or that lim_{i -> \infty} CDF < 1... */
        throw Exc("Panic", "GenericVar");
    }

    RandomVar *GenericVar::createInstance(vector<string> &par)
    {
        if (par.size() != 1) 
            throw ParseExc("Wrong number of parameters", "GenericVar");
        return new GenericVar(par[0]);
    }
}
