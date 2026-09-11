/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Helper::Scheduler.
 **/

#ifndef HELPER_SCHEDULER_HPP
#define HELPER_SCHEDULER_HPP

#include <helper/Helper.hpp>

#include <atomic>
#include <chrono>
#include <functional>
#include <map>
#include <thread>

namespace Helper {

/**
 * @brief The %Scheduler class provides a mechanism for scheduling and executing tasks at specified periods.
 *
 * The %Scheduler class allows tasks to be scheduled at specific periods.
 * It automatically executes these tasks at their respective periods.
 * The %Scheduler class supports both major frame and minor frame scheduling.
 *
 * The %Scheduler class can be started and stopped, and it provides information about the scheduling process, such as
 * the number of missed major and minor frames.
 *
 * @note
 * The %Scheduler class uses a separate thread for scheduling and executing tasks.
 * It is safe to use the %Scheduler class in a multithreaded environment.
 **/
class HELPER_EXPORT Scheduler
{
  public:
    //! Task to be scheduled
    using Task = std::function< void() >;

    /**
     * @brief Major Frame Hook.
     *
     * @param[in] majorFrame
     *   Active Major Frame.
     **/
    using MajorFrameHook = std::function< void( size_t majorFrame ) >;

    /**
     * @brief Minor Frame Hook.
     *
     * @param[in] majorFrame
     *   Active Major Frame.
     * @param[in] minorFrame
     *   Active Minor Frame.
     **/
    using MinorFrameHook = std::function< void( size_t majorFrame, size_t minorFrame ) >;

    //! Kind of Stop Condition
    enum class Stop
    {
      //! Stop at end of Major Frame
      MajorFrame,
      //! Stop at end of Minor Frame
      MinorFrame
    };

    /**
     * @brief Initialises the %Scheduler Class.
     *
     * @param[in] allowedJitter
     *   Allowed jitter of major and minor frame timing during execution.
     **/
    explicit Scheduler( std::chrono::milliseconds allowedJitter );

    /**
     * @brief Deleted Copy Constructor
     *
     * @param[in,out] scheduler
     *   Source instance.
     **/
    Scheduler( Scheduler &scheduler ) = delete;

    /**
     * @brief Deleted Move Constructor
     *
     * @param[in,out] scheduler
     *   Source instance.
     **/
    Scheduler( Scheduler &&scheduler ) = delete;

    //! Destructor
    ~Scheduler() = default;

    /**
     * @brief Deleted Assignment Operator.
     *
     * @param[in,out] scheduler
     *   Source instance.
     **/
    Scheduler &operator=( Scheduler &scheduler ) = delete;

    /**
     * @brief Deleted Move-Assignment Operator.
     *
     * @param[in,out] scheduler
     *   Source instance.
     **/
    Scheduler &operator=( Scheduler &&scheduler ) = delete;

    /**
     * @brief Add Major Frame Hook.
     *
     * The hook is called on every start of the Major Frame.
     *
     * @param[in] hook
     *   Major Frame Hook.
     *
     * @return @p *this for chaining calls.
     **/
    Scheduler& majorFrameHook( MajorFrameHook hook );

    /**
     * @brief Add Minor Frame Hook.
     *
     * The hook is called on every start of the Minor Frame.
     *
     * @param[in] hook
     *   Minor Frame Hook.
     *
     * @return @p *this for chaining calls.
     **/
    Scheduler& minorFrameHook( MinorFrameHook hook );

    /**
     * @brief Add task to %Scheduler.
     *
     * @param[in] period
     *   Period, when @p task shall be scheduled.
     * @param[in] task
     *   Task to be added.
     *
     * @return @p *this for chaining calls.
     **/
    Scheduler& task( std::chrono::milliseconds period, Task task );

    /**
     * @brief Returns the set Value for the Allowed Jitter.
     *
     * @return Allowed Jitter.
     **/
    [[nodiscard]] std::chrono::milliseconds allowedJitter() const;

    /**
     * @brief Returns the Major Frame Period.
     *
     * This value is calculated depending on the task periods.
     * The _Major Frame Period_ is the lcm of all task periods.
     *
     * @return Major Frame Period
     **/
    [[nodiscard]] std::chrono::milliseconds majorFramePeriod() const;

    /**
     * @brief Returns the Major Frame Period.
     *
     * This value is calculated depending on the task periods.
     * The _Major Frame Period_ is the lcm of all task periods.
     *
     * @return Major Frame Period
     **/
    [[nodiscard]] std::chrono::milliseconds minorFramePeriod() const;

    /**
     * @brief Returns the Minor Frame Period.
     *
     * This value is calculated depending on the task periods.
     * The Minor Frame Period_ is the gcd of all task periods.
     *
     * @return Minor Frame Period
     **/
    [[nodiscard]] size_t majorFrameCount() const;

    /**
     * @brief Returns the Number of Minor Frames per Major Frame.
     *
     * The number od Minor Frames is calculated by dividing the Major Frame Period by the Minor Frame Period.
     *
     * @return Number of Minor Frames per Major Frame.
     **/
    [[nodiscard]] size_t numberOfMinorFrames() const;

    /**
     * @brief Returns the Number of Missed Major Frames.
     *
     * On start of a Major Frame, the expected and actual start time of the Major Frame is compared.
     * If the difference is bigger than the allowed jitter, this counter is incremented.
     *
     * @return Number of Missed Major Frames
     **/
    [[nodiscard]] size_t missedMajorFrames() const;

    /**
     * @brief Returns the Number of Missed Consecutive Major Frames.
     *
     * On start of a Major Frame, the expected and actual start time of the Major Frame is compared.
     * If the difference is bigger than the allowed jitter, this counter is incremented.
     * This counter is reset, on the next in-time Major Frame.
     *
     * @return Number of Missed Consecutive Major Frames
     **/
    [[nodiscard]] size_t missedConsecutiveMajorFrames() const;

    /**
     * @brief Returns the Number of Missed Minor Frames.
     *
     * On start of a Minor Frame, the expected and actual start time of the Minor Frame is compared.
     * If the difference is bigger than the allowed jitter, this counter is incremented.
     *
     * @return Number of Missed Minor Frames
     **/
    [[nodiscard]] size_t missedMinorFrames() const;

    /**
     * @brief Returns the Number of Missed Consecutive Minor Frames.
     *
     * On start of a Minor Frame, the expected and actual start time of the Minor Frame is compared.
     * If the difference is bigger than the allowed jitter, this counter is incremented.
     * This counter is reset, on the next in-time Minor Frame.
     *
     * @return Number of Missed Consecutive Minor Frames
     **/
    [[nodiscard]] size_t missedConsecutiveMinorFrames() const;

    /**
     * @brief Start the Scheduling Process.
     *
     * The start() method starts the scheduling process.
     * Once started, the %Scheduler will execute the scheduled tasks at their respective periods.
     *
     * @note
     * Calling start() when the scheduling process is already running has no effect.
     *
     * @sa stop()
     **/
    void start();

    /**
     * @brief Stop the Scheduling Process.
     *
     * The stop() method stops the scheduling process.
     *
     * @note
     * If the scheduling process is already stopped, calling stop() has no effect.
     *
     * @sa start()
     **/
    void stop( Stop stop = Stop::MajorFrame );

    /**
     * @brief Returns, if the %Scheduler is currently running.
     *
     * @return If the scheduler is currently running.
     **/
    [[nodiscard]] bool isRunning() const;

    /**
     * @brief Waits until the %Scheduler is stopped.
     **/
    void waitStopped();

  private:
    //! Tasks
    using Tasks = std::multimap< std::chrono::milliseconds, Task >;

    /**
     * @brief Thread function for Scheduling and Executing Tasks.
     *
     * This function is the entry point of the separate thread used by the Scheduler class for scheduling and executing
     * tasks.
     * It continuously checks if any tasks are due for execution based on their periods and executes them accordingly.
     *
     * @param[in] stopToken
     *   A std::stop_token object that can be used to stop the scheduling process.
     **/
    void schedulerLoop( std::stop_token stopToken );

    /**
     * @brief The maximum allowed jitter for scheduling tasks.
     *
     * It represents the maximum allowed jitter for scheduling tasks in milliseconds.
     *
     * Jitter refers to the variation in the execution timing of tasks.
     * In the context of the Scheduler class, it refers to the difference between the expected execution time of a task
     * and the actual execution time.
     * The @ref allowedJitterV value is used in the schedulerLoop() method to determine if a task is due for execution
     * based on its period.
     *
     * If the current time exceeds the calculated execution time of a task by more than the @ref allowedJitterV, the
     * task is considered to have missed its deadline and will not be executed in that iteration of the scheduling loop.
     *
     * Example usage:
     *
     * @code
     * // Check if the current time exceeds the calculated execution time of a task by more than the AllowedJitter
     * if (std::chrono::high_resolution_clock::now() > (minorFrameTime + allowedJitterV)) {...}
     * @endcode
     **/
    const std::chrono::milliseconds allowedJitterV;
    //! Major Frame Period.
    std::chrono::milliseconds majorFramePeriodV{ 1 };
    //! Minor Frame Period.
    std::chrono::milliseconds minorFramePeriodV{ 0 };
    //! Number of minor frames within Major Frame.
    std::chrono::milliseconds::rep numberOfMinorFramesV{ 0 };
    //! Number of missed Major Frames
    std::size_t missedMajorFramesV{ 0 };
    //! Number of missed consecutive Major Frames
    std::size_t missedConsecutiveMajorFramesV{ 0 };
    //! Number of missed Minor Frames
    std::size_t missedMinorFramesV{ 0 };
    //! Number of missed consecutive Minor Frames
    std::size_t missedConsecutiveMinorFramesV{ 0 };

    //! Scheduled tasks
    Tasks tasksV;
    //! Major Frame Hook
    MajorFrameHook majorFrameHookV;
    //! Minor Frame Hook
    MinorFrameHook minorFrameHookV;
    //! Schedule Thread
    std::jthread threadV;
    //! Number of scheduled Major Frames
    std::atomic< size_t > majorFrameCountV;
    //! Stop condition
    std::atomic< Stop > stopV;
};

}

#endif
