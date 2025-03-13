#pragma once

namespace marzbanpp {

struct Admin {
  std::string username;
  bool is_sudo;
  uint64_t telegram_id;
  std::optional<std::string> discord_webhook;
  uint64_t users_usage;
};

}// namespace marzbanpp