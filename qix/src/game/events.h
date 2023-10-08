#pragma once

#include <deque>
#include <vector>
#include <algorithm>

struct Event {
  enum class Type {
    None
  };

  inline explicit Event(Type type) : type_(type) {}

  Type type_;
};

inline bool IsIn(Event::Type type, const std::initializer_list<Event::Type>& list) {
  return std::find(list.begin(), list.end(), type) != list.end();
}

class EventListener {
 public:
  virtual ~EventListener() noexcept {}

  virtual void Update(const Event& event) = 0;
};

class Events {
 public:
  enum class QueueRule { AllowDuplicates, NoDuplicates };

  Events() = default;

  Events(const Events&) = delete;

  Events(const Events&&) noexcept = delete;

  void Push(Event::Type type, QueueRule queue_rule = QueueRule::AllowDuplicates) {
    if (Event::Type::None == type) {
      return;
    }
    if (QueueRule::NoDuplicates == queue_rule) {
      Remove(type);
    }
    events_.emplace_back(type);
  }

  template<class ...Args>
  void Push(Args&&... args) { events_.emplace_back(std::forward<Args>(args)...); }

  inline void PushFront(Event::Type type) { events_.emplace_front(type); }

  inline void Remove(Event::Type type) { events_.erase(std::remove(events_.begin(), events_.end(), type), events_.end()); }

  Event Pop() {
    auto event = events_.front();

    events_.pop_front();

    return event;
  }

  void Clear() { events_.clear(); }

  inline bool IsEmpty() const { return events_.empty(); }

 private:
  std::deque<Event> events_;
};
