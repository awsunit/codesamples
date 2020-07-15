/*
 * Copyright ©2019 Hannah C Tang.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Autumn Quarter 2019 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>

#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

using std::map;
using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::size_t;

namespace hw4 {

#define  npos std::string::npos
/* converts string into a lowercase version */
static void ToLower(string *string);
/* Trims the whitespace off both sides of string (thx stackof) */
static void Trim(string *string);
 /*
  Helper for Trim, gets rid of spaces to right of word
  OpenSourced help aided in the construction of this and lt.
  args: word - the string to trim whitespace from
  modifies: word
  effects: post-method - any whitespacec to the right of
                         word is gone
 */
static inline void rt(string *word);
 /*
  Helper for Trim, gets rid of spaces to left of word

  args: word - the string to trim whitespace from
  modifies: word
  effects: post-method - any whitespacec to the left of
                         word is gone
 */
static inline void lt(string *word);
/* Checks if buf is at \r\n */
static bool AtDelimiter(unsigned char *buf);

static const char *kHeaderEnd = "\r\n\r\n";
static const int kHeaderEndLen = 4;

#define BSIZE 2
bool HttpConnection::GetNextRequest(HttpRequest *request) {
  // Use "WrappedRead" to read data into the buffer_
  // instance variable.  Keep reading data until either the
  // connection drops or you see a "\r\n\r\n" that demarcates
  // the end of the request header.
  //
  // Once you've seen the request header, use ParseRequest()
  // to parse the header into the *request argument.
  //
  // Very tricky part:  clients can send back-to-back requests
  // on the same socket.  So, you need to preserve everything
  // after the "\r\n\r\n" in buffer_ for the next time the
  // caller invokes GetNextRequest()!

  // STEP 1:
  unsigned char buf[BSIZE];
  size_t read = 0;  // used for tracking position of this request within buffer_
  int temp;
  bool seen = false;
  string s;
  while ((temp = WrappedRead(fd_, buf, BSIZE)) != 0) {
    if (temp == -1) {
      cerr << "bad code" << endl;
      return false;
    }
    read += temp;
    string r = reinterpret_cast<char *>(buf);
    s.append(r);
    if (AtDelimiter(buf)) {
      if (seen == true) {
        break;
      }
      seen = true;
    } else {
      seen = false;
    }
  }
  buffer_ = s;
  *request = ParseRequest(buffer_);
  //
  return true;
}
static bool AtDelimiter(unsigned char *buf) {
  // check end of read data for kHeaderEnd
  const char *c = kHeaderEnd;
  for (int i = 0; i < 2; i++) {
    if (*c++ != *buf++) {
      return false;
    }
  }
  return true;
}

bool HttpConnection::WriteResponse(const HttpResponse &response) {
  string str = response.GenerateResponseString();
  int res = WrappedWrite(fd_,
                         (unsigned char *) str.c_str(),
                         str.length());
  if (res != static_cast<int>(str.length()))
    return false;
  return true;
}

HttpRequest HttpConnection::ParseRequest(const string &headers) {
  HttpRequest req("/");  // by default, get "/".
  // Split the header into lines.  Extract the URI from the first line
  // and store it in req.URI.  For each additional line beyond the
  // first, extract out the header name and value and store them in
  // req.headers_ (i.e., HttpRequest::AddHeader).  You should look
  // at HttpResponse.h for details about the HTTP header format that
  // you need to parse.
  //
  // You'll probably want to look up boost functions for (a) splitting
  // a string into lines on a "\r\n" delimiter, (b) trimming
  // whitespace from the end of a string, and (c) converting a string
  // to lowercase.

  // STEP 2:
  //
  if (headers.compare("") != 0) {
    string delim = "\r\n";
    size_t a, uripos, nextpos, pos = headers.find(delim);
    string u;
    u = headers.substr(0, pos);
    // get uri end position
    uripos = u.find(" ", 4);
    string uri;
    uri = u.substr(4, uripos - 3);
    Trim(&uri);
    req.set_uri(uri);
    while ((nextpos = headers.find(delim, pos + 1)) != npos) {
      // found a header -> split up
      size_t spot;
      string key, value, lk, lv;
      if ((spot = headers.find(":", pos)) == npos)
        return req;  // malformed -> error?
      // spot at :
      key = headers.substr(pos + 2, spot - (pos + 2));
      value = headers.substr(spot + 1, nextpos - (spot + 1));
      // lowercase/trim
      ToLower(&key);
      Trim(&key);
      ToLower(&value);
      Trim(&value);
      // add to req
      req.AddHeader(key, value);
      pos = nextpos;
      if ((a = headers.find(delim, nextpos + 1)) == (nextpos + 2)) {
        break;  // at end of headers
      }
    }  // end-while
  }  // end-if (emptystring)
  return req;
}

static inline void lt(string *word) {
  string s = *word;
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
          return !std::isspace(ch); }));
  *word = s;
}

static inline void rt(string *word) {
  string s = *word;
  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
          return !std::isspace(ch); }).base(), s.end());
  *word = s;
}

static void Trim(string *s) {
  lt(s);
  rt(s);
}

static void ToLower(string *s) {
  string word = *s;
  std::transform(word.begin(), word.end(), word.begin(),
         [](unsigned char c){ return std::tolower(c);});
  *s = word;
}
}  // namespace hw4
