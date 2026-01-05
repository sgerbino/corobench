#pragma once

#if defined(__clang__) && !defined(__apple_build_version__)
#define CORO_AWAIT_ELIDABLE [[clang::coro_await_elidable]]
#else
#define CORO_AWAIT_ELIDABLE
#endif
