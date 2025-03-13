#pragma once

#include "user.h"

namespace marzbanpp {

struct Users {
  std::vector<User> users;
  uint64_t total;
};

}// namespace marzbanpp