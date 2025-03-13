#include "api.h"

#include "net/http_client.h"
#include "net/http_headers.h"
#include "types/user.h"

namespace {

using namespace marzbanpp;

auto RestApiStatusCodeDescription(Api::RestApiStatusCode error) noexcept {
  switch (error) {
    case Api::RestApiStatusCode::kOk: return "Ok"s;
    case Api::RestApiStatusCode::kYouAreNotAllowed: return "You're not allowed"s;
    case Api::RestApiStatusCode::kUserNotFound: return "User not found"s;
    case Api::RestApiStatusCode::kValidationError: return "Validation error"s;
    default: return "Unknown error code: "s + std::to_string(static_cast<int>(error));
  }
};

}// namespace

namespace marzbanpp {

struct AdminToken {
  std::string access_token;
  std::string token_type;
};

Api::Ptr Api::AuthAndCreate(const std::string& uri, const std::string& username, const std::string& password) {
  HttpClient http_client;
  HttpHeaders headers;

  headers.Add("Content-Type", "application/x-www-form-urlencoded");

  const auto post_data = std::format("username={}&password={}", username, password);
  const auto response = http_client.Post(uri + "/api/admin/token", post_data, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("error retrieving admin token for {}: {}"),
        username,
        response.error().Description())};
  }

  if (response->status_code != static_cast<int>(RestApiStatusCode::kOk)) {
    throw std::runtime_error{
      std::format(
        "error retrieving admin token for {}: {}",
        username,
        RestApiStatusCodeDescription(static_cast<RestApiStatusCode>(response->status_code)))};
  }

  auto admin_token = glz::read_json<AdminToken>(response->body);

  if (!admin_token) {
    throw std::runtime_error{admin_token.error().custom_error_message.data()};
  }

  struct MakeSharedEnabler : Api {
    MakeSharedEnabler(std::string uri, AdminToken token)
        : Api(
            std::move(uri),
            std::move(token.token_type),
            std::move(token.access_token)) {}
  };

  return std::make_shared<MakeSharedEnabler>(uri, std::move(*admin_token));
}

Api::Expected<Inbounds>
Api::GetInbounds() const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/inbounds/"s, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  const auto inbounds = glz::read_json<Inbounds>(response->body);

  if (inbounds) {
    return *inbounds;
  }

  const auto error = Error{
    .response_body = response->body,
    .response_headers = response->headers,
    .status_code = response->status_code,
    .error = fmt::format(fmt::runtime("parsing Inbounds object failed: {}"), glz::format_error(inbounds, response->body))};

  return std::unexpected{error};
}

Api::Expected<User>
Api::AddUser(const User& user) const {
  if (!user.username.has_value()) {
    throw std::runtime_error{"user.username must be set but none"};
  }

  const auto is_valid_status =
    user.status.has_value() &&
    (*user.status == status_values::kActive || *user.status == status_values::kOnHold);

  if (!is_valid_status) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("user status must be '{}' or '{}'"),
        status_values::kActive,
        status_values::kOnHold)};
  }

  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string json_request;

  if (glz::write_json(user, json_request)) {
    throw std::runtime_error{fmt::format(fmt::runtime("error JSONify user to send request"))};
  }

  HttpClient http_client;
  const auto response = http_client.Post(uri_ + "/api/user/"s, json_request, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  const auto added_user = glz::read_json<User>(response->body);

  if (added_user) {
    return *added_user;
  }

  const auto error = Error{
    .response_body = response->body,
    .response_headers = response->headers,
    .status_code = response->status_code,
    .error = fmt::format(fmt::runtime("parsing JSON failed: {}"), glz::format_error(added_user, response->body))};

  return std::unexpected{error};
}

Api::Expected<User>
Api::GetUser(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/user/"s + username, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  const auto user = glz::read_json<User>(response->body);

  if (user) {
    return *user;
  }

  const auto error = Error{
    .response_body = response->body,
    .response_headers = response->headers,
    .status_code = response->status_code,
    .error = fmt::format(fmt::runtime("parsing JSON failed: {}"), glz::format_error(user, response->body))};

  return std::unexpected{error};
}

Api::Expected<User>
Api::ModifyUser(const std::string& username, const User& modified_user) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string json_request;

  if (glz::write_json(modified_user, json_request)) {
    throw std::runtime_error{fmt::format(fmt::runtime("error JSONify user to send request"))};
  }

  HttpClient http_client;
  const auto response = http_client.Put(uri_ + "/api/user/"s + username, json_request, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  const auto added_user = glz::read_json<User>(response->body);

  if (added_user) {
    return *added_user;
  }

  const auto error = Error{
    .response_body = response->body,
    .response_headers = response->headers,
    .status_code = response->status_code,
    .error = fmt::format(fmt::runtime("parsing JSON failed: {}"), glz::format_error(added_user, response->body))};

  return std::unexpected{error};
}

Api::RestApiStatusCode
Api::RemoveUser(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Delete(uri_ + "/api/user/"s + username, {}, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error for '{}': {}"),
        __FUNCTION__,
        username,
        response.error().Description())};
  }

  return static_cast<Api::RestApiStatusCode>(response->status_code);
}

Api::Expected<User>
Api::ResetUserDataUsage(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Post(uri_ + "/api/user/"s + username + "/reset", {}, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error for '{}': {}"),
        __FUNCTION__,
        username,
        response.error().Description())};
  }

  const auto user = glz::read_json<User>(response->body);

  if (user) {
    return *user;
  }

  const auto error = Error{
    .response_body = response->body,
    .response_headers = response->headers,
    .status_code = response->status_code,
    .error = fmt::format(fmt::runtime("parsing JSON failed: {}"), glz::format_error(user, response->body))};

  return std::unexpected{error};
}

Api::Expected<Users>
Api::GetUsers(GetUsersParams params) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/x-www-form-urlencoded");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string query;
  std::vector<std::string> data;

  if (params.limit) {
    data.push_back("limit=" + std::to_string(*params.limit));
  }

  if (params.offset) {
    data.push_back("offset=" + std::to_string(*params.offset));
  }

  if (params.sort) {
    data.push_back("sort=" + *params.sort);
  }

  if (params.status) {
    data.push_back("status=" + *params.status);
  }

  if (params.username && !params.username->empty()) {
    for (const auto& username : *params.username) {
      data.push_back("username=" + username);
    }
  }

  for (const auto& query_element : data) {
    query += query_element + "&";
  }

  query.pop_back();

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/users/?" + query, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  const auto users = glz::read_json<Users>(response->body);

  if (users) {
    return *users;
  }

  const auto error = Error{
    .response_body = response->body,
    .response_headers = response->headers,
    .status_code = response->status_code,
    .error = fmt::format(fmt::runtime("parsing JSON failed: {}"), glz::format_error(users, response->body))};

  return std::unexpected{error};
}

Api::Api(std::string uri, std::string token_type, std::string access_token)
    : uri_{std::move(uri)},
      token_type_{std::move(token_type)},
      access_token_{std::move(access_token)} {}

Api::~Api() = default;

}// namespace marzbanpp
