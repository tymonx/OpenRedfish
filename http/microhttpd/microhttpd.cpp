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

#include "microhttpd.hpp"

#include <string>
#include <cstring>

#ifdef LOG_HTTP
#include <iostream>
#define LOGUSR  std::cout

#define log_error(inst, stream)    do {(inst) << stream;} while(0)
#define log_debug(inst, stream)    do {(inst) << stream;} while(0)
#else
#define log_error(inst, stream)    do { } while(0)
#define log_debug(inst, stream)    do { } while(0)
#endif

static const char JSON_MIME[] = "application/json";

struct connection_info {
    std::string message;
};

static int request_post(void* cls, struct MHD_Connection* connection,
        const char* url, const char* method, const char* version,
        const char* upload_data, size_t* upload_data_size, void** con_cls) {

    (void)cls;
    (void)url;
    (void)method;
    (void)version;

    struct connection_info* con_info =
        static_cast<struct connection_info*>(*con_cls);

    if (NULL == con_info) {
        log_debug(LOGUSR, "Create connection info\n");
        con_info = new struct connection_info;
        if (NULL == con_info) {
            return MHD_NO;
        }
        *con_cls = con_info;
        return MHD_YES;
    }

    if (0 != *upload_data_size) {
        con_info->message.append(upload_data, *upload_data_size);
        *upload_data_size = 0;
        return MHD_YES;
    } else {
        log_debug(LOGUSR, "Data size:" << con_info->message.length() << "\n");
        log_debug(LOGUSR, "Data: " << con_info->message << "\n");

        Request message;
        message.append(con_info->message);
        static_cast<MicroHttpd*>(cls)->call(Method::POST, message);

        delete con_info;
        *con_cls = NULL;
    }

    struct MHD_Response* response;
    int ret;

    /* Create response */
    response = MHD_create_response_from_buffer(0, NULL,
            MHD_RESPMEM_PERSISTENT);

    if (NULL == response) {
        log_error(LOGUSR, "Cannot create response\n");
        return MHD_NO;
    }

    /* Response to client */
    ret = MHD_queue_response(connection, MHD_HTTP_NO_CONTENT,
            response);
    MHD_destroy_response(response);

    return ret;
}

static int request_get(void* cls, struct MHD_Connection* connection,
        const char* url, const char* method, const char* version,
        const char* upload_data, size_t* upload_data_size, void** con_cls) {

    (void)cls;
    (void)url;
    (void)method;
    (void)version;
    (void)upload_data;

    struct MHD_Response* response;
    int ret;

    if (0 != *upload_data_size) {
        /* Upload data in a GET!? */
        return MHD_NO;
    }
    *con_cls = NULL;

    Request message;
    static_cast<MicroHttpd*>(cls)->call(Method::GET, message);

    /* Create response */
    response = MHD_create_response_from_buffer(
            message.get_response().length(),
            const_cast<char*>(message.get_response().c_str()),
            MHD_RESPMEM_MUST_COPY);

    if (NULL == response) {
        log_error(LOGUSR, "Cannot create response\n");
        return MHD_NO;
    }

    /* Add to header Content-Type: application/json */
    MHD_add_response_header(response, "Content-Type", JSON_MIME);

    /* Response to client */
    ret = MHD_queue_response(connection, message.get_status(), response);
    MHD_destroy_response(response);

    return ret;
}

static int request_put(void* cls, struct MHD_Connection* connection,
        const char* url, const char* method, const char* version,
        const char* upload_data, size_t* upload_data_size, void** con_cls) {

    (void)cls;
    (void)url;
    (void)method;
    (void)version;
    (void)upload_data;
    (void)upload_data_size;

    struct MHD_Response* response;
    int ret;

    *con_cls = NULL;

    Request message;
    static_cast<MicroHttpd*>(cls)->call(Method::PUT, message);

    /* Create response */
    response = MHD_create_response_from_buffer(0, NULL,
            MHD_RESPMEM_PERSISTENT);

    if (NULL == response) {
        log_error(LOGUSR, "Cannot create response\n");
        return MHD_NO;
    }

    /* Response to client */
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

static int request_patch(void* cls, struct MHD_Connection* connection,
        const char* url, const char* method, const char* version,
        const char* upload_data, size_t* upload_data_size, void** con_cls) {

    (void)cls;
    (void)url;
    (void)method;
    (void)version;
    (void)upload_data;
    (void)upload_data_size;

    struct MHD_Response* response;
    int ret;

    *con_cls = NULL;

    Request message;
    static_cast<MicroHttpd*>(cls)->call(Method::PATCH, message);

    /* Create response */
    response = MHD_create_response_from_buffer(0, NULL,
            MHD_RESPMEM_PERSISTENT);

    if (NULL == response) {
        log_error(LOGUSR, "Cannot create response\n");
        return MHD_NO;
    }

    /* Response to client */
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

static int request_delete(void* cls, struct MHD_Connection* connection,
        const char* url, const char* method, const char* version,
        const char* upload_data, size_t* upload_data_size, void** con_cls) {

    (void)cls;
    (void)url;
    (void)method;
    (void)version;
    (void)upload_data;
    (void)upload_data_size;

    struct MHD_Response* response;
    int ret;

    *con_cls = NULL;

    Request message;
    static_cast<MicroHttpd*>(cls)->call(Method::DELETE, message);

    /* Create response */
    response = MHD_create_response_from_buffer(0, NULL,
            MHD_RESPMEM_PERSISTENT);

    if (NULL == response) {
        log_error(LOGUSR, "Cannot create response\n");
        return MHD_NO;
    }

    /* Response to client */
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

static int request_head(void* cls, struct MHD_Connection* connection,
        const char* url, const char* method, const char* version,
        const char* upload_data, size_t* upload_data_size, void** con_cls) {

    (void)cls;
    (void)url;
    (void)method;
    (void)version;
    (void)upload_data;
    (void)upload_data_size;

    struct MHD_Response* response;
    int ret;

    *con_cls = NULL;

    Request message;
    static_cast<MicroHttpd*>(cls)->call(Method::HEAD, message);

    /* Create response */
    response = MHD_create_response_from_buffer(0, NULL,
            MHD_RESPMEM_PERSISTENT);

    if (NULL == response) {
        log_error(LOGUSR, "Cannot create response\n");
        return MHD_NO;
    }

    /* Response to client */
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

static int print_key_value(void* cls, enum MHD_ValueKind kind,
        const char* key, const char* value) {
    (void)cls;
    (void)kind;
    (void)key;
    (void)value;

    log_debug(LOGUSR,
            "Kind: " << kind << ", " << key << ": " << value << "\n");
    return MHD_YES;
}

static int access_handler_callback(void* cls, struct MHD_Connection *connection,
    const char* url, const char* method, const char* version,
    const char* upload_data, size_t* upload_data_size, void** con_cls) {

    log_debug(LOGUSR, "Access handler callback\n");

    MHD_get_connection_values(connection, MHD_HEADER_KIND, &print_key_value, NULL);

    if (0 == strcmp(method, "POST")) {
        log_debug(LOGUSR, "Method POST\n");
        return request_post(cls, connection, url, method, version,
                upload_data, upload_data_size, con_cls);
    }

    if (0 == strcmp(method, "GET")) {
        log_debug(LOGUSR, "Method GET\n");
        return request_get(cls, connection, url, method, version,
                upload_data, upload_data_size, con_cls);
    }

    if (0 == strcmp(method, "PUT")) {
        log_debug(LOGUSR, "Method PUT\n");
        return request_put(cls, connection, url, method, version,
                upload_data, upload_data_size, con_cls);
    }

    if (0 == strcmp(method, "PATCH")) {
        log_debug(LOGUSR, "Method PATCH\n");
        return request_patch(cls, connection, url, method, version,
                upload_data, upload_data_size, con_cls);
    }

    if (0 == strcmp(method, "DELETE")) {
        log_debug(LOGUSR, "Method DELETE\n");
        return request_delete(cls, connection, url, method, version,
                upload_data, upload_data_size, con_cls);
    }

    if (0 == strcmp(method, "HEAD")) {
        log_debug(LOGUSR, "Method HEAD\n");
        return request_head(cls, connection, url, method, version,
                upload_data, upload_data_size, con_cls);
    }

    /* Create response */
    int ret;
    struct MHD_Response* response = MHD_create_response_from_buffer(0, NULL,
            MHD_RESPMEM_PERSISTENT);

    if (NULL == response) {
        log_error(LOGUSR, "Cannot create response\n");
        return MHD_NO;
    }

    log_error(LOGUSR, "Method not allowed\n");

    /* Response to client */
    ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
    MHD_destroy_response(response);

    return ret;
}

static void default_method_callback(Request&) { }

ServerAdapter* Server::create_server() {
    return new MicroHttpd;
}

MicroHttpd::MicroHttpd() : m_daemon(NULL) {
    for (size_t i = 0; i < Method::MAX; i++) {
        m_method_callback[i] = default_method_callback;
    }
}

MicroHttpd::~MicroHttpd() {
    if (NULL != m_daemon) {
        MHD_stop_daemon(m_daemon);
        m_daemon = NULL;
    }
}

void MicroHttpd::open(std::string url) {
    (void)url;

    log_debug(LOGUSR, "Start daemon\n");
    m_daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, 8888,
            NULL, NULL,
            access_handler_callback, this,
            MHD_OPTION_END);
}

void MicroHttpd::close() {
    if (NULL != m_daemon) {
        MHD_stop_daemon(m_daemon);
        m_daemon = NULL;
    }
}

void MicroHttpd::support(MethodCallback method_callback) {
    if (nullptr == method_callback) {
        method_callback = default_method_callback;
    }

    for (size_t i = 0; i < Method::MAX; i++) {
        m_method_callback[i] = method_callback;
    }
}

void MicroHttpd::support(const Method method, MethodCallback method_callback) {
    if (nullptr == method_callback) {
        method_callback = default_method_callback;
    }

    if (method < Method::MAX) {
        m_method_callback[method] = method_callback;
    }
}
