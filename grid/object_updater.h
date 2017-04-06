#ifndef OBJECT_UPDATER_H_
#define OBJECT_UPDATER_H_

#include <atomic>
#include <cassert>
#include <vector>

#include "makra.h"

template <typename T>
class ObjectUpdater {
 public:
  ObjectUpdater();

  const T* LockCurrentObject();
  void ReleaseCurrentObject();

  T* GetFreeObject();
  void AddObject(T* object);

  void SetCurrentObject(T* object);

 private:
  std::atomic<T*> current_object_;
  std::atomic<T*> return_bucket_;
  std::vector<T*> free_objects_;
  T* locked_current_object_;
};


// -------------------------------------------------------------------------- //
// ----------------------------- Implementation ----------------------------- //
// -------------------------------------------------------------------------- //

template <typename T>
ObjectUpdater<T>::ObjectUpdater()
    : current_object_(nullptr), return_bucket_(nullptr),
      free_objects_(), locked_current_object_(nullptr) {}

template <typename T>
const T* ObjectUpdater<T>::LockCurrentObject() {
  assert(locked_current_object_ == nullptr);
  locked_current_object_ = current_object_.exchange(nullptr);
  assert(locked_current_object_ != nullptr);
  return locked_current_object_;
}

template <typename T>
void ObjectUpdater<T>::ReleaseCurrentObject() {
  assert(locked_current_object_ != nullptr);
  T* null_object = nullptr;
  if (current_object_.compare_exchange_strong(
          null_object, locked_current_object_)) {
    locked_current_object_ = nullptr;
  } else {
    locked_current_object_ = return_bucket_.exchange(locked_current_object_);
    assert(locked_current_object_ == nullptr);
  }
}

template <typename T>
T* ObjectUpdater<T>::GetFreeObject() {
  assert(!free_objects_.empty());
  T* free_object_ = free_objects_.back();
  free_objects_.pop_back();
  return free_object_;
}

template <typename T>
void ObjectUpdater<T>::AddObject(T* object) {
  free_objects_.push_back(object);
}

template <typename T>
void ObjectUpdater<T>::SetCurrentObject(T* object) {
  T* returned_object = return_bucket_.exchange(nullptr);
  if (returned_object != nullptr) {
    AddObject(returned_object);
  }
  T* old_object = current_object_.exchange(object);
  if (old_object != nullptr) {
    AddObject(old_object);
  }
}

#endif  // OBJECT_UPDATER_H_
