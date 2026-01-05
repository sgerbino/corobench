#pragma once

#if defined(__clang__) && !defined(__apple_build_version__)
#define CORO_AWAIT_ELIDABLE [[clang::coro_await_elidable]]
#define CORO_AWAIT_ELIDABLE_ARGUMENT [[clang::coro_await_elidable_argument]]
#else
#define CORO_AWAIT_ELIDABLE
#define CORO_AWAIT_ELIDABLE_ARGUMENT
#endif