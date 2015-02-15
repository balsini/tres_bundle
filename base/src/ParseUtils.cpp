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
 * \file ParseUtils.cpp
 */
 
#include <cstdlib>
#include <tres/ParseUtils.hpp>

namespace tres_parse_utils
{
    using namespace std;

    //removes the spaces at the beginning and at the end of the string
    string remove_spaces(const string &tk)
    {
	    string temp = tk;
	    string::size_type pos = 0;
	    if (tk == "")
	        return temp;
	    // remove spaces at the beginning...
	    temp.erase(0, temp.find_first_not_of(' '));
	    pos = temp.find_last_not_of(' ');
	    temp.erase(pos+1, temp.size() - pos - 1);
	    return temp;
    }

    vector<string> split(const string &code, const string &sep, 
			 const string &open_par, 
			 const string &close_par)
    {
	    vector<string> temp;
	    string::size_type pos = 0;
	    string::size_type old_pos = 0;
	    int count = 0;
	    while (pos != string::npos)
        {
	        pos = code.find(sep, old_pos);
	        if (pos != string::npos)
	        { 
		        temp.push_back(remove_spaces(code.substr(old_pos,pos-old_pos)));
		        old_pos = pos + sep.size();
		        count ++;
	        }
	        else
    		    temp.push_back(remove_spaces(code.substr(old_pos, code.size() - old_pos)));
	    }
	    return temp;
    }

    vector<string> split_instr(const string &code)
    {
	    vector<string> temp;
	    string::size_type pos = 0;
	    string::size_type old_pos = 0;
	    while (pos != string::npos)
	    {
	        pos = code.find(';', old_pos);
	        if (pos != string::npos)
	        { 
		        temp.push_back(remove_spaces(code.substr(old_pos,pos-old_pos)));
		        old_pos = ++pos;
	        }
	    }
	    return temp;
    }

    string get_token(const string &instr, const string &open_par)
    {
	    string temp;
	    string::size_type pos = instr.find(open_par);
	    string::size_type pos1 = instr.find_first_not_of(" \n");
	    temp = instr.substr(pos1, pos - pos1);
	    return temp;
    }

    string get_param(const string &instr, const string &open_par,
		     const string &close_par)
    {
	    string temp("");
	    string::size_type pos = instr.find(open_par);
	    if (pos != string::npos)
	    {
	        string::size_type end = instr.find_last_of(close_par);
	        temp = instr.substr(pos+1, end-pos-1);
	    }
	    return temp;
    }

    vector<string> split_param(const string &p, const string &sep,
			       char open_par, char close_par)
    {
	    vector<string> temp;
	    string::size_type pos = 0;
	    string::size_type old_pos = 0;
	    string symbols = sep + open_par;
	    while (pos <= p.size())
	    {
	        pos = p.find_first_of(symbols, pos);
	        if (pos != string::npos)
		        if (p[pos] == open_par)
		        {
		            pos = p.find(close_par, pos);
		            if (pos == p.size())
		                pos = string::npos;
		            else
		                ++pos;
		        }
            if (pos != string::npos)
	        {
		        string t = remove_spaces(p.substr(old_pos, pos - old_pos));
		        temp.push_back(t);
		        old_pos = ++pos;
            }
	    }
	    if (pos != old_pos)
	    {
	        string t = remove_spaces(p.substr(old_pos, p.size() - old_pos));
	        if (t != "")
	            temp.push_back(t);
	    }
	    return temp;
    }

    void parse_double(const string &nums, double &res, string &unit)
    {
	    string tmp = remove_spaces(nums);
	    string::size_type pos = 0;
	    string symb = "smnu";
	    pos = tmp.find_first_of(symb, pos);
	    if (pos != string::npos) 
	        unit = tmp.substr(pos, tmp.size() - pos);
	    else
	        unit = "";  
	    string snum = tmp.substr(0,pos);
	    res = atof(snum.c_str());
    }

    ParseExc::ParseExc(const string &cl, const string &par)
        : tres::BaseExc("Parse error: in " + cl + " param(s) " + par + " is unknown")
    {
    }
}
