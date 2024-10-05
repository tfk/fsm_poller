#include "fsm_poller.h"
#include <iostream>
#include <mutex>
#include <thread>
class MediaPlayer {

public:
  enum class State { STOPPED, PAUSED, PLAYING };

private:
  State s = State::STOPPED;
  std::mutex mtx;

public:
  State getState() {
    std::lock_guard<std::mutex> l(mtx);
    return s;
  }

  void play() {
    std::lock_guard<std::mutex> l(mtx);
    s = State::PLAYING;
  }

  void pause() {
    std::lock_guard<std::mutex> l(mtx);
    if (s == State::PLAYING) {
      s = State::PAUSED;
    }
  }

  void stop() {
    std::lock_guard<std::mutex> l(mtx);
    s = State::STOPPED;
  }
};

MediaPlayer player;

void pollFunction() {
  auto stateCheck = [&]() { return player.getState(); };
  fsm::Poller<MediaPlayer::State> poller(stateCheck);
  bool runFlag = true;
  poller.to(MediaPlayer::State::STOPPED, [&](auto _) {
    std::cout << "The music has stopped\n";
    runFlag = false;
  });

  poller.fromTo(MediaPlayer::State::PLAYING, MediaPlayer::State::PAUSED,
                [](auto _) { std::cout << "Just a small pause\n"; });

  poller.executeWhileInState(MediaPlayer::State::PLAYING, [](auto _) {
    std::cout << "The music is playing\n";
  });

  while (runFlag) {
    poller.update();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

int main() {
  std::thread t(pollFunction);

  player.play();
  std::this_thread::sleep_for(std::chrono::seconds(5));

  player.pause();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  player.play();
  std::this_thread::sleep_for(std::chrono::seconds(5));

  player.stop();
  std::this_thread::sleep_for(std::chrono::seconds(3));

  t.join();
  return 0;
}