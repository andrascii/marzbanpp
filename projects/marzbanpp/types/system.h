#pragma once

namespace marzbanpp {

struct System {
  template <typename T>
  using Opt = std::optional<T>;

  Opt<std::string> version;
  Opt<uint64_t> mem_total;
  Opt<uint64_t> mem_used;
  Opt<uint64_t> cpu_cores;
  Opt<double> cpu_usage;
  Opt<uint64_t> total_user;
  Opt<uint64_t> users_active;
  Opt<uint64_t> users_on_hold;
  Opt<uint64_t> users_disabled;
  Opt<uint64_t> users_expired;
  Opt<uint64_t> users_limited;
  Opt<uint64_t> online_users;
  Opt<uint64_t> incoming_bandwidth;
  Opt<uint64_t> outgoing_bandwidth;
  Opt<uint64_t> incoming_bandwidth_speed;
  Opt<uint64_t> outgoing_bandwidth_speed;
};

}// namespace marzbanpp