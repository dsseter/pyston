// Copyright (c) 2014-2015 Dropbox, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef PYSTON_CORE_UTIL_H
#define PYSTON_CORE_UTIL_H

#include <algorithm>
#include <cstdio>
#include <sys/time.h>

#include "core/common.h"
#include "core/stringpool.h"

namespace pyston {

class Timer {
private:
    static int level;
    timeval start_time;
    const char* desc;
    long min_usec;
    bool ended;
    std::function<void(long)> exit_callback;

public:
    Timer(const char* desc = NULL, long min_usec = -1);
    ~Timer();

    void setExitCallback(std::function<void(long)> _exit_callback) { exit_callback = _exit_callback; }

    void restart(const char* newdesc, long new_min_usec);
    void restart(const char* newdesc = NULL);

    long end();
    long split(const char* newdesc = NULL) {
        long rtn = end();
        restart(newdesc);
        return rtn;
    }
};

bool startswith(const std::string& s, const std::string& pattern);
bool endswith(const std::string& s, const std::string& pattern);

void removeDirectoryIfExists(const std::string& path);

// Checks that lhs and rhs, which are iterables of InternedStrings, have the
// same set of names in them.
template <class T1, class T2> bool sameKeyset(T1* lhs, T2* rhs) {
    std::vector<InternedString> lv, rv;
    for (typename T1::iterator it = lhs->begin(); it != lhs->end(); it++) {
        lv.push_back(it->first);
    }
    for (typename T2::iterator it = rhs->begin(); it != rhs->end(); it++) {
        rv.push_back(it->first);
    }

    std::sort(lv.begin(), lv.end());
    std::sort(rv.begin(), rv.end());

    std::vector<InternedString> lextra(lv.size());
    std::vector<InternedString>::iterator diffend
        = std::set_difference(lv.begin(), lv.end(), rv.begin(), rv.end(), lextra.begin());
    lextra.resize(diffend - lextra.begin());

    bool good = true;
    if (lextra.size()) {
        printf("Only in lhs:\n");
        for (int i = 0; i < lextra.size(); i++) {
            printf("%s\n", lextra[i].c_str());
        }
        good = false;
    }

    std::vector<InternedString> rextra(rv.size());
    diffend = std::set_difference(rv.begin(), rv.end(), lv.begin(), lv.end(), rextra.begin());
    rextra.resize(diffend - rextra.begin());

    if (rextra.size()) {
        printf("Only in rhs:\n");
        for (int i = 0; i < rextra.size(); i++) {
            printf("%s\n", rextra[i].c_str());
        }
        good = false;
    }
    return good;
}
}

#endif
