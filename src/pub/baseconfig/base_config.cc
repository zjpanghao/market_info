/*******************************************************************************
 * Copyright 2016/05 - 2021/07 
 * file	  :
 * author : tianyiheng
 * date   : 2016-06-14
 * brief  :
 *******************************************************************************/
#include <string.h>
#include <fstream>
#include <algorithm>
#include "base_config.h"


namespace plugin_pub {

typedef std::map<std::string, std::string> MapStrToStr;


ConfigValue::ConfigValue() {

}


ConfigValue::~ConfigValue() {

}

void ConfigValue::AddValue(std::string const & key, std::string const& val) {
  MapStrToStr::iterator it = map_key_val_.find(key);
  if (map_key_val_.end() == it) {
    map_key_val_.insert(std::make_pair(key, val));
  }
}

std::string ConfigValue::GetValue(std::string const & key) {
  std::string tmp = "0";
  MapStrToStr::iterator it = map_key_val_.find(key);
  if (map_key_val_.end() != it) {
    tmp = it->second;
  }
  return tmp;
}
// ---------------------------------------------------------------------------

BaseConfig::BaseConfig() {

}

BaseConfig::~BaseConfig() {
  Clear();
}

void BaseConfig::Clear() {
  std::map<std::string, ConfigValue *>::iterator it = config_sec_.begin();
  for (; it != config_sec_.end(); ++it) {
	delete it->second;
	it->second = NULL;
  }
  config_sec_.clear();
}

bool BaseConfig::LoadFile(std::string config_file) {
  std::ifstream in_file;
  in_file.open(config_file.c_str(), std::ios::in);
  if (!in_file.is_open()) {
	return false;
  }
  ParseFile(in_file);
  in_file.close();
  return true;
}


void BaseConfig::ParseFile(std::ifstream & in_file) {
  char line_buffer[1024] = "";
  std::string line_str;
  std::string cur_section;
  while (in_file.getline(line_buffer, sizeof(line_buffer))) {
    line_str = line_buffer;
    line_str = EraseSpaceToken(line_str);
    if (IsSect(line_str)) {
      cur_section = FetchSecName(line_str);
    }
    else if (IsKeyValue(line_str)) {
      std::pair<std::string, std::string> kv_pair = FectchKeyValue(line_str);
      AddConfig(cur_section, kv_pair.first, kv_pair.second);
    }
    memset(line_buffer, 0, sizeof(line_buffer));
  }
}

std::string BaseConfig::EraseSpaceToken(std::string const &s) {
  std::string tmp;
  for (size_t i = 0; i < s.length(); ++i) {
    if (s[i] == ETOKEN_TAB ||
        s[i] == ETOKEN_BACKSPACE ||
        s[i] == ETOKEN_RETURN ||
        s[i] == ETOKEN_LINE) {
      continue;
    }
    tmp.append(1, s[i]);

  }
  return tmp;
}

bool BaseConfig::IsSect(std::string const &s) {
  return (s.find('[') != std::string::npos && s.find(']') != std::string::npos);
}

std::string BaseConfig::FetchSecName(std::string const & s) {
  std::string sec_name;
  for (size_t i = 0; i < s.length(); ++i) {
	if (s[i] != '[' && s[i] != ']') {
	  sec_name.append(1, s[i]);
	}
  }
  return sec_name;
}

bool BaseConfig::IsKeyValue(std::string const &s) {
  return (std::count(s.begin(), s.end(), '=') == 1);
}

void  BaseConfig::AddConfig(std::string sect, std::string key, std::string val) {
  std::map<std::string, ConfigValue *>::iterator it = config_sec_.find(sect);
  if (it == config_sec_.end()) {
	ConfigValue * conf_val_pt = new ConfigValue;
	if (NULL != conf_val_pt) {
	  conf_val_pt->AddValue(key, val);
	  config_sec_.insert(std::make_pair(sect, conf_val_pt));
	}
  }
  else {
	ConfigValue * conf_val_pt = it->second;
	if (NULL != conf_val_pt) {
	  conf_val_pt->AddValue(key, val);
	}
  }
}

std::pair<std::string, std::string> BaseConfig::FectchKeyValue(std::string const & s) {
  std::string key, val;
  std::pair<std::string, std::string> kv_pair;
  size_t pos = s.find('=');
  if (pos != std::string::npos) {
	key = s.substr(0, pos);
	val = s.substr(pos + 1, std::string::npos);
  }
  kv_pair.first = key;
  kv_pair.second = val;
  return kv_pair;
}

std::string BaseConfig::GetParamString(std::string sec_name, std::string key) {
  std::string val = "0";
  std::map<std::string, ConfigValue *>::iterator it = config_sec_.find(sec_name);
  if (it != config_sec_.end()) {
	ConfigValue * conf_val_pt = it->second;
	if (NULL != conf_val_pt) {
	  val = conf_val_pt->GetValue(key);
	}
  }
  return val;
}

int BaseConfig::GetParamInt(std::string sec_name, std::string key) {
  std::string val = "0";
  std::map<std::string, ConfigValue *>::iterator it = config_sec_.find(sec_name);
  if (it != config_sec_.end()) {
	ConfigValue * conf_val_pt = it->second;
	if (NULL != conf_val_pt) {
	  val = conf_val_pt->GetValue(key);
	}
  }
  std::stringstream ss;
  ss << val;
  int int_val = 0;
  ss >> int_val;
  return int_val;
}

}  // namespace plugin_pub
