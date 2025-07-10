# marzbanpp
marzbanpp is the C++ library for interaction with Marzban through its REST API.

This is a synchronous library I wrote to my own needs.

Feel free to fork and improve it if you want (i.e. you can make async version of it).
Also you can develop tests.

## To install use vcpkg
```bash
vcpkg install marzbanpp
```

Add in the CMakeLists.txt:
```cmake
include(CMakeFindDependencyMacro)
find_dependency(marzbanpp REQUIRED)
target_link_libraries(your-target-name PUBLIC marzbanpp::marzbanpp)
```

## Examples
```c++
// C/C++
#include <chrono>
#include <iostream>

// fmt
#include <fmt/core.h>
#include <fmt/ranges.h>

// marzbanpp
#include <marzbanpp/marzbanpp.h>

using namespace std::chrono_literals;

using Seconds = std::chrono::seconds;
using TimePoint = std::chrono::time_point<std::chrono::system_clock, Seconds>;


TimePoint Now() {
  using namespace std::chrono;
  return time_point_cast<seconds>(system_clock::now());
}


void Print500Users(const std::shared_ptr<marzbanpp::IApi>& api) {
  const auto params = marzbanpp::IApi::GetUsersParams{
    .offset = 0,
    .limit = 500,
    .status = "active"
  };

  const auto users = api->GetUsers(params);

  for (const auto& user : users.users) {
    const auto has_username = user.username.has_value();
    const auto has_expire = user.expire.has_value();

    if (has_username && has_expire) {
      const auto timepoint = TimePoint{Seconds{*user.expire}};
      std::string expire = fmt::format("{:%d-%m-%Y}", timepoint);
      std::cout << "username is " << *user.username << ", expire is " << expire << std::endl;
    } else if (user.username.has_value()) {
      std::cout << "username: " << *user.username << std::endl;
    } else {
      std::cout << "found user without username and expire fields (this shouldn't normally happen)" << std::endl;
    }
  }
}


//
// Creates user with specified expiration date/note/username and add him VLESS inbound with xtls-rprx-vision flow
//
marzbanpp::User CreateUser(
  const std::shared_ptr<marzbanpp::IApi>& api,
  const std::string& username,
  const TimePoint& expiration_date,
  const std::string& note
) const {
  const auto inbounds = api->GetInbounds();

  marzbanpp::User::Inbounds user_inbounds;
  for (const auto& inbound : inbounds.vless) {
    user_inbounds.vless.push_back(inbound.tag);
  }

  marzbanpp::User::Proxies::Vless vless;
  vless.flow = "xtls-rprx-vision";
  marzbanpp::User::Proxies proxies;
  proxies.vless = vless;

  marzbanpp::User user;
  user.status = status_values::kActive;
  user.username = username;
  user.inbounds = user_inbounds;
  user.proxies = proxies;
  user.expire = expiration_date.time_since_epoch().count();
  user.note = note;

  return api->AddUser(user);
}


//
// Extends user subscription expiration time
//
User ExtendUserExpirationDate(const std::shared_ptr<marzbanpp::IApi>& api, const std::string& username, const Seconds& interval) const {
  auto user = api->GetUser(username);

  if (!user.has_value()) {
    throw std::runtime_error{"user '" + username + "' not found"};
  }

  user->status = status_values::kActive;

  if (!user->expire.has_value()) {
    user->expire = (Now() + interval).time_since_epoch().count();
  } else {
    const auto expire_seconds = Seconds{*user->expire};
    const auto expire_time_point = TimePoint{expire_seconds};
    const auto has_not_expired = Now() < expire_time_point;

    if (has_not_expired) {
      user->expire = (expire_time_point + interval).time_since_epoch().count();
    } else {
      user->expire = (Now() + interval).time_since_epoch().count();
    }
  }

  return api->ModifyUser(username, *user);
}

int main() {
  try {
    // marzbanpp::ApiDecorator is a wrapper over marzbanpp::Api implementation.
    // marzbanpp::ApiDecorator duplicates interface of marzbanpp::Api.
    // But ApiDecorator internally catches MarzbanServerResponseError with status_code == 401 which means that auth token is expired and handles this situation.
    // So using ApiDecorator you don't need to handle reauth, it will be done automatically.
    // And you can just use api methods.

    const auto api = std::make_shared<marzbanpp::ApiDecorator>(
      "https://marzban-panel.com:8000",
      "marzban-admin",
      "marzban-admin-password"
    );
  
    Print500Users(api);
  
    const auto expire = Now() + 7d; // 7 days from now
    const auto user = CreateUser(api, "User9000", expire, "note: this is my bro");
    // do something with user...

    // extend user's subscription for 1 day
    const auto modified_user = ExtendUserExpirationDate(api, "User9000", 86400s);
    // do something with modified_user...
  
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
}
```
