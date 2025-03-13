#pragma once

namespace marzbanpp {

struct Inbounds {
  struct Inbound {
    std::string tag;
    std::string protocol;
    std::string network;
    std::string tls;
    std::variant<uint64_t, std::string> port;
  };

  std::vector<Inbound> vless;
  std::vector<Inbound> vmess;
  std::vector<Inbound> trojan;
  std::vector<Inbound> shadowsocks;
};

}// namespace marzbanpp