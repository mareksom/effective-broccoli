#ifndef LOCK_FREE_QUEUE_H_
#define LOCK_FREE_QUEUE_H_

#include <atomic>

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
  // element of the queue and return true.  The first element is consumed
  // (i.e. it will not be returned with the next call to @Consume()).
  bool Consume(T& t);

  // Returns true when the queue is empty.
  bool IsEmpty();


  // --------------------------- WRITER FUNCTIONS --------------------------- //

  // Appends the value @t to the end of the queue.  When the queue is full
  // (there are @Size or more elements), actively waits for some free space.
  void Append(T t);

 private:
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
}

#endif  // LOCK_FREE_QUEUE_H_
