/*
 * Copyright (C) 2016 Edward d'Auvergne
 *
 * This file is part of the program FlightGear.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include <cppunit/TestListener.h>

#include "fgTestListener.hxx"
#include "logging.hxx"

using namespace std;


// Handle failures.
void fgTestListener::addFailure(const CppUnit::TestFailure &failure)
{
    m_failure = true;
    if (failure.isError())
        m_error = true;
}


// Override the base class function to restore IO streams.
void fgTestListener::endTest(CppUnit::Test *test)
{
    // Test timing.
    sum_time += clock() - m_time;

    // Restore the IO streams.
    cout.rdbuf(orig_cout);
    cerr.rdbuf(orig_cerr);

    // Per-test single character status feedback.
    if (m_failure)
        cerr << (m_error ? "E" : "F");
    else
        cerr << '.';
    cerr.flush();

    // Store the captured IO for any failed tests.
    if (m_failure) {
        // Set up the data structure.
        TestIOCapt test_io;
        test_io.name = test->getName();

        // Standard IO.
        test_io.stdio = capt.str();

        // The simgear logstreams.
        capturedIO &obj = getIOstreams();
        test_io.sg_bulk = obj.sg_bulk.str();
        test_io.sg_bulk_only = obj.sg_bulk_only.str();
        test_io.sg_debug_only = obj.sg_debug_only.str();
        test_io.sg_info_only = obj.sg_info_only.str();
        test_io.sg_warn_only = obj.sg_warn_only.str();
        test_io.sg_alert_only = obj.sg_alert_only.str();

        // Add the test's IO to the list.
        io_capt.push_back(test_io);
    }
}


// Override the base class function to capture IO streams.
void fgTestListener::startTest(CppUnit::Test *test)
{
    // Clear the simgear logstream buffers.
    capturedIO &obj = getIOstreams();
    obj.sg_bulk.str("");
    obj.sg_bulk_only.str("");
    obj.sg_debug_only.str("");
    obj.sg_info_only.str("");
    obj.sg_warn_only.str("");
    obj.sg_alert_only.str("");

    // Store the original STDOUT and STDERR for restoring later on.
    orig_cout = cout.rdbuf();
    orig_cerr = cerr.rdbuf();

    // Clear the captured stream and then catch stdout and stderr.
    capt.str(string());
    cout.rdbuf(capt.rdbuf());
    cerr.rdbuf(capt.rdbuf());

    // Reset the test status.
    m_failure = false;
    m_error = false;
    m_time = clock();
}