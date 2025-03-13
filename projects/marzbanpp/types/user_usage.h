#pragma once

namespace marzbanpp {

struct UserUsage {
  struct Usage {
    std::optional<uint64_t> node_id;
    std::optional<std::string> node_name;
    std::optional<uint64_t> used_traffic;
  };

  std::string username;
  std::vector<Usage> usages;
};

}// namespace marzbanpp