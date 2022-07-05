/**
 * @file CmdLineParser.h
 * @auth DuckyLi
 * @date 2022-07-04 11:13:40
 * @description
 */

#ifndef _CMDLINEPARSER_H_
#define _CMDLINEPARSER_H_

#include <ff/Settings.h>
#include <ff/Variant.h>
#include <ff/ff_config.h>

#include <initializer_list>
#include <map>
#include <string>
#include <vector>

NS_FF_BEG

/**
 * name:
 *  [longName,shotName] or [@name],
 *  exp:
 *    [help,h] == app.exe --help, app.exe -h
 *    [@imgPath] == resize.exe img.jpg
 *
 * defaultValue:
 *  default value for param
 *
 * description:
 *  description of param
 */

struct LIBFF_API CmdLineOption {
  std::string name;
  std::string defaultValue;
  std::string description;
};

class LIBFF_API CmdLineParser {
 private:
  struct _CmdLineOption {
    std::string longName;
    std::string shortName;
    std::string defaultValue;
    std::string description;
  };

  void init(const std::initializer_list<CmdLineOption>& options);

  ff::Settings m_valueStore;
  ff::Settings* m_pValueStore;
  std::string m_usage;
  std::vector<_CmdLineOption> m_namedOptions;
  std::vector<_CmdLineOption> m_positionalOptions;

 public:
  CmdLineParser(const std::initializer_list<CmdLineOption>& options);
  CmdLineParser(const std::initializer_list<CmdLineOption>& options,
                ff::Settings& optStore);
  ~CmdLineParser();

  void parse(int argc, const char** argv);
  Variant get(const std::string& name) const;
  const std::string& usage() const;
};

NS_FF_END

#endif /** _CMDLINEPARSER_H_ */
