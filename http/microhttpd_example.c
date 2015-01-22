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

#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define GET                 0
#define POST                1

#define JSON_MIME           "application/json"
#define JSON_TEST           "{\"Test\": \"simple\"}"

struct connection_info {
    char* message;
    size_t message_length;
};

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
    /* Clear context pointer */
    free(*con_cls);
    *con_cls = NULL;

    /* Create response */
    response = MHD_create_response_from_data(strlen(JSON_TEST), JSON_TEST,
            MHD_NO, MHD_NO);
    if (NULL == response) {
        fprintf(stderr, "Cannot create response\n");
        return MHD_NO;
    }

    /* Add to header Content-Type: application/json */
    MHD_add_response_header(response, "Content-Type", JSON_MIME);

    /* Response to client */
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

static int request_post(void* cls, struct MHD_Connection* connection,
        const char* url, const char* method, const char* version,
        const char* upload_data, size_t* upload_data_size, void** con_cls) {

    (void)cls;
    (void)url;
    (void)method;
    (void)version;

    struct connection_info* con_info = *con_cls;

    if (NULL == *con_cls) {
        printf("Create connection info\n");
        con_info = malloc(sizeof(struct connection_info));
        if (NULL == con_info) {
            fprintf(stderr, "Cannot create connection info\n");
            return MHD_NO;
        }
        memset(con_info, 0, sizeof(struct connection_info));
        *con_cls = con_info;
        return MHD_YES;
    }

    if (0 != *upload_data_size) {
        size_t message_length = *upload_data_size + con_info->message_length;
        char* message = malloc(message_length + 1);
        if (NULL == message) {
            fprintf(stderr, "Cannot create message buffer\n");
            free(*con_cls);
            *con_cls = NULL;
            return MHD_NO;
        }

        /* Copy upload data to new section in buffer. Extent string */
        memcpy(&message[con_info->message_length], upload_data,
                *upload_data_size);
        message[message_length] = '\0';
        /* Copy old message before new message to keep data continuous */
        memcpy(message, con_info->message, con_info->message_length);
        /* Update message length */
        con_info->message_length = message_length;
        /* Remove old message and add new */
        free(con_info->message);
        con_info->message = message;

        /* We upload all data */
        *upload_data_size = 0;
        return MHD_YES;
    } else {
        printf("Data size: %lu\n", con_info->message_length);
        printf("Data: %s\n", con_info->message);

        free(con_info->message);

        /* Destroy context */
        free(*con_cls);
        *con_cls = NULL;
    }

    struct MHD_Response* response;
    int ret;

    /* Create response */
    response = MHD_create_response_from_data(0, NULL,
            MHD_NO, MHD_NO);

    if (NULL == response) {
        fprintf(stderr, "Cannot create response\n");
        return MHD_NO;
    }

    /* Response to client */
    ret = MHD_queue_response(connection, MHD_HTTP_NO_CONTENT,
            response);
    MHD_destroy_response(response);

    return ret;
}

static int print_key_value(void* cls, enum MHD_ValueKind kind,
        const char* key, const char* value) {
    (void)cls;

    printf("Kind: %d, %s: %s\n", kind, key, value);
    return MHD_YES;
}

static int access_handler_callback(void* cls, struct MHD_Connection *connection,
    const char* url, const char* method, const char* version,
    const char* upload_data, size_t* upload_data_size, void** con_cls) {

    printf("Access handler callback\n");

    MHD_get_connection_values(connection, MHD_HEADER_KIND, &print_key_value, NULL);

    if (0 == strcmp(method, "POST")) {
        printf("Method POST\n");
        return request_post(cls, connection, url, method, version,
                upload_data, upload_data_size, con_cls);
    }

    if (0 == strcmp(method, "GET")) {
        printf("Method GET\n");
        return request_get(cls, connection, url, method, version,
                upload_data, upload_data_size, con_cls);
    }

    if (0 == strcmp(method, "PUT")) {
        printf("Method PUT\n");
        return request_post(cls, connection, url, method, version,
                upload_data, upload_data_size, con_cls);
    }

    if (0 == strcmp(method, "PATCH")) {
        printf("Method PATCH\n");
        return request_post(cls, connection, url, method, version,
                upload_data, upload_data_size, con_cls);
    }

    if (0 == strcmp(method, "DELETE")) {
        printf("Method DELETE\n");
        return request_post(cls, connection, url, method, version,
                upload_data, upload_data_size, con_cls);
    }

    if (0 == strcmp(method, "HEAD")) {
        printf("Method HEAD\n");
        return request_post(cls, connection, url, method, version,
                upload_data, upload_data_size, con_cls);
    }

    /* Create response */
    int ret;
    struct MHD_Response* response = MHD_create_response_from_buffer(0, NULL,
            MHD_RESPMEM_PERSISTENT);

    if (NULL == response) {
        fprintf(stderr, "Cannot create response\n");
        return MHD_NO;
    }

    fprintf(stderr, "Method not allowed\n");

    /* Response to client */
    ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
    MHD_destroy_response(response);

    return ret;
}

int main(int argc, char* argv[]) {

    struct MHD_Daemon* daemon;

    if (argc != 2) {
        printf("%s PORT\n", argv[0]);
        return 1;
    }

    printf("Start daemon\n");
    daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, atoi(argv[1]),
            NULL, NULL,
            access_handler_callback, NULL,
            MHD_OPTION_END);

    if (NULL == daemon) {
        fprintf(stderr, "Error to start daemon!\n");
        return 1;
    }

    getchar();
    MHD_stop_daemon(daemon);

    return 0;
}
