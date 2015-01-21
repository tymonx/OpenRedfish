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

#define JSON_MIME       "application/json"
#define JSON_TEST       "{\"Test\": \"simple\"}"

static int request_get(void *cls, struct MHD_Connection *connection,
        const char *url, const char *method, const char *version,
        const char *upload_data, size_t *upload_data_size, void **con_cls) {

    (void)cls;
    (void)url;
    (void)method;
    (void)version;
    (void)upload_data;

    struct MHD_Response *response;
    int ret;

    if (0 != *upload_data_size) {
        /* Upload data in a GET!? */
        return MHD_NO;
    }
    /* Clear context pointer */
    *con_cls = NULL;

    /* Create response */
    response = MHD_create_response_from_data(strlen(JSON_TEST), JSON_TEST,
            MHD_NO, MHD_NO);
    if (NULL == response) {
        return MHD_NO;
    }

    /* Add to header Content-Type: application/json */
    MHD_add_response_header(response, "Content-Type", JSON_MIME);

    /* Response to client */
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

static int access_handler_callback(void *cls, struct MHD_Connection *connection,
    const char *url, const char *method, const char *version,
    const char *upload_data, size_t *upload_data_size, void **ptr) {

    if (0 == strcmp(method, "GET")) {
        return request_get(cls, connection, url, method, version,
            upload_data, upload_data_size, ptr);
    }

    return MHD_NO;
}

int main(int argc, char **argv) {

    struct MHD_Daemon *daemon;

    if (argc != 2) {
        printf("%s PORT\n", argv[0]);
        return 1;
    }

    daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, atoi(argv[1]),
            NULL, NULL, &access_handler_callback, NULL, MHD_OPTION_END);

    if (NULL == daemon) {
        return 1;
    }

    getchar();
    MHD_stop_daemon(daemon);

    return 0;
}
