/*!
 * @copyright Copyright (c) 2015, Tymoteusz Blazejczyk
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of OpenRedfish nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _JSON_HPP_
#define _JSON_HPP_

#include <map>
#include <string>
#include <vector>
#include <sstream>

namespace json {

class Json {
protected:
    std::stringstream stream;
};

class Array;
class Object;

class Number : Json {
private:
    enum NumberType {
        UINT32,
        SINT32,
        UINT64,
        SINT64,
        DOUBLE
    };

    union NumberContainer {
        std::uint32_t uint32;
        std::int32_t  sint32;
        std::uint64_t uint64;
        std::int64_t  sint64;
        double frac;
    };
};

class Value : Json {
private:
    enum ValueType {
        STRING,
        NUMBER,
        OBJECT,
        ARRAY,
        BOOLEAN,
        EMPTY
    };

    union ValueContainer {
        std::string str;
        Number number;
        Object* object;
        Array* array;
        bool boolean;
    };
};

class Array : Json {
private:
    std::vector<Value> m_values;
};

class Object : Json {
private:
    std::map<std::string, Value> m_objects;
public:
    inline size_t size() const {return m_objects.size();}
    inline Value& operator[](std::string key) {return m_objects[key];}
    //inline void insert(std::string key, Value value) {m_objects[key] = value;}
};

} /* namespace json */

#endif /* _JSON_HPP_ */
