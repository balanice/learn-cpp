#include "curl_test2.h"

#include <curl/curl.h>
#include <spdlog/spdlog.h>

#define MAX_PARALLEL 10

static size_t write_cb(char *data, size_t n, size_t l, void *userp)
{
    /* take care of the data here, ignored in this example */
    (void)data;
    (void)userp;
    return n * l;
}

void test2()
{
    CURLM *cm;
    CURLMsg *msg;
    unsigned int transfers = 0;
    int msgs_left = -1;
    int left = 1;

    curl_global_init(CURL_GLOBAL_ALL);
    cm = curl_multi_init();

    /* Limit the amount of simultaneous connections curl should allow: */
    curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, (long)MAX_PARALLEL);

    // add transfer
    CURL *eh = curl_easy_init();
    curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(eh, CURLOPT_URL, "http://www.baidu.com");
    curl_easy_setopt(eh, CURLOPT_PRIVATE, "http://www.baidu.com");
    curl_multi_add_handle(cm, eh);

    spdlog::info("test2");
    do
    {
        int still_alive = 1;
        curl_multi_perform(cm, &still_alive);

        /* !checksrc! disable EQUALSNULL 1 */
        while ((msg = curl_multi_info_read(cm, &msgs_left)) != NULL)
        {
            if (msg->msg == CURLMSG_DONE)
            {
                char *url;
                CURL *e = msg->easy_handle;
                curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);

                spdlog::info("R: {}, {}, {}", static_cast<int>(msg->data.result), curl_easy_strerror(msg->data.result),
                             url);

                // fprintf(stderr, "R: %d - %s <%s>\n",
                // msg->data.result, curl_easy_strerror(msg->data.result), url);
                curl_multi_remove_handle(cm, e);
                curl_easy_cleanup(e);
                left--;
            }
            else
            {
                spdlog::error("E: CURLMsg ");
            }
        }
        if (left)
            curl_multi_wait(cm, NULL, 0, 1000, NULL);

    } while (left);

    curl_multi_cleanup(cm);
    curl_global_cleanup();
}