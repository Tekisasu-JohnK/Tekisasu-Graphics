/* Tekisasu Graphics
   Copyright (C) 2021-present Tekisasu
   Copyright (C) 2020-2024  Igara Studio S.A.

   This program adhered to the terms of the End-User License Agreement for Aseprite. 
   No distribution of this program is permitted outside of Tekisasu.   
*/

#include "ver/info.h"
#include "generated_version.h"  /* It defines the VERSION macro */

#define PACKAGE                 "Tekisasu Graphics"
#define COPYRIGHT               "Copyright (C) 2021-present Tekisasu, Copyright (C) 2001-2024 Igara Studio S.A."

#if defined(_WIN32) || defined(__APPLE__)
  #define HTTP                  "https"
#else
  #define HTTP                  "http"
#endif

#ifdef CUSTOM_WEBSITE_URL
  #define WEBSITE               CUSTOM_WEBSITE_URL /* To test web server */
#else
  #define WEBSITE               HTTP "://www.aseprite.org/"
#endif
#define WEBSITE_DOWNLOAD        WEBSITE "download/"
#define WEBSITE_CONTRIBUTORS    WEBSITE "contributors/"
#define WEBSITE_NEWS_RSS        HTTP "://dev.tekisasu.com/aseprite-rss/rss"
#define WEBSITE_UPDATE          WEBSITE "update/?xml=1"

const char* get_app_name() { return PACKAGE; }
const char* get_app_version() { return VERSION; }
const char* get_app_copyright() { return COPYRIGHT; }

const char* get_app_url() { return WEBSITE; }
const char* get_app_download_url() { return WEBSITE_DOWNLOAD; }
const char* get_app_contributors_url() { return WEBSITE_CONTRIBUTORS; }
const char* get_app_news_rss_url() { return WEBSITE_NEWS_RSS; }
const char* get_app_update_url() { return WEBSITE_UPDATE; }
