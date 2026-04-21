/// @file atomic.h
/// Contains primitives for synchronization in ESP.
/// As the ESP is single-threaded, the only possible data races come
/// from interrupts, so atomicity is guaranteed by disabling interrupts,
/// then enabling them after doing the update.
#pragma once

#include <interrupts.h>
#include <stdint.h>

/// @brief Contains utilities related to synchronization
namespace pluto::sync
{
  /// @brief Disables all interrupts and returns the previous PS register value.
  /// Pair every call with `restore_interrupt_mask()`.
  inline uint32_t interrupts_disable() noexcept
  {
    uint32_t saved;
    __asm__ volatile("rsil %0, 15" : "=r"(saved)::"memory");
    return saved;
  }

  /// @brief Restores the PS register saved by interrupts_disable().
  /// Does NOT unconditionally re-enable interrupts: it restores
  /// whatever level was active before.
  inline void interrupts_restore(uint32_t saved_ps) noexcept
  {
    // rsync is needed to guarantee the write has taken effect
    __asm__ volatile("wsr %0, ps\n"
                     "rsync" ::"r"(saved_ps)
                     : "memory");
  }

  /// @brief RAII utility over `interrupts_disable` and `interrupts_restore`
  class ScopedNoInterrupts
  {
    uint32_t _saved;

  public:
    ScopedNoInterrupts() noexcept
        : _saved(interrupts_disable())
    {
    }
    ~ScopedNoInterrupts() noexcept { interrupts_restore(_saved); }
    ScopedNoInterrupts(const ScopedNoInterrupts&)            = delete;
    ScopedNoInterrupts& operator=(const ScopedNoInterrupts&) = delete;
  };

  template<typename T>
  inline T atomic_load(volatile const T* ptr) noexcept
  {
    static_assert(sizeof(T) <= 4, "only 32-bit or smaller");
    // single instruction, no need to disable interrupts
    return *ptr;
  }

  template<typename T>
  inline void atomic_store(volatile T* ptr, T val) noexcept
  {
    // single instruction, no need to disable interrupts
    static_assert(sizeof(T) <= 4, "only 32-bit or smaller");
    *ptr = val;
  }

  template<typename T>
  inline T atomic_fetch_add(volatile T* ptr, T val) noexcept
  {
    static_assert(sizeof(T) <= 4, "only 32-bit or smaller");
    ScopedNoInterrupts guard;
    T prev = *ptr;
    *ptr   = static_cast<T>(prev + val);
    return prev;
  }

  template<typename T>
  inline T atomic_fetch_sub(volatile T* ptr, T val) noexcept
  {
    static_assert(sizeof(T) <= 4, "only 32-bit or smaller");
    ScopedNoInterrupts guard;
    T prev = *ptr;
    *ptr   = static_cast<T>(prev - val);
    return prev;
  }

  template<typename T>
  inline bool atomic_cas(volatile T* ptr, T& expected, T desired) noexcept
  {
    static_assert(sizeof(T) <= 4, "only 32-bit or smaller");
    ScopedNoInterrupts guard;
    T actual = *ptr;
    if (actual != expected)
    {
      expected = actual; // write back so caller can inspect or retry
      return false;
    }
    *ptr = desired;
    return true;
  }

  template<typename T>
  struct atomic
  {
    static_assert(sizeof(T) <= 4, "only 32-bit or smaller");

    atomic() noexcept = default;
    explicit constexpr atomic(T val) noexcept
        : _val(val)
    {
    }
    atomic(const atomic&)            = delete;
    atomic& operator=(const atomic&) = delete;
    atomic(atomic&&)                 = delete;
    atomic& operator=(atomic&&)      = delete;

    inline T load() const noexcept { return atomic_load(&_val); }
    inline void store(T val) noexcept { atomic_store(&_val, val); }

    inline T fetch_add(T val) noexcept { return atomic_fetch_add(&_val, val); }
    inline T fetch_sub(T val) noexcept { return atomic_fetch_sub(&_val, val); }

    inline bool compare_exchange(T& expected, T desired) noexcept
    {
      return atomic_cas(&_val, expected, desired);
    }

    inline T operator++(int) noexcept { return fetch_add(1); }
    inline T operator--(int) noexcept { return fetch_sub(1); }
    inline T operator++() noexcept { return fetch_add(1) + 1; }
    inline T operator--() noexcept { return fetch_sub(1) - 1; }

    inline operator T() const noexcept { return load(); }
    inline T operator=(T val) noexcept
    {
      store(val);
      return val;
    }

  private:
    volatile T _val{};
  };

  using atomic_u8   = atomic<uint8_t>;
  using atomic_u16  = atomic<uint16_t>;
  using atomic_u32  = atomic<uint32_t>;
  using atomic_i8   = atomic<int8_t>;
  using atomic_i16  = atomic<int16_t>;
  using atomic_i32  = atomic<int32_t>;
  using atomic_bool = atomic<bool>;
} // namespace pluto::sync