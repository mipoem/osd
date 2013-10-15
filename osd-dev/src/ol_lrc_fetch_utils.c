/* -*- mode: C; c-basic-offset: 2; indent-tabs-mode: nil; -*- */
/*
 * Copyright (C) 2009-2011  Tiger Soldier <tigersoldi@gmail.com>
 *
 * This file is part of OSD Lyrics.
 * 
 * OSD Lyrics is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OSD Lyrics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OSD Lyrics.  If not, see <http://www.gnu.org/licenses/>. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include "ol_lrc_fetch_utils.h"
#include "ol_music_info.h"
#include "ol_lrc_fetch.h"
#include "ol_utils.h"
#include "ol_utils_network.h"
#include "config.h"
#include "ol_debug.h"

static long cntimeout = 6;
static char errbuf[CURL_ERROR_SIZE];
const int RANK_SCALE = 100000;
static gboolean _set_curl_proxy (CURL *curl_handler);

size_t
convert(const char *from_charset,
        const char *to_charset,
        ICONV_CONST char *src,
        size_t srclen,
        char *dest,
        size_t destlen)
{
  ol_log_func ();
  size_t ret;
  iconv_t cv;
  ICONV_CONST char **input = &src;
  char **output = &dest;
  memset(dest, 0, destlen);
  size_t destleft = destlen;

  if((cv = iconv_open(to_charset, from_charset)) == (iconv_t)-1) {
    ol_errorf ("  the conversion from %s to %s is not supported by the implementation.\n", from_charset, to_charset);
    return (size_t)-1;
  }

  ret = iconv(cv, input, &srclen, output, &destleft);
  iconv_close(cv);
  if (ret == (size_t)-1)
    return ret;
  else
    return destlen - destleft;
}

static gboolean
_set_curl_proxy (CURL *curl_handler)
{
  ol_assert_ret (curl_handler != NULL, FALSE);
  gboolean ret = TRUE;
  enum OlProxyType type = ol_get_proxy_type ();
  switch (type)
  {
  case OL_PROXY_NONE:
    if (curl_easy_setopt (curl_handler, CURLOPT_PROXY, "") != CURLE_OK)
    {
      ol_errorf ("Failed to disable proxy\n");
      ret = FALSE;
    }
    break;
  case OL_PROXY_ENVAR:
    /* TODO */
    break;
  case OL_PROXY_HTTP:
    if (curl_easy_setopt (curl_handler, CURLOPT_PROXYTYPE,
                          CURLPROXY_HTTP_1_0) != CURLE_OK)
    {
      ol_errorf ("Failed to set proxy type\n");
      ret = FALSE;
    }
    break;
  case OL_PROXY_SOCKS4:
    if (curl_easy_setopt (curl_handler, CURLOPT_PROXYTYPE,
                          CURLPROXY_SOCKS4) != CURLE_OK)
    {
      ol_errorf ("Failed to set proxy type\n");
      ret = FALSE;
    }
    break;
  case OL_PROXY_SOCKS5:
    if (curl_easy_setopt (curl_handler, CURLOPT_PROXYTYPE,
                          CURLPROXY_SOCKS5) != CURLE_OK)
    {
      ol_errorf ("Failed to set proxy type\n");
      ret = FALSE;
    }
    break;
  default:
    ol_errorf ("Unknown proxy type: %d\n", (int) type);
  }
  if (ret && type != OL_PROXY_NONE && type != OL_PROXY_ENVAR)
  {
    char *host = ol_get_proxy_host ();
    int port = ol_get_proxy_port ();
    char *username = ol_get_proxy_username ();
    char *password = ol_get_proxy_password ();
    if (!ol_is_string_empty (host) &&
        1 < port && port < (1 << 16))
    {
      if (curl_easy_setopt (curl_handler, CURLOPT_PROXY,
                            host) != CURLE_OK)
      {
        ol_error ("Failed to set curl proxy host");
        goto error;
      }
      if (curl_easy_setopt (curl_handler, CURLOPT_PROXYPORT,
                            port) != CURLE_OK)
      {
        ol_error ("Failed to set curl proxy port");
        goto error;
      }
      if (!ol_is_string_empty (username))
      {
        if (curl_easy_setopt (curl_handler, CURLOPT_PROXYUSERNAME,
                              username) != CURLE_OK)
        {
          ol_error ("Failed to set curl proxy username");
          goto error;
        }
        if (!ol_is_string_empty (password))
          if (curl_easy_setopt (curl_handler, CURLOPT_PROXYPASSWORD,
                                password) != CURLE_OK)
          {
            ol_error ("Failed to set curl proxy password");
            goto error;
          }
      }
    }
    goto finish;
  error:
    ret = FALSE;
  finish:
    if (host != NULL) g_free (host);
    if (username != NULL) g_free (username);
    if (password != NULL) g_free (password);
  }
  return ret;
}

static CURL *
my_curl_init(CURL *curl,
             const char *url,
             const char *refer,
             const char *user_agent,
             const char *post_data,
             size_t post_len,
             WriteCallback func,
             void *data,
             long connecttimeout)
{
  ol_log_func ();
  CURLcode code;
  CURL *curl_handler;
  if(curl_global_init(CURL_GLOBAL_ALL) != 0) {
    ol_errorf ("curl_global_init error.\n");
    return NULL;
  }

  if(curl != NULL) {
    curl_handler = curl;
    /*
     * reset all options of a libcurl session handle
     * this puts back the handle to the same state as it was in when it was just created with curl_easy_init
     */
    curl_easy_reset(curl_handler);   
  }
  else
    curl_handler = curl_easy_init();

  if(curl_handler == NULL) {
    ol_errorf ("failed to create CURL easy session handler\n");
    return NULL;
  }

  code = curl_easy_setopt(curl_handler, CURLOPT_ERRORBUFFER, errbuf);
  if(code != CURLE_OK) {
    ol_errorf ("failed to set error buffer [%d]\n", code);
    return NULL;
  }

  code = curl_easy_setopt(curl_handler, CURLOPT_URL, url);
  if(code != CURLE_OK) {
    ol_errorf("failed to set URL [%s]\n", errbuf);
    return NULL;
  }
  if (refer != NULL)
  {
    code = curl_easy_setopt (curl_handler, CURLOPT_REFERER, refer);
    if (code != CURLE_OK)
    {
      ol_errorf("failed to set refer url [%s]\n", errbuf);
      return NULL;
    }
  }
  if (post_data != NULL && post_len > 0)
  {
    code = curl_easy_setopt (curl_handler, CURLOPT_POSTFIELDS, post_data);
    if (code != CURLE_OK)
    {
      ol_errorf("failed to set post field [%s]\n", errbuf);
      return NULL;
    }
    code = curl_easy_setopt (curl_handler, CURLOPT_POSTFIELDSIZE, post_len);
    if (code != CURLE_OK)
    {
      ol_errorf("failed to set post field size  [%s]\n", errbuf);
      return NULL;
    }
  }
  if (user_agent != NULL)
  {
    code = curl_easy_setopt(curl_handler, CURLOPT_USERAGENT, user_agent);
    if (code != CURLE_OK)
    {
      ol_errorf("failed to set user agent [%s]\n", errbuf);
      return NULL;
    }
  }
  if(func != NULL) {
    code = curl_easy_setopt(curl_handler, CURLOPT_WRITEFUNCTION, func);
    if(code != CURLE_OK) {
      ol_errorf("failed to set writefunction [%s]\n", errbuf);
      return NULL;
    }
  }
  if(data != NULL) {
    code = curl_easy_setopt(curl_handler, CURLOPT_WRITEDATA, data);
    if(code != CURLE_OK) {
      ol_errorf("failed to set writedata [%s]\n", errbuf);
      return NULL;
    }
  }
  code = curl_easy_setopt(curl_handler, CURLOPT_CONNECTTIMEOUT, connecttimeout);
  if(code != CURLE_OK) {
    ol_errorf("failed to set connecttimeout [%s]\n", errbuf);
    return NULL;
  }
  if (!_set_curl_proxy (curl_handler))
  {
    return NULL;
  }
  return curl_handler;
}

int
fetch_into_file(const char *url, const char *refer, FILE *fp)
{
  ol_log_func ();
  CURL *curl;
  CURLcode code;

  curl = my_curl_init(NULL, url, refer, NULL, NULL, 0, NULL, fp, cntimeout);
  if(curl == NULL)
    return -1;

  code = curl_easy_perform(curl);
  if(code != CURLE_OK) {
    ol_errorf ("failed to perform: [%s]\n", errbuf);
    curl_easy_cleanup(curl);
    return -1;
  }
  curl_easy_cleanup(curl);
  return 0;
}

static void *
myrealloc(void *ptr, size_t size)
{
  if(ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}

static size_t
WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  ol_log_func ();
  struct memo *memstr = (struct memo *)data;
  size_t need = size * nmemb;

  memstr->mem_base = myrealloc(memstr->mem_base, memstr->mem_len+need);
  if(memstr->mem_base) {
    memcpy(&memstr->mem_base[memstr->mem_len], ptr, need);
    memstr->mem_len += need;
  } else
    return 0;
  return need;
}

int
fetch_into_memory (const char *url,
                   const char *refer,
                   const char *user_agent,
                   const char *post_data,
                   size_t post_len,
                   struct memo *dest)
{
  ol_log_func ();
  CURL *curl;
  CURLcode code;
  WriteCallback callback = WriteMemoryCallback;

  curl = my_curl_init(NULL, url, refer, user_agent, post_data, post_len,
                      callback, dest, cntimeout);
  if(curl == NULL)
    return -1;

  code = curl_easy_perform(curl);
  if(code != CURLE_OK) {
    ol_errorf ("failed to perform: [%s]\n", errbuf);
    return -1;
  }
  curl_easy_cleanup(curl);
  return 0;
}

/*
 * HTML 格式编码
 * 对 String 编码时，使用以下规则：
 * 字母数字字符 "a" 到 "z"、"A" 到 "Z" 和 "0" 到 "9" 保持不变。
 * 特殊字符 "."、"-"、"*" 和 "_" 保持不变。
 * 如果space_cat非0，空格字符 " " 转换为 "+"，否则转换为"%20"。
 * 所有其他字符都是不安全的，因此首先使用一些编码机制将它们转换为一个或多个字节。
 * 然后每个字节用一个包含 3 个字符的字符串 "%xy" 表示，其中 xy 为该字节的两位十六进制表示形式。
 * 推荐的编码机制是 UTF-8。但是，出于兼容性考虑，如果未指定一种编码，则使用相应平台的默认编码。 
 */
int
url_encoding(const char *src, const int srclen, char *dest, int destlen, int space_cat)
{
  ol_log_func ();
  int i;
  int j = 0; 
  char ch;

  /*
   * convert to GBK, this should be done before this function called
   *
   char buf[BUFSZ];	
   char *src_copy;
   iconv_t icv;
   if(charset != NULL)
   icv = iconv_open("GBK", charset);
   else
   icv = iconv_open("GBK", "UTF-8");
	
   if((src_copy = malloc(srclen+1)) == NULL)
   return -1;
   memcpy(src_copy, src, srclen);
   src_copy[srclen] = 0;
   convert_icv(&icv, src_copy, srclen, buf, BUFSZ);
   iconv_close(icv);
   free(src_copy);
  */
    
  if(src == NULL || dest == NULL || srclen < 0 || destlen < 0)
    return -1;

  for(i=0; ((i<srclen) && j<destlen); i++) {
    ch = src[i];
    if((ch>='A') && (ch<='Z'))
      dest[j++] = ch;
    else if((ch>='a') && (ch<='z'))
      dest[j++] = ch;
    else if((ch>='0') && (ch<='9'))
      dest[j++] = ch;
    else if(ch=='.' || ch=='*' || ch=='_' || ch=='-' || ch=='%' || ch=='+')
      dest[j++] = ch;
    else if(space_cat && ch==' ')
    {
      dest[j++] = '+';
    }
    else {
      if(j+3 < destlen) {
        sprintf(dest+j, "%%%02X", (unsigned char)ch);
        j += 3;
      } else
        return -1;
    }
  }
  dest[j] = '\0';
  return 0;
}

static int 
chartonum(char ch)
{
  if(ch>='0' && ch<='9')
    return (char)(ch - '0');
  if(ch>='a' && ch<='z')
    return (char)(ch - 'a' + 10);
  if(ch>='A' && ch<='F')
    return (char)(ch - 'A' + 10);
  return -1;
}

int
url_decoding(const char *src, const int srclen, char *dest, int destlen)
{
  ol_log_func ();
  char ch;
  int idx1, idx2;
  int i;
  int j = 0; 

  if(src == NULL || dest == NULL || srclen < 0 || destlen < 0)
    return -1;

  for(i=0; (i<srclen && j<destlen); i++) {
    ch = src[i];
    switch(ch) {
    case '+' :
      dest[j++] = ' ';
      break;
    case '%':
      idx1 = chartonum(src[i+1]);
      idx2 = chartonum(src[i+2]);
      dest[j++] = (char)((idx1<<4) | idx2);
      i += 2;
      break;
    default:
      dest[j++] = ch;
    }
  }
  dest[j] = '\0';
  /*
   * convert to target charset, this is be done after this function called
   convert("GBK", charset==NULL ? "UTF-8" : charset, buf, strlen(buf), dest, destlen);
  */
  return 0;
}

int
curl_url_encoding(CURL *curl, char *input, char *output, size_t size)
{
  ol_log_func ();
  char *escp;
  int flag = 0;
  if(curl == NULL) {
    curl = curl_easy_init();
    flag = 1;
  }

  /* 
   * convert to GBK, this should be done before this function called
   *
   char buf[BUFSZ];
   iconv_t icv;
   if(charset != NULL)
   icv = iconv_open("GBK", charset);
   else
   icv = iconv_open("GBK", "UTF-8");
	
   convert_icv(&icv, input, strlen(input), buf, BUFSZ);
   iconv_close(icv);
  */

  escp = curl_easy_escape(curl, input, 0);
  if(escp == NULL) {
    ol_errorf ("curl_easy_escape error.\n");
    return -1;
  }
  if(strlen(escp) > size) {
    errno = E2BIG; /* identify that buffer storing the result is too small */
    return -1;
  }
  strcpy(output, escp);
  curl_free(escp);

  if(flag == 1)
    curl_easy_cleanup(curl);
  return 0;
}

int
curl_url_decoding(CURL *curl, char *input, char *output, size_t size)
{
  ol_log_func ();
  char *unescp;
  int flag = 0;
  if(curl == NULL) {
    curl = curl_easy_init();
    flag = 1;
  }

  unescp = curl_easy_unescape(curl, input, 0, NULL);
  if(unescp == NULL) {
    ol_errorf ("curl_easy_unescape error.\n");
    return -1;
  }
  if(strlen(unescp) > size) {
    errno = E2BIG; /* identify that buffer storing the result is too small */
    return -1;
  }
  strcpy(output, unescp); 

  /* 
   * convert to target charset, this is be done after this function called
   convert("GBK", charset==NULL ? "UTF-8" : charset, unescp, strlen(unescp), output, size);
  */

  curl_free(unescp);
  if(flag == 1)
    curl_easy_cleanup(curl);
  return 0;
}

int
ol_lrc_fetch_calc_rank (const OlMusicInfo *info,
                        OlLrcCandidate *candidate)
{
  ol_log_func ();
  if (info == NULL || candidate == NULL)
    return -1;
  double ret = 0.0;
  if (info->title != NULL && candidate->title != NULL)
  {
    int lcs = ol_lcs (info->title, candidate->title);
    ret += (double)(lcs * 2) / (strlen (info->title) + strlen (candidate->title));
    ret = ret * ret * 0.7;
  }
  if (info->artist != NULL && candidate->artist != NULL)
  {
    int lcs = ol_lcs (info->artist, candidate->artist);
    ret += (double)(lcs * 2) / (strlen (info->artist) + strlen (candidate->artist)) * 0.3;
  }
  return ret * RANK_SCALE;
}

int
ol_lrc_fetch_add_candidate (const OlMusicInfo *info,
                            OlLrcCandidate *candidate_list,
                            size_t count,
                            size_t size,
                            struct _OlLrcCandidate *new_candidate)
{
  ol_log_func ();
  if (info == NULL || candidate_list == NULL || new_candidate == NULL ||
      count < 0 || size <= 0)
    return 0;
  new_candidate->rank = ol_lrc_fetch_calc_rank (info, new_candidate);
  if (new_candidate->rank < LRC_RANK_THRESHOLD)
    return count;
  int pos = count;
  while (pos > 0 && new_candidate->rank > candidate_list[pos - 1].rank)
  {
    if (pos < size)
    {
      candidate_list[pos] = candidate_list[pos - 1];
    }
    pos--;
  }
  if (pos < size)
  {
    candidate_list[pos] = *new_candidate;
  }
  if (count < size)
    count++;
  return count;
}
