#include <functional>
#include <list>

namespace fsm {

template <class T> class Poller {
public:
  using State = T;
  struct TransitionEvent {
    State from;
    State to;
  };
  using TransitionCallback = std::function<void(TransitionEvent event)>;
  using StateCallback = std::function<void(State state)>;

private:
  State lastValue;
  std::function<State()> valueGetter;

  using WatchFunction = std::function<void(TransitionEvent)>;
  std::list<WatchFunction> watchers;
  std::list<StateCallback> stateObserver;

  void callWatchers(const TransitionEvent &t) const {
    for (const auto &callWatcher : watchers) {
      callWatcher(t);
    }
  }

  void callStateObservers(const State &s) const {
    for (const auto &observer : stateObserver) {
      observer(s);
    }
  }

public:
  explicit Poller(State &value)
      : lastValue(value), valueGetter([&value]() { return value; }) {}

  explicit Poller(std::function<State()> valueGetter)
      : lastValue(valueGetter()), valueGetter(valueGetter) {}

  Poller(T &firstValue, std::function<T()> valueGetter)
      : lastValue(firstValue), valueGetter(valueGetter) {}

  State getValue() { return valueGetter(); }

  void update() {
    State newValue = valueGetter();
    if (newValue != lastValue) {
      callWatchers({lastValue, newValue});
      lastValue = newValue;
    }
    callStateObservers(newValue);
  }

  void to(const State targetState, TransitionCallback callback) {
    watchers.emplace_back([targetState, callback](const TransitionEvent &t) {
      if (targetState == t.to) {
        callback(t);
      }
    });
  }

  void to(const std::initializer_list<State> targetStates,
          TransitionCallback callback) {
    for (const auto &target : targetStates) {
      to(target, callback);
    }
  }

  void from(const State sourceState, TransitionCallback callback) {
    watchers.emplace_back([sourceState, callback](const TransitionEvent &t) {
      if (sourceState == t.from) {
        callback(t);
      }
    });
  }

  void from(const std::initializer_list<State> sourceStates,
            TransitionCallback callback) {
    for (const auto &source : sourceStates) {
      from(source, callback);
    }
  }

  void fromTo(const State sourceState, const State targetState,
              TransitionCallback callback) {
    watchers.emplace_back(
        [sourceState, targetState, callback](const TransitionEvent &t) {
          if (sourceState == t.from && targetState == t.to) {
            callback(t);
          }
        });
  }

  void fromTo(const std::initializer_list<State> sourceStates,
              const std::initializer_list<State> targetStates,
              TransitionCallback callback) {
    for (const auto &source : sourceStates) {
      for (const auto &target : targetStates) {
        fromTo(source, target, callback);
      }
    }
  }

  void anyTransition(TransitionCallback callback) {
    watchers.emplace_back(
        [callback](const TransitionEvent &t) { callback(t); });
  }

  void executeWhileInState(const State s, StateCallback callback) {
    stateObserver.emplace_back([s, callback](State currentState) {
      if (currentState == s) {
        callback(currentState);
      }
    });
  }

  void executeWhileInStates(const std::initializer_list<State> states,
                            StateCallback callback) {
    for (const auto &state : states) {
      executeWhileInState(state, callback);
    }
  }
};

} // namespace fsm
