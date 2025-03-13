#pragma once

namespace marzbanpp {

template <typename F>
concept NoexceptInvocable = std::invocable<F> && noexcept(std::declval<F>()());

class Finally final {
 public:
  using F = std::function<void()>;

  template <NoexceptInvocable F>
  explicit Finally(F callback) : callback_{callback} {}

  ~Finally() {
    if (callback_) {
      callback_();
    }
  }

 private:
  F callback_;
};

}// namespace marzbanpp
