#ifndef OBJECT_UPDATER_CONSUME_H_
#define OBJECT_UPDATER_CONSUME_H_

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>

#include "lock_free_queue.h"

template <typename T>
class ObjectUpdaterConsume {
 public:
  ObjectUpdaterConsume();

  // If @block == false, returns nullptr when no object is available.
  // If @block == true, never return nullptr.
  const T* LockCurrentObject(bool block);

  // No need for releasing when @LockCurrentObject returned nullptr.
  void ReleaseCurrentObject();

  T* GetFreeObject();
  void SetCurrentObject(T* object);

 private:
  static constexpr int Size = 4;

  std::mutex mutex_;
  std::condition_variable cond_var_;

  T t_[Size];
  std::atomic<T*> current_object_;
  T* locked_current_object_;

  LockFreeQueue<T*, Size> return_queue_;
  int internal_return_stack_size_;
  T* internal_return_stack_[Size];
};


// -------------------------------------------------------------------------- //
// ----------------------------- Implementation ----------------------------- //
// -------------------------------------------------------------------------- //

template <typename T>
ObjectUpdaterConsume<T>::ObjectUpdaterConsume()
    : current_object_(nullptr), locked_current_object_(nullptr),
      return_queue_(), internal_return_stack_size_(0) {
  for (int i = 0; i < Size; i++) {
    return_queue_.Append(&t_[i]);
  }
}

template <typename T>
const T* ObjectUpdaterConsume<T>::LockCurrentObject(bool block) {
  assert(locked_current_object_ == nullptr);
  if (block) {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_var_.wait(lock, [this]() -> bool {
                           return current_object_.load() != nullptr;
                         });
  }
  locked_current_object_ = current_object_.exchange(nullptr);
  if (block) {
    assert(locked_current_object_ != nullptr);
  }
  return locked_current_object_;
}

template <typename T>
void ObjectUpdaterConsume<T>::ReleaseCurrentObject() {
  if (locked_current_object_ != nullptr) {
    return_queue_.Append(locked_current_object_);
    locked_current_object_ = nullptr;
  }
}

template <typename T>
T* ObjectUpdaterConsume<T>::GetFreeObject() {
  T* t_ptr;
  if (internal_return_stack_size_ == 0) {
    const bool not_empty = return_queue_.Consume(t_ptr);
    if (!not_empty) {
      assert(not_empty);
    }
  } else {
    internal_return_stack_size_--;
    t_ptr = internal_return_stack_[internal_return_stack_size_];
  }
  assert(t_ptr != nullptr);
  return t_ptr;
}

template <typename T>
void ObjectUpdaterConsume<T>::SetCurrentObject(T* object) {
  assert(object != nullptr);
  mutex_.lock();
  T* t_ptr = current_object_.exchange(object);
  mutex_.unlock();
  cond_var_.notify_one();
  if (t_ptr != nullptr) {
    internal_return_stack_[internal_return_stack_size_++] = t_ptr;
  }
}

#endif  // OBJECT_UPDATER_CONSUME_H_
