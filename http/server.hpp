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

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <string>
#include <type_traits>

namespace OpenRedfish {
namespace http {

using std::string;

class Server {
public:
    class Request;
    class Response;

    typedef void (*MethodCallback)(const Request&, Response&);

    enum Method : unsigned int {
        POST    = 0,
        GET     = 1,
        PUT     = 2,
        PATCH   = 3,
        DELETE  = 4,
        HEAD    = 5
    };

    Server(const string& url);
    virtual void open() = 0;
    virtual void close() = 0;
    void support(MethodCallback callback);
    void support(const Method method, MethodCallback callback);
    void call(const Method method, const Request& request, Response& response);
    inline const string& get_url() const { return m_url; }
    virtual ~Server();

    class Request {
    public:
        Request(const string& url, const string& message);
        inline const string& get_url() const { return m_url; }
        inline const string& get_message() const { return m_message; }
    private:
        const string m_url;
        const string m_message;
    };

    class Response {
    public:
        Response();
        void set_reply(const unsigned int status, const string& message = "");
        inline unsigned int get_status() const { return m_status; }
        inline const string& get_message() const { return m_message; }
    private:
        unsigned int m_status;
        string m_message;
    };
protected:
    string m_url;
private:
    static constexpr unsigned int MAX_METHODS = 6;
    MethodCallback m_method_callback[MAX_METHODS];
};

} /* namespace http */
} /* namespace OpenRedfish */

#endif /* _SERVER_HPP_ */
