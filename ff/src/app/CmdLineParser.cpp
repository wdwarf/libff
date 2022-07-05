/**
 * @file CmdLineParser.cpp
 * @auth DuckyLi
 * @date 2022-07-04 11:14:55
 * @description
 */

#include <ff/CmdLineParser.h>
#include <ff/String.h>

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

NS_FF_BEG

CmdLineParser::CmdLineParser(
    const std::initializer_list<CmdLineOption>& options, ff::Settings& optStore)
    : m_pValueStore(&optStore) {
  this->init(options);
}

CmdLineParser::CmdLineParser(
    const std::initializer_list<CmdLineOption>& options) {
  this->m_pValueStore = &this->m_valueStore;
  this->init(options);
}

void CmdLineParser::init(const std::initializer_list<CmdLineOption>& options) {
  stringstream helpStr;
  helpStr << "Usage:" << endl;

  for (auto& option : options) {
    if (option.name.empty()) continue;

    _CmdLineOption opt;
    opt.defaultValue = option.defaultValue;
    opt.description = option.description;

    if (StartsWith(option.name, "@")) {
      opt.longName = option.name.substr(1);
      this->m_positionalOptions.push_back(opt);

      helpStr << "\t@" << opt.longName << "\t" << opt.description << endl;
      continue;
    }

    vector<string> names =
        Split(option.name, ff::IsAnyOf(","), RemoveEmptyString);
    opt.longName = names[0];
    opt.shortName = names.size() > 1 ? names[1] : "";
    this->m_namedOptions.push_back(opt);

    helpStr << "\t";
    if (!opt.shortName.empty()) {
      helpStr << "-" << opt.shortName << " [--" << opt.longName << "]";
    } else {
      helpStr << "--" << opt.longName;
    }
    helpStr << "\t" << opt.description << endl;
  }

  this->m_usage = helpStr.str();
}

void CmdLineParser::parse(int argc, const char** argv) {
  if (argc < 2) return;

  auto doParse =
      [this](const std::string& cmd, const vector<_CmdLineOption>& namedOptions,
             vector<_CmdLineOption>& positionalOptions, ff::Settings& store) {
        if (0 == cmd.find("--")) {
          auto param = cmd.substr(2);
          auto pos = param.find_first_of('=');
          if (pos == string::npos) {
            for (auto& opt : namedOptions) {
              if (opt.longName == param) {
                store.setValue(opt.longName, "");
                break;
              }
            }

            return;
          }

          string name = param.substr(0, pos);
          string value = param.substr(pos + 1);
          for (auto& opt : namedOptions) {
            if (opt.longName == name) {
              store.setValue(opt.longName, value);
              break;
            }
          }

          return;
        }

        if (0 == cmd.find("-")) {
          auto param = cmd.substr(1);
          auto pos = param.find_first_of('=');
          if (pos == string::npos) {
            for (auto& opt : namedOptions) {
              if (opt.shortName == param) {
                store.setValue(opt.longName, "");
                break;
              }
            }

            return;
          }

          string name = param.substr(0, pos);
          string value = param.substr(pos + 1);
          for (auto& opt : namedOptions) {
            if (opt.shortName == name) {
              store.setValue(opt.longName, value);
              break;
            }
          }

          return;
        }

        if (positionalOptions.empty()) return;
        auto opt = *positionalOptions.begin();
        positionalOptions.erase(positionalOptions.begin());
        store.setValue(opt.longName, cmd);
      };

  auto positionalOptions = this->m_positionalOptions;
  for (int i = 1; i < argc; ++i) {
    doParse(argv[i], this->m_namedOptions, positionalOptions,
            *this->m_pValueStore);
  }
}

CmdLineParser::~CmdLineParser() {}

Variant CmdLineParser::get(const std::string& name) const {
  return this->m_pValueStore->getValue(name);
}

const std::string& CmdLineParser::usage() const { return this->m_usage; }

NS_FF_END
