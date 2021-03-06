#pragma once

#include <chrono>
#include <functional>

#include "envoy/server/drain_manager.h"
#include "envoy/server/instance.h"

#include "common/common/logger.h"

namespace Envoy {
namespace Server {

/**
 * Implementation of drain manager that does the following by default:
 * 1) Terminates the parent process after 15 minutes.
 * 2) Drains the parent process over a period of 10 minutes where drain close becomes more
 *    likely each second that passes.
 */
class DrainManagerImpl : Logger::Loggable<Logger::Id::main>, public DrainManager {
public:
  DrainManagerImpl(Instance& server);

  // Server::DrainManager
  bool drainClose() const override;
  void startDrainSequence(std::function<void()> completion) override;
  void startParentShutdownSequence() override;

private:
  bool draining() const { return drain_tick_timer_ != nullptr; }
  void drainSequenceTick();

  Instance& server_;
  Event::TimerPtr drain_tick_timer_;
  std::atomic<uint32_t> drain_time_completed_{};
  Event::TimerPtr parent_shutdown_timer_;
  std::function<void()> drain_sequence_completion_;
};

} // namespace Server
} // namespace Envoy
