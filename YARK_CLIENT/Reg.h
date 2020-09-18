#pragma once
#include <map>
extern std::map<std::string, int> Registry;
int RegInt(std::string key, int defualt);