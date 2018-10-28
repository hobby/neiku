
// vim:fenc=utf-8:ff=unix:ts=4:sw=4:et
// desc: jsoncpp test.

#include <stdint.h>
#include <cstdio>
#include <string>

#include "jsoncpp/jsoncpp.h"

using namespace std;

int main(int argc, char* argv[])
{
    std::string jsonstr = "{\"name\":\"名字\",\"gender\":\"性别\", \"\":\"empty value\"}";
    printf("jsonstr:[%s]\n", jsonstr.c_str());

    Json::Reader reader;
    Json::Value json, defaultString("默认值");
    if (reader.parse(jsonstr, json))
    {
        printf("json:[%s]\n", json.toStyledString().c_str());

        std::string name = json["name"].asString();
        std::string gender = json["gender"].asString();
        std::string empty = json[""].asString();
        printf("json.name:[%s], json.gender:[%s], empty:[%s]\n"
               , name.c_str(), gender.c_str(), empty.c_str());
    }
    return 0;
}
