/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Helper::Scheduler.
 **/

#include "Scheduler.hpp"

#include <numeric>

namespace Helper {

Scheduler::Scheduler( const std::chrono::milliseconds allowedJitter ) :
  allowedJitterV{ allowedJitter }
{
}

Scheduler& Scheduler::majorFrameHook( MajorFrameHook hook )
{
  majorFrameHookV = std::move( hook );
  return *this;
}

Scheduler& Scheduler::minorFrameHook( MinorFrameHook hook )
{
  minorFrameHookV = std::move( hook );
  return *this;
}

Scheduler& Scheduler::task( std::chrono::milliseconds period, Task task )
{
  if ( isRunning() )
  {
    // already running
    return *this;
  }

  tasksV.emplace( period, std::move( task ) );

  // update periods
  majorFramePeriodV = std::chrono::milliseconds{
    std::lcm( majorFramePeriodV.count(), period.count() ) };
  minorFramePeriodV = std::chrono::milliseconds{
    std::gcd( minorFramePeriodV.count(), period.count() ) };

  numberOfMinorFramesV =
    ( minorFramePeriodV.count() != 0 ) ? ( majorFramePeriodV / minorFramePeriodV ) : 0;

  return *this;
}

std::chrono::milliseconds Scheduler::allowedJitter() const
{
  return allowedJitterV;
}

std::chrono::milliseconds Scheduler::majorFramePeriod() const
{
  return majorFramePeriodV;
}

std::chrono::milliseconds Scheduler::minorFramePeriod() const
{
  return minorFramePeriodV;
}

size_t Scheduler::majorFrameCount() const
{
  return majorFrameCountV;
}

size_t Scheduler::numberOfMinorFrames() const
{
  return numberOfMinorFramesV;
}

size_t Scheduler::missedMajorFrames() const
{
  return missedMajorFramesV;
}

size_t Scheduler::missedConsecutiveMajorFrames() const
{
  return missedConsecutiveMajorFramesV;
}

size_t Scheduler::missedMinorFrames() const
{
  return missedMinorFramesV;
}

size_t Scheduler::missedConsecutiveMinorFrames() const
{
  return missedConsecutiveMinorFramesV;
}

void Scheduler::start()
{
  if ( isRunning() )
  {
    // already running
    return;
  }

  if ( tasksV.empty() )
  {
    // No tasks available
    return;
  }

  majorFrameCountV = 0;
  missedMajorFramesV = 0;
  missedConsecutiveMajorFramesV = 0;
  missedMinorFramesV = 0;
  missedConsecutiveMinorFramesV = 0;

  threadV = std::jthread{ std::bind_front( &Scheduler::schedulerLoop, this ) };
}

void Scheduler::stop( const Stop stop )
{
  if ( !isRunning() )
  {
    return;
  }

  stopV = stop;
  threadV.request_stop();
}

bool Scheduler::isRunning() const
{
  return threadV.joinable();
}

void Scheduler::waitStopped()
{
  threadV.join();
}

void Scheduler::schedulerLoop( std::stop_token stopToken )
{
  auto majorFrameTime{ std::chrono::steady_clock::now() };

  for ( ;; )
  {
    // Check for Major Frame jittering
    if ( std::chrono::steady_clock::now() > ( majorFrameTime + allowedJitterV ) )
    {
      // handle jitter violation
      ++missedMajorFramesV;
      ++missedConsecutiveMajorFramesV;

      // TODO
    }
    else
    {
      missedConsecutiveMajorFramesV = 0;
    }

    ++majorFrameCountV;

    if ( majorFrameHookV )
    {
      majorFrameHookV( majorFrameCountV );
    }

    auto minorFrameTime{ majorFrameTime };
    for (
      std::chrono::milliseconds::rep minorFrameCount{ 0 };
      minorFrameCount < numberOfMinorFramesV;
      ++minorFrameCount )
    {
      // Check for Minor Frame jittering
      if ( std::chrono::steady_clock::now() > ( minorFrameTime + allowedJitterV ) )
      {
        // handle jitter violation

        ++missedMinorFramesV;
        ++missedConsecutiveMinorFramesV;

        // TODO
      }
      else
      {
        missedConsecutiveMinorFramesV = 0;
      }

      if ( minorFrameHookV )
      {
        minorFrameHookV( majorFrameCountV, minorFrameCount + 1 );
      }

      for ( const auto & [ taskPeriod, task ] : tasksV )
      {
        if ( ( minorFrameCount % ( numberOfMinorFramesV / ( majorFramePeriodV.count() / taskPeriod.count() ) ) ) == 0 )
        {
          task();
        }
      }

      if ( stopToken.stop_requested() && ( Stop::MinorFrame == stopV ) )
      {
        break;
      }

      minorFrameTime += minorFramePeriodV;

      // sleep for next minor frame
      std::this_thread::sleep_until( minorFrameTime );
    }

    // check if stop is requested
    if ( stopToken.stop_requested() )
    {
      break;
    }

    majorFrameTime += majorFramePeriodV;

    // sleep for next major frame
    std::this_thread::sleep_until( majorFrameTime );
  }
}

}
