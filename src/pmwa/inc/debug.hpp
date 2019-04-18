#ifndef PMWA_DEBUG_H
#define PMWA_DEBUG_H

#include <string>

#include <plog/Log.h>

class AutoPlog_impl{
    std::string message;
    std::string funcname;
    int line;
public:
    AutoPlog_impl(std::string const &msg, std::string const &f, int l):message(msg), funcname(f), line(l){
        LOGD << message << "> Start. (" << funcname << "@" << line << ")";
    }
    ~AutoPlog_impl(){
        LOGD << message << "> End. (" << funcname << "@" << line << ")";
    }
};

// we should expand macro twice to expand __LINE__ properly.
// (concatenation(##) is stronger than substitution (__LINE__ -> line number))
#define AutoPlog_macroimpl2(msg, f, l) AutoPlog_impl ADD_obj_##l(msg, f, l)
#define AutoPlog_macroimpl(msg, f, l) AutoPlog_macroimpl2(msg, f, l)
#define AutoPlog(msg) AutoPlog_macroimpl(msg, PLOG_GET_FUNC(), __LINE__)

#define NameValue(name) #name << " = " << (name)

#endif // PMWA_DEBUG_H
