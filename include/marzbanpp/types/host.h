#pragma once

namespace marzbanpp {

using namespace std::string_view_literals;

namespace security_values {
constexpr auto kInboundDefault = "inbound_default"sv;
constexpr auto kNone = "none"sv;
constexpr auto kTls = "tls"sv;
}// namespace security_values

namespace alpn_values {
constexpr auto kEmpty = ""sv;
constexpr auto kH2 = "h2"sv;
constexpr auto kHttp11 = "http/1.1"sv;
constexpr auto kH2Http11 = "h2,http/1.1"sv;
}// namespace alpn_values

namespace fingerprint_values {
constexpr auto kChrome = "chrome"sv;
constexpr auto kFirefox = "firefox"sv;
constexpr auto kSafari = "safari"sv;
constexpr auto kIos = "ios"sv;
constexpr auto kAndroid = "android"sv;
constexpr auto kEdge = "edge"sv;
constexpr auto k360 = "360"sv;
constexpr auto kQq = "qq"sv;
constexpr auto kRandom = "random"sv;
constexpr auto kRandomized = "randomized"sv;
}// namespace fingerprint_values

struct Host {
  template <typename T>
  using Opt = std::optional<T>;

  Opt<std::string> remark;
  Opt<std::string> address;
  Opt<uint64_t> port;
  Opt<std::string> sni;
  Opt<std::string> host;
  Opt<std::string> path;
  Opt<std::string> security;
  Opt<std::string> alpn;
  Opt<std::string> fingerprint;
  Opt<bool> allowinsecure;
  Opt<bool> is_disabled;
  Opt<bool> mux_enable;
  Opt<bool> use_sni_as_host;
  Opt<bool> random_user_agent;
  Opt<std::string> noise_setting;
  Opt<std::string> fragment_setting;
};

}// namespace marzbanpp