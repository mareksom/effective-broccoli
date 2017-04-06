#ifndef GRID_LOCK_FREE_QUEUE_H_
#define GRID_LOCK_FREE_QUEUE_H_

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace Grid {

// A queue that uses only a single atomic variable.  It is faster than a normal
// queue protected with a mutex.  However, it can only be used between two
// threads: reader and writer, where reader can only read, and writer can only
// write.
template <typename T, int Size>
class LockFreeQueue {
 public:
  LockFreeQueue();

  // --------------------------- READER FUNCTIONS --------------------------- //

  // When the queue is empty, returns false.  Otherwise, sets @t to the first
  // element of the queue and returns true.  The first element is consumed
  // (i.e. it will not be returned with the next call to @Consume*()).
  bool Consume(T& t);

  // When the queue is empty, blocks.  Otherwise, sets @t to the first element
  // of the queue and returns true.  The first element is consumed
  // (i.e. it will not be returned with the next call to @Consume*()).
  void ConsumeBlock(T& t);

  // Returns true when the queue is empty.
  bool IsEmpty();


  // --------------------------- WRITER FUNCTIONS --------------------------- //

  // Appends the value @t to the end of the queue.  When the queue is full
  // (there are @Size or more elements), actively waits for some free space.
  void Append(T t);

 private:
  std::mutex mutex_;
  std::condition_variable cond_var_;

  int begin_;
  int end_;
  std::atomic<int> length_;
  T array_[Size];
};


// -------------------------------------------------------------------------- //
// ----------------------------- Implementation ----------------------------- //
// -------------------------------------------------------------------------- //

template <typename T, int Size>
LockFreeQueue<T, Size>::LockFreeQueue() : begin_(0), end_(0), length_(0) {}

template <typename T, int Size>
bool LockFreeQueue<T, Size>::Consume(T& t) {
  if (length_.load() == 0) {
    return false;
  }
  t = array_[begin_];
  if (++begin_ == Size) {
    begin_ = 0;
  }
  length_.fetch_sub(1);
  return true;
}

template <typename T, int Size>
void LockFreeQueue<T, Size>::ConsumeBlock(T& t) {
  if (length_.load() == 0) {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_var_.wait(lock, [this]() -> bool {
                          return length_.load() != 0;
                        });
  }
  t = array_[begin_];
  if (++begin_ == Size) {
    begin_ = 0;
  }
  length_.fetch_sub(1);
}

template <typename T, int Size>
bool LockFreeQueue<T, Size>::IsEmpty() {
  return length_.load() > 0;
}

template <typename T, int Size>
void LockFreeQueue<T, Size>::Append(T t) {
  while (length_.load() == Size) {
    // Wait.
  }
  array_[end_] = t;
  if (++end_ == Size) {
    end_ = 0;
  }
  length_.fetch_add(1);
  cond_var_.notify_one();
}

}  // namespace Grid

#endif  // GRID_LOCK_FREE_QUEUE_H_
