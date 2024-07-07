#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        // out of memory!
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int download_file(const char *url, const char *output_filename) {
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  // will be grown as needed by the realloc above
    chunk.size = 0;            // no data at this point

    curl_global_init(CURL_GLOBAL_ALL);

    // init the curl session
    curl_handle = curl_easy_init();

    // load URL
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    // 'chunk' struct to the callback function
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    // Add user-agent string
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    // Instantiate request
    res = curl_easy_perform(curl_handle);

    // error handler
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return 1;
    } else {
        // open file for writing
        FILE *file = fopen(output_filename, "wb");
        if(!file) {
            fprintf(stderr, "Could not open %s for writing\n", output_filename);
            return 1;
        }
        fwrite(chunk.memory, 1, chunk.size, file);
        fclose(file);
    }

    // cleanup
    curl_easy_cleanup(curl_handle);
    free(chunk.memory);
    curl_global_cleanup();

    return 0;
}

int main(void) {
    const char *urls[] = {
        "http://standards-oui.ieee.org/oui/oui.csv",
        "http://standards-oui.ieee.org/cid/cid.csv",
        "http://standards-oui.ieee.org/iab/iab.csv",
        "http://standards-oui.ieee.org/oui28/mam.csv",
        "http://standards-oui.ieee.org/oui36/oui36.csv"
    };
    const char *filenames[] = {
        "oui.csv",
        "cid.csv",
        "iab.csv",
        "mam.csv",
        "oui36.csv"
    };

    int num_files = sizeof(urls) / sizeof(urls[0]);

    for(int i = 0; i < num_files; i++) {
        printf("Downloading %s...\n", urls[i]);
        if(download_file(urls[i], filenames[i]) != 0) {
            fprintf(stderr, "Failed to download %s\n", urls[i]);
            return 1;
        }
    }

    printf("All files downloaded successfully.\n");
    return 0;
}

