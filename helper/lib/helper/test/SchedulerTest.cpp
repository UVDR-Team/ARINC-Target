// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Helper::Scheduler.
 **/

#include <helper/Scheduler.hpp>

#include <boost/test/unit_test.hpp>

namespace Helper {

BOOST_AUTO_TEST_SUITE( HelperTest )
BOOST_AUTO_TEST_SUITE( SchedulerTest )

//! How often task 1 is scheduled
static size_t task1Count{ 0 };
//! How often task 2 is scheduled
static size_t task2Count{ 0 };

//! Scheduler Test Task 1
static void task1()
{
  ++task1Count;
  BOOST_TEST_MESSAGE( "Task 1 " << task1Count );
}

//! Scheduler Test Task 2
static void task2( Scheduler &scheduler )
{
  ++task2Count;
  BOOST_TEST_MESSAGE( "Task 2 " << task2Count );

  if ( task2Count > 3 )
  {
    scheduler.stop( Scheduler::Stop::MinorFrame );
    BOOST_TEST_MESSAGE( "Stop Scheduler\n" );
  }
}

//! Scheduler Unit Test
BOOST_AUTO_TEST_CASE( test1 )
{
  Scheduler scheduler{ std::chrono::milliseconds{ 100 } };

  BOOST_CHECK( scheduler.majorFramePeriod() == std::chrono::milliseconds{ 1 } );
  BOOST_CHECK( scheduler.minorFramePeriod() == std::chrono::milliseconds{ 0 } );
  BOOST_CHECK( scheduler.numberOfMinorFrames() == 0 );

  BOOST_CHECK( scheduler.majorFrameCount() == 0 );
  BOOST_CHECK( scheduler.missedMajorFrames() == 0 );
  BOOST_CHECK( scheduler.missedConsecutiveMajorFrames() == 0 );
  BOOST_CHECK( scheduler.missedMinorFrames() == 0 );
  BOOST_CHECK( scheduler.missedConsecutiveMinorFrames() == 0 );

  scheduler.task( std::chrono::milliseconds{ 300 }, &task1 ) ;
  scheduler.task(
    std::chrono::milliseconds{ 400 },
    std::bind_front( &task2, std::ref( scheduler ) ) ) ;

  scheduler.majorFrameHook( []( const size_t majorFrameCount ){
    BOOST_TEST_MESSAGE( "Major Frame " << majorFrameCount );
  } );
  scheduler.minorFrameHook(
    []( const size_t majorFrameCount, const size_t minorFrameCount )
    {
      BOOST_TEST_MESSAGE( "Minor Frame " << majorFrameCount << ":" << minorFrameCount );
    } );

  BOOST_CHECK( scheduler.allowedJitter() == std::chrono::milliseconds{ 100 } );
  BOOST_CHECK( scheduler.majorFramePeriod() == std::chrono::milliseconds{ 1200 } );
  BOOST_CHECK( scheduler.minorFramePeriod() == std::chrono::milliseconds{ 100 } );
  BOOST_CHECK( scheduler.numberOfMinorFrames() == 12 );

  BOOST_CHECK( scheduler.majorFrameCount() == 0 );
  BOOST_CHECK( scheduler.missedMajorFrames() == 0 );
  BOOST_CHECK( scheduler.missedConsecutiveMajorFrames() == 0 );
  BOOST_CHECK( scheduler.missedMinorFrames() == 0 );
  BOOST_CHECK( scheduler.missedConsecutiveMinorFrames() == 0 );

  BOOST_CHECK( !scheduler.isRunning() );

  scheduler.start();

  BOOST_CHECK( scheduler.isRunning() );

  scheduler.waitStopped();

  BOOST_CHECK( !scheduler.isRunning() );

  BOOST_CHECK( scheduler.majorFrameCount() == 2 );
  BOOST_CHECK( scheduler.missedMajorFrames() == 0 );
  BOOST_CHECK( scheduler.missedConsecutiveMajorFrames() == 0 );
  BOOST_CHECK( scheduler.missedMinorFrames() == 0 );
  BOOST_CHECK( scheduler.missedConsecutiveMinorFrames() == 0 );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
