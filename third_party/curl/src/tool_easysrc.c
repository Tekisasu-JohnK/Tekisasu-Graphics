/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2020, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
#include "tool_setup.h"

#include "slist_wc.h"

#ifndef CURL_DISABLE_LIBCURL_OPTION

#define ENABLE_CURLX_PRINTF
/* use our own printf() functions */
#include "curlx.h"

#include "tool_cfgable.h"
#include "tool_easysrc.h"
#include "tool_msgs.h"

#include "memdebug.h" /* keep this as LAST include */

/* global variable definitions, for easy-interface source code generation */

struct slist_wc *easysrc_decl = NULL; /* Variable declarations */
struct slist_wc *easysrc_data = NULL; /* Build slists, forms etc. */
struct slist_wc *easysrc_code = NULL; /* Setopt calls */
struct slist_wc *easysrc_toohard = NULL; /* Unconvertible setopt */
struct slist_wc *easysrc_clean = NULL;  /* Clean up allocated data */
int easysrc_mime_count = 0;
int easysrc_slist_count = 0;

static const char *const srchead[]={
  "/********* Sample code generated by the curl command line tool **********",
  " * All curl_easy_setopt() options are documented at:",
  " * https://curl.se/libcurl/c/curl_easy_setopt.html",
  " ************************************************************************/",
  "#include <curl/curl.h>",
  "",
  "int main(int argc, char *argv[])",
  "{",
  "  CURLcode ret;",
  "  CURL *hnd;",
  NULL
};
/* easysrc_decl declarations come here */
/* easysrc_data initialisations come here */
/* easysrc_code statements come here */
static const char *const srchard[]={
  "/* Here is a list of options the curl code used that cannot get generated",
  "   as source easily. You may select to either not use them or implement",
  "   them yourself.",
  "",
  NULL
};
static const char *const srcend[]={
  "",
  "  return (int)ret;",
  "}",
  "/**** End of sample code ****/",
  NULL
};

/* Clean up all source code if we run out of memory */
static void easysrc_free(void)
{
  slist_wc_free_all(easysrc_decl);
  easysrc_decl = NULL;
  slist_wc_free_all(easysrc_data);
  easysrc_data = NULL;
  slist_wc_free_all(easysrc_code);
  easysrc_code = NULL;
  slist_wc_free_all(easysrc_toohard);
  easysrc_toohard = NULL;
  slist_wc_free_all(easysrc_clean);
  easysrc_clean = NULL;
}

/* Add a source line to the main code or remarks */
CURLcode easysrc_add(struct slist_wc **plist, const char *line)
{
  CURLcode ret = CURLE_OK;
  struct slist_wc *list = slist_wc_append(*plist, line);
  if(!list) {
    easysrc_free();
    ret = CURLE_OUT_OF_MEMORY;
  }
  else
    *plist = list;
  return ret;
}

CURLcode easysrc_addf(struct slist_wc **plist, const char *fmt, ...)
{
  CURLcode ret;
  char *bufp;
  va_list ap;
  va_start(ap, fmt);
  bufp = curlx_mvaprintf(fmt, ap);
  va_end(ap);
  if(!bufp) {
    ret = CURLE_OUT_OF_MEMORY;
  }
  else {
    ret = easysrc_add(plist, bufp);
    curl_free(bufp);
  }
  return ret;
}

#define CHKRET(v) do {CURLcode ret = (v); if(ret) return ret;} while(0)

CURLcode easysrc_init(void)
{
  CHKRET(easysrc_add(&easysrc_code,
                     "hnd = curl_easy_init();"));
  return CURLE_OK;
}

CURLcode easysrc_perform(void)
{
  /* Note any setopt calls which we could not convert */
  if(easysrc_toohard) {
    int i;
    struct curl_slist *ptr;
    const char *c;
    CHKRET(easysrc_add(&easysrc_code, ""));
    /* Preamble comment */
    for(i = 0; ((c = srchard[i]) != NULL); i++)
      CHKRET(easysrc_add(&easysrc_code, c));
    /* Each unconverted option */
    if(easysrc_toohard) {
      for(ptr = easysrc_toohard->first; ptr; ptr = ptr->next)
        CHKRET(easysrc_add(&easysrc_code, ptr->data));
    }
    CHKRET(easysrc_add(&easysrc_code, ""));
    CHKRET(easysrc_add(&easysrc_code, "*/"));

    slist_wc_free_all(easysrc_toohard);
    easysrc_toohard = NULL;
  }

  CHKRET(easysrc_add(&easysrc_code, ""));
  CHKRET(easysrc_add(&easysrc_code, "ret = curl_easy_perform(hnd);"));
  CHKRET(easysrc_add(&easysrc_code, ""));

  return CURLE_OK;
}

CURLcode easysrc_cleanup(void)
{
  CHKRET(easysrc_add(&easysrc_code, "curl_easy_cleanup(hnd);"));
  CHKRET(easysrc_add(&easysrc_code, "hnd = NULL;"));

  return CURLE_OK;
}

void dumpeasysrc(struct GlobalConfig *config)
{
  struct curl_slist *ptr;
  char *o = config->libcurl;

  FILE *out;
  bool fopened = FALSE;
  if(strcmp(o, "-")) {
    out = fopen(o, FOPEN_WRITETEXT);
    fopened = TRUE;
  }
  else
    out = stdout;
  if(!out)
    warnf(config, "Failed to open %s to write libcurl code!\n", o);
  else {
    int i;
    const char *c;

    for(i = 0; ((c = srchead[i]) != NULL); i++)
      fprintf(out, "%s\n", c);

    /* Declare variables used for complex setopt values */
    if(easysrc_decl) {
      for(ptr = easysrc_decl->first; ptr; ptr = ptr->next)
        fprintf(out, "  %s\n", ptr->data);
    }

    /* Set up complex values for setopt calls */
    if(easysrc_data) {
      fprintf(out, "\n");

      for(ptr = easysrc_data->first; ptr; ptr = ptr->next)
        fprintf(out, "  %s\n", ptr->data);
    }

    fprintf(out, "\n");
    if(easysrc_code) {
      for(ptr = easysrc_code->first; ptr; ptr = ptr->next) {
        if(ptr->data[0]) {
          fprintf(out, "  %s\n", ptr->data);
        }
        else {
          fprintf(out, "\n");
        }
      }
    }

    if(easysrc_clean) {
      for(ptr = easysrc_clean->first; ptr; ptr = ptr->next)
        fprintf(out, "  %s\n", ptr->data);
    }

    for(i = 0; ((c = srcend[i]) != NULL); i++)
      fprintf(out, "%s\n", c);

    if(fopened)
      fclose(out);
  }

  easysrc_free();
}

#endif /* CURL_DISABLE_LIBCURL_OPTION */
