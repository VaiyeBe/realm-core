/*************************************************************************
 *
 * Copyright 2016 Realm Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 **************************************************************************/

#include "testsettings.hpp"
#ifdef TEST_COLUMN_MIXED

#include <limits>

#include <memory>
#include <realm/column_mixed.hpp>

#include "test.hpp"

using namespace realm;
using namespace realm::util;


// Test independence and thread-safety
// -----------------------------------
//
// All tests must be thread safe and independent of each other. This
// is required because it allows for both shuffling of the execution
// order and for parallelized testing.
//
// In particular, avoid using std::rand() since it is not guaranteed
// to be thread safe. Instead use the API offered in
// `test/util/random.hpp`.
//
// All files created in tests must use the TEST_PATH macro (or one of
// its friends) to obtain a suitable file system path. See
// `test/util/test_path.hpp`.
//
//
// Debugging and the ONLY() macro
// ------------------------------
//
// A simple way of disabling all tests except one called `Foo`, is to
// replace TEST(Foo) with ONLY(Foo) and then recompile and rerun the
// test suite. Note that you can also use filtering by setting the
// environment varible `UNITTEST_FILTER`. See `README.md` for more on
// this.
//
// Another way to debug a particular test, is to copy that test into
// `experiments/testcase.cpp` and then run `sh build.sh
// check-testcase` (or one of its friends) from the command line.


TEST(MixedColumn_Int)
{
    ref_type ref = MixedColumn::create(Allocator::get_default());
    MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

    CHECK(!c.is_nullable());

    int64_t max_val = std::numeric_limits<int64_t>::max();
    int64_t min_val = std::numeric_limits<int64_t>::min();
    int64_t all_bit = 0xFFFFFFFFFFFFFFFFULL; // FIXME: Undefined cast from unsigned to signed

    c.insert_int(0, 2);
    c.insert_int(1, min_val);
    c.insert_int(2, max_val);
    c.insert_int(3, all_bit);
    CHECK_EQUAL(4, c.size());
    CHECK(!c.is_null(0));
    CHECK_LOGIC_ERROR(c.set_null(0), LogicError::column_not_nullable);

    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_Int, c.get_type(i));

    CHECK_EQUAL(2, c.get_int(0));
    CHECK_EQUAL(min_val, c.get_int(1));
    CHECK_EQUAL(max_val, c.get_int(2));
    CHECK_EQUAL(all_bit, c.get_int(3));

    c.set_int(0, 400);
    c.set_int(1, 0);
    c.set_int(2, -99999);
    c.set_int(3, 1);

    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_Int, c.get_type(i));

    CHECK_EQUAL(400, c.get_int(0));
    CHECK_EQUAL(0, c.get_int(1));
    CHECK_EQUAL(-99999, c.get_int(2));
    CHECK_EQUAL(1, c.get_int(3));
    CHECK_EQUAL(4, c.size());

    c.destroy();
}


TEST(MixedColumn_Float)
{
    ref_type ref = MixedColumn::create(Allocator::get_default());
    MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

    uint32_t v = 0xFFFFFFFF;
    float f = float(v);
    float fval1[] = {0.0f, 100.123f, -111.222f, f};
    float fval2[] = {-0.0f, -100.123f, std::numeric_limits<float>::max(), std::numeric_limits<float>::min()};

    // Test insert
    for (size_t i = 0; i < 4; ++i)
        c.insert_float(i, fval1[i]);
    CHECK_EQUAL(4, c.size());

    for (size_t i = 0; i < c.size(); ++i) {
        CHECK_EQUAL(type_Float, c.get_type(i));
        CHECK_EQUAL(fval1[i], c.get_float(i));
    }

    // Set to new values - ensure sign is changed
    for (size_t i = 0; i < 4; ++i)
        c.set_float(i, fval2[i]);

    for (size_t i = 0; i < c.size(); ++i) {
        CHECK_EQUAL(type_Float, c.get_type(i));
        CHECK_EQUAL(fval2[i], c.get_float(i));
    }
    CHECK_EQUAL(4, c.size());

    c.destroy();
}


TEST(MixedColumn_Double)
{
    ref_type ref = MixedColumn::create(Allocator::get_default());
    MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

    uint64_t v = 0xFFFFFFFFFFFFFFFFULL;
    double d = double(v);
    double fval1[] = {1.0, 200.123, -111.222, d};
    double fval2[] = {-1.0, -100.123, std::numeric_limits<double>::max(), std::numeric_limits<double>::min()};

    // Test insert
    for (size_t i = 0; i < 4; ++i)
        c.insert_double(i, fval1[i]);
    CHECK_EQUAL(4, c.size());

    for (size_t i = 0; i < c.size(); ++i) {
        CHECK_EQUAL(type_Double, c.get_type(i));
        double v_i = c.get_double(i);
        CHECK_EQUAL(fval1[i], v_i);
    }

    // Set to new values - ensure sign is changed
    for (size_t i = 0; i < 4; ++i)
        c.set_double(i, fval2[i]);

    CHECK_EQUAL(4, c.size());
    for (size_t i = 0; i < c.size(); ++i) {
        CHECK_EQUAL(type_Double, c.get_type(i));
        CHECK_EQUAL(fval2[i], c.get_double(i));
    }

    c.destroy();
}


TEST(MixedColumn_Bool)
{
    ref_type ref = MixedColumn::create(Allocator::get_default());
    MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

    c.insert_bool(0, true);
    c.insert_bool(1, false);
    c.insert_bool(2, true);
    CHECK_EQUAL(3, c.size());

    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_Bool, c.get_type(i));

    CHECK_EQUAL(true, c.get_bool(0));
    CHECK_EQUAL(false, c.get_bool(1));
    CHECK_EQUAL(true, c.get_bool(2));

    c.set_bool(0, false);
    c.set_bool(1, true);
    c.set_bool(2, false);
    CHECK_EQUAL(3, c.size());

    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_Bool, c.get_type(i));

    CHECK_EQUAL(false, c.get_bool(0));
    CHECK_EQUAL(true, c.get_bool(1));
    CHECK_EQUAL(false, c.get_bool(2));

    c.destroy();
}


TEST(MixedColumn_Date)
{
    ref_type ref = MixedColumn::create(Allocator::get_default());
    MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

    c.insert_olddatetime(0, 2);
    c.insert_olddatetime(1, 100);
    c.insert_olddatetime(2, 20000);
    CHECK_EQUAL(3, c.size());

    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_OldDateTime, c.get_type(i));

    CHECK_EQUAL(2, c.get_olddatetime(0));
    CHECK_EQUAL(100, c.get_olddatetime(1));
    CHECK_EQUAL(20000, c.get_olddatetime(2));

    c.set_olddatetime(0, 400);
    c.set_olddatetime(1, 0);
    c.set_olddatetime(2, 99999);

    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_OldDateTime, c.get_type(i));

    CHECK_EQUAL(400, c.get_olddatetime(0));
    CHECK_EQUAL(0, c.get_olddatetime(1));
    CHECK_EQUAL(99999, c.get_olddatetime(2));
    CHECK_EQUAL(3, c.size());

    c.destroy();
}


TEST(MixedColumn_Timestamp)
{
    ref_type ref = MixedColumn::create(Allocator::get_default());
    MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

    c.insert_timestamp(0, Timestamp{});
    c.insert_timestamp(1, Timestamp(100, 200));
    c.insert_timestamp(2, Timestamp(0, 0)); // Should *not* equal null
    c.insert_timestamp(3, Timestamp(-1000, 0));

    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_Timestamp, c.get_type(i));

    CHECK_EQUAL(4, c.size());
    //    CHECK(c.get_timestamp(0) == Timestamp(null()));
    //    operator== should not be called for null according to column_timestamp.hpp:38
    CHECK(c.get_timestamp(1) == Timestamp(100, 200));
    CHECK(c.get_timestamp(2) == Timestamp(0, 0)); // Should *not* equal null
    CHECK(c.get_timestamp(3) == Timestamp(-1000, 0));

    // MixedColumn has not implemented is_null
    //    CHECK(c.is_null(0));
    //    CHECK(!c.is_null(1));
    //    CHECK(!c.is_null(2));
    //    CHECK(!c.is_null(3));

    c.set_timestamp(0, Timestamp(555, 666));
    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_Timestamp, c.get_type(i));
    CHECK(c.get_timestamp(0) == Timestamp(555, 666));

    c.destroy();
}


TEST(MixedColumn_String)
{
    ref_type ref = MixedColumn::create(Allocator::get_default());
    MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

    c.insert_string(0, "aaa");
    c.insert_string(1, "bbbbb");
    c.insert_string(2, "ccccccc");
    CHECK_EQUAL(3, c.size());

    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_String, c.get_type(i));

    CHECK_EQUAL("aaa", c.get_string(0));
    CHECK_EQUAL("bbbbb", c.get_string(1));
    CHECK_EQUAL("ccccccc", c.get_string(2));

    c.set_string(0, "dd");
    c.set_string(1, "");
    c.set_string(2, "eeeeeeeee");
    CHECK_EQUAL(3, c.size());

    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_String, c.get_type(i));

    CHECK_EQUAL("dd", c.get_string(0));
    CHECK_EQUAL("", c.get_string(1));
    CHECK_EQUAL("eeeeeeeee", c.get_string(2));

    c.destroy();
}


TEST(MixedColumn_Binary)
{
    ref_type ref = MixedColumn::create(Allocator::get_default());
    MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

    c.insert_binary(0, BinaryData("aaa", 4));
    c.insert_binary(1, BinaryData("bbbbb", 6));
    c.insert_binary(2, BinaryData("ccccccc", 8));
    CHECK_EQUAL(3, c.size());

    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_Binary, c.get_type(i));

    CHECK_EQUAL("aaa", c.get_binary(0).data());
    CHECK_EQUAL("bbbbb", c.get_binary(1).data());
    CHECK_EQUAL("ccccccc", c.get_binary(2).data());

    c.set_binary(0, BinaryData("dd", 3));
    c.set_binary(1, BinaryData("", 1));
    c.set_binary(2, BinaryData("eeeeeeeee", 10));
    CHECK_EQUAL(3, c.size());

    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_Binary, c.get_type(i));

    CHECK_EQUAL("dd", c.get_binary(0).data());
    CHECK_EQUAL("", c.get_binary(1).data());
    CHECK_EQUAL("eeeeeeeee", c.get_binary(2).data());

    c.destroy();
}


TEST(MixedColumn_Table)
{
    ref_type ref = MixedColumn::create(Allocator::get_default());
    MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

    c.insert_subtable(0, nullptr);
    c.insert_subtable(1, nullptr);
    CHECK_EQUAL(2, c.size());

    for (size_t i = 0; i < c.size(); ++i)
        CHECK_EQUAL(type_Table, c.get_type(i));

    TableRef t1(c.get_subtable_tableref(0));
    TableRef t2(c.get_subtable_tableref(1));
    CHECK(t1->is_empty());
    CHECK(t2->is_empty());

    c.destroy();
}


TEST(MixedColumn_Mixed)
{
    ref_type ref = MixedColumn::create(Allocator::get_default());
    MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

    // Insert mixed types
    c.insert_int(0, 23);
    c.insert_bool(0, false);
    c.insert_olddatetime(0, 23423);
    c.insert_string(0, "Hello");
    c.insert_binary(0, BinaryData("binary"));
    c.insert_subtable(0, nullptr);
    c.insert_float(0, 1.124f);
    c.insert_double(0, 1234.124);
    c.insert_timestamp(0, Timestamp(111, 222));
    CHECK_EQUAL(9, c.size());

    // Check types
    CHECK_EQUAL(type_Timestamp, c.get_type(0));
    CHECK_EQUAL(type_Double, c.get_type(1));
    CHECK_EQUAL(type_Float, c.get_type(2));
    CHECK_EQUAL(type_Table, c.get_type(3));
    CHECK_EQUAL(type_Binary, c.get_type(4));
    CHECK_EQUAL(type_String, c.get_type(5));
    CHECK_EQUAL(type_OldDateTime, c.get_type(6));
    CHECK_EQUAL(type_Bool, c.get_type(7));
    CHECK_EQUAL(type_Int, c.get_type(8));

    // Check values
    CHECK_EQUAL(c.get_int(8), 23);
    CHECK_EQUAL(c.get_bool(7), false);
    CHECK_EQUAL(c.get_olddatetime(6), 23423);
    CHECK_EQUAL(c.get_string(5), "Hello");
    CHECK_EQUAL(c.get_binary(4), BinaryData("binary"));
    CHECK_EQUAL(c.get_float(2), 1.124f);
    CHECK_EQUAL(c.get_double(1), 1234.124);
    CHECK(c.get_timestamp(0) == Timestamp(111, 222));

    // Change all entries to new types
    c.set_int(0, 23);
    c.set_bool(1, false);
    c.set_olddatetime(2, 23423);
    c.set_string(3, "Hello");
    c.set_binary(4, BinaryData("binary"));
    c.set_subtable(5, nullptr);
    c.set_float(6, 1.124f);
    c.set_double(7, 1234.124);
    c.set_timestamp(8, Timestamp{});
    CHECK_EQUAL(9, c.size());

    CHECK_EQUAL(type_Timestamp, c.get_type(8));
    CHECK_EQUAL(type_Double, c.get_type(7));
    CHECK_EQUAL(type_Float, c.get_type(6));
    CHECK_EQUAL(type_Table, c.get_type(5));
    CHECK_EQUAL(type_Binary, c.get_type(4));
    CHECK_EQUAL(type_String, c.get_type(3));
    CHECK_EQUAL(type_OldDateTime, c.get_type(2));
    CHECK_EQUAL(type_Bool, c.get_type(1));
    CHECK_EQUAL(type_Int, c.get_type(0));

    c.destroy();
}


TEST(MixedColumn_SubtableSize)
{
    ref_type ref = MixedColumn::create(Allocator::get_default());
    MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

    c.insert_subtable(0, nullptr);
    c.insert_subtable(1, nullptr);
    c.insert_subtable(2, nullptr);
    c.insert_subtable(3, nullptr);
    c.insert_subtable(4, nullptr);

    // No table instantiated yet (zero ref)
    CHECK_EQUAL(0, c.get_subtable_size(0));

    {
        // Empty table (no columns)
        TableRef t1 = c.get_subtable_tableref(1);
        CHECK(t1->is_empty());
        CHECK_EQUAL(0, c.get_subtable_size(1));
    }

    {
        // Empty table (1 column, no rows)
        TableRef t2 = c.get_subtable_tableref(2);
        CHECK(t2->is_empty());
        t2->add_column(type_Int, "col1");
        CHECK_EQUAL(0, c.get_subtable_size(2));
    }

    {
        // Table with rows
        TableRef t3 = c.get_subtable_tableref(3);
        CHECK(t3->is_empty());
        t3->add_column(type_Int, "col1");
        t3->add_empty_row(10);
        CHECK_EQUAL(10, c.get_subtable_size(3));
    }

    {
        // Table with mixed column first
        TableRef t4 = c.get_subtable_tableref(4);
        CHECK(t4->is_empty());
        t4->add_column(type_Mixed, "col1");
        t4->add_empty_row(10);
        // FAILS because it tries to manually get size from first column
        // which is topped by a node with two subentries.
        CHECK_EQUAL(10, c.get_subtable_size(4));
    }

    c.destroy();
}


TEST(MixedColumn_WriteLeak)
{
    class NullBuffer : public std::streambuf {
    public:
        int overflow(int c)
        {
            return c;
        }
    };

    NullBuffer null_buffer;
    std::ostream null_stream(&null_buffer);
    _impl::OutputStream out(null_stream);

    ref_type ref = MixedColumn::create(Allocator::get_default());
    MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

    c.insert_subtable(0, nullptr);
    c.insert_subtable(1, nullptr);

    c.write(0, 2, 2, out);

    c.destroy();
}

TEST(MixedColumn_SwapRows)
{
    auto epsilon = std::numeric_limits<float>::epsilon();

    // Normal case
    {
        ref_type ref = MixedColumn::create(Allocator::get_default());
        MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

        c.insert_bool(0, false);
        c.insert_string(1, "a");
        c.insert_float(2, 391.931f);
        c.insert_binary(3, BinaryData("foo"));

        c.swap_rows(1, 2);

        CHECK_EQUAL(type_Float, c.get_type(1));
        CHECK_APPROXIMATELY_EQUAL(c.get_float(1), 391.931, epsilon);
        CHECK_EQUAL(type_String, c.get_type(2));
        CHECK_EQUAL(c.get_string(2), "a");
        CHECK_EQUAL(c.size(), 4);

        c.destroy();
    }

    // First two elements
    {
        ref_type ref = MixedColumn::create(Allocator::get_default());
        MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

        c.insert_bool(0, false);
        c.insert_string(1, "a");
        c.insert_float(2, 391.931f);

        c.swap_rows(0, 1);

        CHECK_EQUAL(type_String, c.get_type(0));
        CHECK_EQUAL(c.get_string(0), "a");
        CHECK_EQUAL(type_Bool, c.get_type(1));
        CHECK_EQUAL(c.get_bool(1), false);
        CHECK_EQUAL(c.size(), 3); // size should not change

        c.destroy();
    }

    // Last two elements
    {
        ref_type ref = MixedColumn::create(Allocator::get_default());
        MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

        c.insert_bool(0, false);
        c.insert_string(1, "a");
        c.insert_float(2, 391.931f);

        c.swap_rows(1, 2);

        CHECK_EQUAL(type_Float, c.get_type(1));
        CHECK_APPROXIMATELY_EQUAL(c.get_float(1), 391.931, epsilon);
        CHECK_EQUAL(type_String, c.get_type(2));
        CHECK_EQUAL(c.get_string(2), "a");
        CHECK_EQUAL(c.size(), 3); // size should not change

        c.destroy();
    }

    // Indices in wrong order
    {
        ref_type ref = MixedColumn::create(Allocator::get_default());
        MixedColumn c(Allocator::get_default(), ref, nullptr, 0);

        c.insert_bool(0, false);
        c.insert_string(1, "a");
        c.insert_float(2, 391.931f);

        c.swap_rows(2, 1);

        CHECK_EQUAL(type_Float, c.get_type(1));
        CHECK_APPROXIMATELY_EQUAL(c.get_float(1), 391.931, epsilon);
        CHECK_EQUAL(type_String, c.get_type(2));
        CHECK_EQUAL(c.get_string(2), "a");
        CHECK_EQUAL(c.size(), 3); // size should not change

        c.destroy();
    }
}

TEST(Mixed_CompareRawTypes)
{
    Mixed m;
    {
        int64_t equal = 0;
        int64_t not_equal = 1;
        m.set_int(equal);
        CHECK(m == equal);
        CHECK(m != not_equal);
        CHECK(equal == m);
        CHECK(not_equal != m);
    }
    {
        bool equal = false;
        bool not_equal = true;
        m.set_bool(equal);
        CHECK(m == equal);
        CHECK(m != not_equal);
        CHECK(equal == m);
        CHECK(not_equal != m);
    }
    {
        float equal = 1.1f;
        float not_equal = 1.11f;
        m.set_float(equal);
        CHECK(m == equal);
        CHECK(m != not_equal);
        CHECK(equal == m);
        CHECK(not_equal != m);
    }
    {
        double equal = 2.77;
        double not_equal = 2.89;
        m.set_double(equal);
        CHECK(m == equal);
        CHECK(m != not_equal);
        CHECK(equal == m);
        CHECK(not_equal != m);
    }
    {
        char equal_chars [] = { "hello world" };
        char not_equal_chars [] = { "hello" };
        StringData equal(equal_chars);
        StringData not_equal(not_equal_chars);
        m.set_string(equal);
        // StringData
        CHECK(m == equal);
        CHECK(m != not_equal);
        CHECK(equal == m);
        CHECK(not_equal != m);
        // char *
        CHECK(m == equal_chars);
        CHECK(m != not_equal_chars);
        CHECK(equal_chars == m);
        CHECK(not_equal_chars != m);
        // const char *
        const char* const_equal = equal_chars;
        const char* const_not_equal = not_equal_chars;
        CHECK(m == const_equal);
        CHECK(m != const_not_equal);
        CHECK(const_equal == m);
        CHECK(const_not_equal != m);
    }
    {
        BinaryData equal("data");
        BinaryData not_equal("beta");
        m.set_binary(equal);
        CHECK(m == equal);
        CHECK(m != not_equal);
        CHECK(equal == m);
        CHECK(not_equal != m);
    }
    {
        OldDateTime equal(26);
        OldDateTime not_equal(27);
        m.set_olddatetime(equal);
        CHECK(m == equal);
        CHECK(m != not_equal);
        CHECK(equal == m);
        CHECK(not_equal != m);
    }
    {
        Timestamp equal(23, 24);
        Timestamp not_equal(23, 25);
        m.set_timestamp(equal);
        CHECK(m == equal);
        CHECK(m != not_equal);
        CHECK(equal == m);
        CHECK(not_equal != m);
    }
}


#endif // TEST_COLUMN_MIXED
