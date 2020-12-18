/*
 * This file is part of PowerDNS or dnsdist.
 * Copyright -- PowerDNS.COM B.V. and its contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * In addition, for the avoidance of any doubt, permission is granted to
 * link this program with OpenSSL and to (re)distribute the binaries
 * produced as the result of such linking.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <inttypes.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include "iputils.hh"
#include "dnsname.hh"
#include "sholder.hh"
#include <atomic>
#include <boost/optional.hpp>

extern GlobalStateHolder<SuffixMatchNode> g_dontThrottleNames;
extern GlobalStateHolder<NetmaskGroup> g_dontThrottleNetmasks;

/** this class is used both to send and answer channel commands to the PowerDNS Recursor */
class RecursorControlChannel
{
public:
  RecursorControlChannel();

  ~RecursorControlChannel();

  int listen(const std::string& filename);
  void connect(const std::string& path, const std::string& filename);

  uint64_t getStat(const std::string& name);

  void send(const std::pair<int, const std::string&>&, const std::string* remote=nullptr, unsigned int timeout=5, int fd = -1);
  std::pair<int, std::string> recv(std::string* remote=0, unsigned int timeout=5);

  int d_fd;
  static volatile sig_atomic_t stop;
private:
  struct sockaddr_un d_local;
};

class RecursorControlParser
{
public:
  RecursorControlParser()
  {
  }
  static void nop(void){}
  typedef void func_t(void);
  pair<int, std::string> getAnswer(int s, const std::string& question, func_t** func);
};

enum class StatComponent { API, Carbon, RecControl, SNMP };

struct StatsMapEntry {
  std::string d_prometheusName;
  std::string d_value;
};
typedef std::map<std::string, StatsMapEntry> StatsMap;
StatsMap getAllStatsMap(StatComponent component);

extern std::mutex g_carbon_config_lock;
std::vector<std::pair<DNSName, uint16_t> >* pleaseGetQueryRing();
std::vector<std::pair<DNSName, uint16_t> >* pleaseGetServfailQueryRing();
std::vector<std::pair<DNSName, uint16_t> >* pleaseGetBogusQueryRing();
std::vector<ComboAddress>* pleaseGetRemotes();
std::vector<ComboAddress>* pleaseGetServfailRemotes();
std::vector<ComboAddress>* pleaseGetBogusRemotes();
std::vector<ComboAddress>* pleaseGetLargeAnswerRemotes();
std::vector<ComboAddress>* pleaseGetTimeouts();
DNSName getRegisteredName(const DNSName& dom);
std::atomic<unsigned long>* getDynMetric(const std::string& str, const std::string& prometheusName);
boost::optional<uint64_t> getStatByName(const std::string& name);
bool isStatBlacklisted(StatComponent component, const std::string& name);
void blacklistStat(StatComponent component, const string& name);
void blacklistStats(StatComponent component, const string& stats);

void registerAllStats();

void doExitGeneric(bool nicely);
void doExit();
void doExitNicely();
