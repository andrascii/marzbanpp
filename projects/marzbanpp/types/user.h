#pragma once

#include "admin.h"

namespace marzbanpp {

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace data_limit_strategy_values {
constexpr auto kNoReset = "no_reset"sv;
constexpr auto kDay = "day"sv;
constexpr auto kWeek = "week"sv;
constexpr auto kMonth = "month"sv;
constexpr auto kYear = "year"sv;
}// namespace data_limit_strategy_values

namespace status_values {
constexpr auto kActive = "active"sv;
constexpr auto kOnHold = "on_hold"sv;
constexpr auto kDisabled = "disabled"sv;
constexpr auto kLimited = "limited"sv;
constexpr auto kExpired = "expired"sv;
}// namespace status_values

struct User {
  // vectors in not in std::optional because glaze
  // library doesn't initialize it if JSON doesn't contain fields in array.
  // Also it doesn't write anything to JSON if vector is empty.

  template <typename T>
  using Opt = std::optional<T>;

  struct Proxies {
    struct Vless {
      Opt<std::string> flow;
      Opt<std::string> id;
    };

    struct Shadowsocks {
      Opt<std::string> password;
      Opt<std::string> method;
    };

    Opt<Vless> vless;
    Opt<Shadowsocks> shadowsocks;
  };

  struct Inbounds {
    std::vector<std::string> vless;
    std::vector<std::string> shadowsocks;
  };

  struct ExcludedInbounds {
    std::vector<std::string> vless;
    std::vector<std::string> shadowsocks;
  };

  Opt<Proxies> proxies;
  Opt<uint64_t> expire;    // utc timestamp
  Opt<uint64_t> data_limit;// bytes
  Opt<std::string> data_limit_reset_strategy;
  Opt<Inbounds> inbounds;
  Opt<std::string> note;
  Opt<std::string> sub_updated_at;
  Opt<std::string> sub_last_user_agent;
  Opt<std::string> online_at;
  Opt<uint64_t> on_hold_expire_duration;
  Opt<std::string> on_hold_timeout;// date-time format (etc. 2023-11-03T20:30:00)
  Opt<std::string> auto_delete_in_days;
  Opt<std::string> next_plan;
  Opt<std::string> username;
  Opt<std::string> status;
  Opt<uint64_t> used_traffic;
  Opt<uint64_t> lifetime_used_traffic;
  Opt<std::string> created_at;
  std::vector<std::string> links;
  Opt<std::string> subscription_url;
  Opt<ExcludedInbounds> excluded_inbounds;
  Opt<Admin> admin;
};

}// namespace marzbanpp
