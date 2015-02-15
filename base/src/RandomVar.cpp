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
 * \file RandomVar.cpp
 */
 
#include <cmath>
#include <memory>
#include <cstdlib>
#include <tres/Factory.hpp>
#include <tres/ParseUtils.hpp>
#include "RandomVar.hpp"

#ifdef CEPHES_LIB
#include "cephes/cephes.h"
#endif

namespace tres {

    using namespace std;
    using namespace tres_parse_utils;

    RandomGen RandomVar::_stdgen(1);

    RandomGen* RandomVar::_pstdgen(&_stdgen);

    const RandNum RandomGen::A = 16807;
    const RandNum RandomGen::M = 2147483647;
    const RandNum RandomGen::Q = 127773; // M div A
    const RandNum RandomGen::R = 2836;   // M mod A


    const char * const RandomVar::Exc::_FILEOPEN = "Unable to open RandFile";
    const char * const RandomVar::Exc::_FILECLOSE = "Too short RandFile";
    const char * const RandomVar::Exc::_WRONGPDF = "Malformed PDF";

    RandomGen::RandomGen(RandNum s) : _seed(s), _xn(s)
    {
    }

    RandNum RandomGen::sample()
    {
        RandNum xq, xr;

        xq = _xn / Q;
        xr = _xn % Q;

        _xn = A * xr - R * xq;
        if (_xn < 0)
            _xn += M;
        return _xn;
    }

    void RandomGen::init(RandNum s)
    {
        _xn = _seed = s;
    }

    const unsigned long PoissonVar::CUTOFF = 10000;

    RandomVar::RandomVar(RandomGen* gen) : _gen(gen)
    {
        if (_gen == NULL) 
            _gen = _pstdgen;
    }

    RandomVar::RandomVar(const RandomVar &r) : _gen(r._gen)
    {
    }

    RandomVar::~RandomVar()
    {
    }

    RandomGen* RandomVar::changeGenerator(RandomGen *g)
    { 
        RandomGen *old = _pstdgen;
        _pstdgen = g; 
        return old;
    }

    void RandomVar::restoreGenerator()
    {
        _pstdgen = &_stdgen;
    }

    RandomVar* DeltaVar::createInstance(vector<string> &par) 
    {
        if (par.size() != 1) 
            throw ParseExc("Wrong number of parameters", "DeltaVar");
        double a = atof(par[0].c_str());
        return new DeltaVar(a);
    }

    double UniformVar::get()
    {
        double tmp;
        tmp = _gen->sample();
        tmp = tmp * (_max - _min) / _gen->getModule() + _min;
  
        return tmp;
    }

    RandomVar *UniformVar::createInstance(vector<string> &par) 
    {
        double a,b;

        if (par.size() != 2)
            throw ParseExc("Wrong number of parameters", "UniformVar");

        a = atof(par[0].c_str());
        b = atof(par[1].c_str());
        return new UniformVar(a,b);
    } 

    double ExponentialVar::get()
    {
        return -log(UniformVar::get()) * _lambda;
    }

    RandomVar *ExponentialVar::createInstance(vector<string> &par) 
    {
        if (par.size() != 1)
            throw ParseExc("Wrong number of parameters", "ExponentialVar");

        double a = atof(par[0].c_str());      
        return new ExponentialVar(a);
    }

    double ParetoVar::get()
    {
        return _mu * pow (UniformVar::get(), -1/_order);
    }

    RandomVar *ParetoVar::createInstance(vector<string> &par) 
    {
        double a,b;

        if (par.size() != 2) 
            throw ParseExc("Wrong number of parameters", "ParetoVar");

        a = atof(par[0].c_str());
        b = atof(par[1].c_str());
        return new ParetoVar(a,b);
    } 

#ifndef CEPHES_LIB
    double NormalVar::get() 
    {
        double t1,t2,r;
  
        if (_yes)
        {
            _yes = false;
            return _oldv;
        }
  
        do
        {
            t1 = 2 * UniformVar::get() - 1;
            t2 = 2 * UniformVar::get() - 1;
    
            r = t1*t1 + t2*t2;
        } while (r >= 1);
  
        r = sqrt(-2*log(r)/r) * _sigma;
        _oldv = _mu + t1 * r;
        _yes = 1;
  
        return _mu + t2 * r;
    }
#else
    double NormalVar::get()
    {
        return _mu + _sigma * ndtri(UniformVar::get());
    }
#endif

    RandomVar *NormalVar::createInstance(vector<string> &par) 
    {
        double a,b;

        if (par.size() != 2) 
            throw ParseExc("Wrong number of parameters", "NormalVar");

        a = atof(par[0].c_str());
        b = atof(par[1].c_str());
        return new NormalVar(a,b);
    } 

    double PoissonVar::get() 
    {
        double u = UniformVar::get();
        double F = exp(-_lambda);
        double S = F;

        for (register unsigned int i = 1; i < CUTOFF; ++i)
        {
            if (u < S) return i - 1;
            F = F * _lambda / double(i);
            S += F;
        }
        return CUTOFF;
    }

    RandomVar* PoissonVar::createInstance(vector<string> &par) 
    {
        double a;

        if (par.size() != 1)
            throw ParseExc("Wrong number of parameters", "PoissonVar");
        a = atof(par[0].c_str());
        return new PoissonVar(a);
    } 

    DetVar::DetVar(const std::string &filename) :
        _array()
    {
        ifstream inFile;
        double v;

        inFile.open(filename.c_str());
        if (!inFile.is_open())
        {
            string errMsg = Exc::_FILEOPEN  + string(filename) + "\n";
            throw Exc(errMsg, "DetVar");
        }
        while (!inFile.eof())
        {
            inFile >> v;
            _array.push_back(v);
        }
        inFile.close();
        _count = 0;
    }

    DetVar::DetVar(vector<double> &a) : _array(a) 
    {
        _count = 0;
    }

    DetVar::DetVar(double *a, int s)
    {
        for (int i = 0; i < s; ++i) 
            _array.push_back(a[i]);
        _count = 0;
    }

    double DetVar::get() 
    {
        if (_count >= _array.size())
            _count = 0;
        return _array[_count++];
    }

    double DetVar::getMaximum() throw(MaxException)
    {
        if (_array.empty()) return 0;
        double max = _array[0];
        for (unsigned int i = 1; i < _array.size(); i++)
            if (_array[i] > max) max = _array[i];
        return max;
    }

    double DetVar::getMinimum() throw(MaxException)
    {
        if (_array.empty()) return 0;
        double min = _array[0];
        for (unsigned int i = 1; i < _array.size(); i++)
            if (_array[i] < min) min = _array[i];
        return min;
    }

    RandomVar *DetVar::createInstance(vector<string> &par) 
    {
        if (par.size() != 1) 
            throw ParseExc("Wrong number of parameters", "DetVar");

        return new DetVar(par[0]);
    } 
    
    double DistVar::get()
    {
	    double p = UniformVar::get();
	    //for (auto i : _array) 
	    for (auto i = _array.begin(); i != _array.end();  ++i )
	        if (p <= i->second) return i->first;
	    throw string("cannot extract number in DistVar::get()!!");
    }

    double DistVar::getMaximum() throw(MaxException)
    {
    	return _array[_array.size()-1].first;
    }
    
    double DistVar::getMinimum() throw(MaxException)
    {
	    return _array[0].first;
    }

    RandomVar *DistVar::createInstance(vector<string> &par)
    {
	    //cout << "Creating instance of DistVar" << endl;
	    vector<Elem> ini;

	    //for (auto x : par) {
	    /*for (auto x = par.begin(); x != par.end();  ++x ) {
	        cout << *x << endl;
	    }*/

	    //for (auto x : par) {
	    for (auto x = par.begin(); x != par.end();  ++x )
	    {
	        //cout << *x << endl;
	        vector<string> l = split_param(*x, ":", '[', ']');
	        //cout << "size: " << l.size() << endl;
	        //cout << l[0] << endl;
	        if (l.size() != 2) 
		    throw string("Wrong num of param in DistVar::createInstance");
	        Elem e; 
	        e.first = atof(l[0].c_str());
	        e.second = atof(l[1].c_str());
	        ini.push_back(e);
	        //cout << e.first << "," << e.second << endl;
	    }
	    return new DistVar(ini.begin(), ini.end());
    }

    RandomVar *parsevar(const std::string &str)
    {
        RandomVar *temp;
        string token = get_token(str);
        string p = get_param(str);
        vector<string> parms = split_param(p);

        std::unique_ptr<RandomVar> 
            var(Factory<RandomVar>::instance().create(token,parms));

        if (var.get() == 0) throw ParseExc("parsevar", str);
        temp = var.release();
        return temp;
    }
}
