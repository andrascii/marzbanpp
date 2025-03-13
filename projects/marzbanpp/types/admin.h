#pragma once

namespace marzbanpp {

struct Admin {
  std::optional<std::string> username;
  std::optional<std::string> password;
  std::optional<bool> is_sudo;
  std::optional<uint64_t> telegram_id;
  std::optional<std::string> discord_webhook;
  std::optional<uint64_t> users_usage;
};

}// namespace marzbanpp