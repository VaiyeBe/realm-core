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
#ifdef TEST_ARRAY_STRING_LONG

#include <vector>

#include <realm/array_string_long.hpp>
#include "test.hpp"

using namespace realm;
using namespace realm::util;
using namespace realm::test_util;

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


namespace {

struct nullable {
    static constexpr bool value = true;
};

struct non_nullable {
    static constexpr bool value = false;
};

} // anonymous namespace


TEST_TYPES(ArrayStringLong_Basic, non_nullable, nullable)
{
    constexpr bool nullable = TEST_TYPE::value;

    ArrayStringLong c(Allocator::get_default(), nullable);
    c.create();

    // TEST(ArrayStringLong_MultiEmpty)

    c.add("");
    c.add("");
    c.add("");
    c.add("");
    c.add("");
    c.add("");
    CHECK_EQUAL(6, c.size());

    CHECK_EQUAL("", c.get(0));
    CHECK_EQUAL("", c.get(1));
    CHECK_EQUAL("", c.get(2));
    CHECK_EQUAL("", c.get(3));
    CHECK_EQUAL("", c.get(4));
    CHECK_EQUAL("", c.get(5));


    // TEST(ArrayStringLong_Set)

    c.set(0, "hey");

    CHECK_EQUAL(6, c.size());
    CHECK_EQUAL("hey", c.get(0));
    CHECK_EQUAL("", c.get(1));
    CHECK_EQUAL("", c.get(2));
    CHECK_EQUAL("", c.get(3));
    CHECK_EQUAL("", c.get(4));
    CHECK_EQUAL("", c.get(5));


    // TEST(ArrayStringLong_Add)

    c.clear();

    CHECK_EQUAL(0, c.size());

    c.add("abc");
    CHECK_EQUAL("abc", c.get(0)); // single
    CHECK_EQUAL(1, c.size());

    c.add("defg"); // non-empty
    CHECK_EQUAL("abc", c.get(0));
    CHECK_EQUAL("defg", c.get(1));
    CHECK_EQUAL(2, c.size());


    // TEST(ArrayStringLong_Set2)

    // {shrink, grow} x {first, middle, last, single}
    c.clear();

    c.add("abc");
    c.set(0, "de"); // shrink single
    CHECK_EQUAL("de", c.get(0));
    CHECK_EQUAL(1, c.size());

    c.set(0, "abcd"); // grow single
    CHECK_EQUAL("abcd", c.get(0));
    CHECK_EQUAL(1, c.size());

    c.add("efg");
    CHECK_EQUAL("abcd", c.get(0));
    CHECK_EQUAL("efg", c.get(1));
    CHECK_EQUAL(2, c.size());

    c.set(1, "hi"); // shrink last
    CHECK_EQUAL("abcd", c.get(0));
    CHECK_EQUAL("hi", c.get(1));
    CHECK_EQUAL(2, c.size());

    c.set(1, "jklmno"); // grow last
    CHECK_EQUAL("abcd", c.get(0));
    CHECK_EQUAL("jklmno", c.get(1));
    CHECK_EQUAL(2, c.size());

    c.add("pq");
    c.set(1, "efghijkl"); // grow middle
    CHECK_EQUAL("abcd", c.get(0));
    CHECK_EQUAL("efghijkl", c.get(1));
    CHECK_EQUAL("pq", c.get(2));
    CHECK_EQUAL(3, c.size());

    c.set(1, "x"); // shrink middle
    CHECK_EQUAL("abcd", c.get(0));
    CHECK_EQUAL("x", c.get(1));
    CHECK_EQUAL("pq", c.get(2));
    CHECK_EQUAL(3, c.size());

    c.set(0, "qwertyuio"); // grow first
    CHECK_EQUAL("qwertyuio", c.get(0));
    CHECK_EQUAL("x", c.get(1));
    CHECK_EQUAL("pq", c.get(2));
    CHECK_EQUAL(3, c.size());

    c.set(0, "mno"); // shrink first
    CHECK_EQUAL("mno", c.get(0));
    CHECK_EQUAL("x", c.get(1));
    CHECK_EQUAL("pq", c.get(2));
    CHECK_EQUAL(3, c.size());


    // TEST(ArrayStringLong_Insert)

    c.clear();

    c.insert(0, "abc"); // single
    CHECK_EQUAL(c.get(0), "abc");
    CHECK_EQUAL(1, c.size());

    c.insert(1, "d"); // end
    CHECK_EQUAL("abc", c.get(0));
    CHECK_EQUAL("d", c.get(1));
    CHECK_EQUAL(2, c.size());

    c.insert(2, "ef"); // end
    CHECK_EQUAL("abc", c.get(0));
    CHECK_EQUAL("d", c.get(1));
    CHECK_EQUAL("ef", c.get(2));
    CHECK_EQUAL(3, c.size());

    c.insert(1, "ghij"); // middle
    CHECK_EQUAL("abc", c.get(0));
    CHECK_EQUAL("ghij", c.get(1));
    CHECK_EQUAL("d", c.get(2));
    CHECK_EQUAL("ef", c.get(3));
    CHECK_EQUAL(4, c.size());

    c.insert(0, "klmno"); // first
    CHECK_EQUAL("klmno", c.get(0));
    CHECK_EQUAL("abc", c.get(1));
    CHECK_EQUAL("ghij", c.get(2));
    CHECK_EQUAL("d", c.get(3));
    CHECK_EQUAL("ef", c.get(4));
    CHECK_EQUAL(5, c.size());


    // TEST(ArrayStringLong_Delete)

    c.clear();

    c.add("a");
    c.add("bc");
    c.add("def");
    c.add("ghij");
    c.add("klmno");

    c.erase(0); // first
    CHECK_EQUAL("bc", c.get(0));
    CHECK_EQUAL("def", c.get(1));
    CHECK_EQUAL("ghij", c.get(2));
    CHECK_EQUAL("klmno", c.get(3));
    CHECK_EQUAL(4, c.size());

    c.erase(3); // last
    CHECK_EQUAL("bc", c.get(0));
    CHECK_EQUAL("def", c.get(1));
    CHECK_EQUAL("ghij", c.get(2));
    CHECK_EQUAL(3, c.size());

    c.erase(1); // middle
    CHECK_EQUAL("bc", c.get(0));
    CHECK_EQUAL("ghij", c.get(1));
    CHECK_EQUAL(2, c.size());

    c.erase(0); // single
    CHECK_EQUAL("ghij", c.get(0));
    CHECK_EQUAL(1, c.size());

    c.erase(0); // all
    CHECK_EQUAL(0, c.size());
    CHECK(c.is_empty());


    // TEST(ArrayStringLong_Find)

    c.clear();

    c.add("a");
    c.add("bc iu");
    c.add("def");
    c.add("ghij uihi i ih iu huih ui");
    c.add("klmno hiuh iuh uih i huih i biuhui");

    CHECK_EQUAL(size_t(-1), c.find_first(""));

    CHECK_EQUAL(size_t(-1), c.find_first("xlmno hiuh iuh uih i huih i biuhui"));

    CHECK_EQUAL(3, c.find_first("ghij uihi i ih iu huih ui"));


    // TEST(ArrayStringLong_Count)

    c.clear();

    // first, middle and end
    c.add("foobar");
    c.add("bar abc");
    c.add("foobar");
    c.add("baz");
    c.add("foobar");

    CHECK_EQUAL(3, c.count("foobar"));


    // TEST(ArrayStringLong_Destroy)

    c.destroy();
}


TEST(ArrayStringLong_Null)
{

    {
        ArrayStringLong a(Allocator::get_default(), true);
        a.create();

        a.add("foo");
        a.add("");
        a.add(realm::null());

        CHECK_EQUAL(a.is_null(0), false);
        CHECK_EQUAL(a.is_null(1), false);
        CHECK_EQUAL(a.is_null(2), true);
        CHECK(a.get(0) == "foo");

        // Test set
        a.set_null(0);
        a.set_null(1);
        a.set_null(2);
        CHECK_EQUAL(a.is_null(1), true);
        CHECK_EQUAL(a.is_null(0), true);
        CHECK_EQUAL(a.is_null(2), true);

        a.destroy();
    }

    {
        ArrayStringLong a(Allocator::get_default(), true);
        a.create();

        a.add(realm::null());
        a.add("");
        a.add("foo");

        CHECK_EQUAL(a.is_null(0), true);
        CHECK_EQUAL(a.is_null(1), false);
        CHECK_EQUAL(a.is_null(2), false);
        CHECK(a.get(2) == "foo");

        // Test insert
        a.insert(0, realm::null());
        a.insert(2, realm::null());
        a.insert(4, realm::null());

        CHECK_EQUAL(a.is_null(0), true);
        CHECK_EQUAL(a.is_null(1), true);
        CHECK_EQUAL(a.is_null(2), true);
        CHECK_EQUAL(a.is_null(3), false);
        CHECK_EQUAL(a.is_null(4), true);
        CHECK_EQUAL(a.is_null(5), false);

        a.destroy();
    }

    {
        ArrayStringLong a(Allocator::get_default(), true);
        a.create();

        a.add("");
        a.add(realm::null());
        a.add("foo");

        CHECK_EQUAL(a.is_null(0), false);
        CHECK_EQUAL(a.is_null(1), true);
        CHECK_EQUAL(a.is_null(2), false);
        CHECK(a.get(2) == "foo");


        a.erase(0);
        CHECK_EQUAL(a.is_null(0), true);
        CHECK_EQUAL(a.is_null(1), false);

        a.erase(0);
        CHECK_EQUAL(a.is_null(0), false);

        a.destroy();
    }

    {
        ArrayStringLong a(Allocator::get_default(), false);
        a.create();

        a.add("");
        a.add("foo");

        CHECK_EQUAL(a.is_null(0), false);
        CHECK_EQUAL(a.get(0), "");
        CHECK_EQUAL(a.is_null(1), false);

        a.destroy();
    }

    {
        ArrayStringLong a(Allocator::get_default(), false);
        a.create();

        a.add("");

        CHECK_EQUAL(a.is_null(0), false);
        CHECK_EQUAL(a.get(0), "");

        a.destroy();
    }

    Random random(random_int<unsigned long>());

    for (size_t t = 0; t < 2; t++) {
        ArrayStringLong a(Allocator::get_default(), true);
        a.create();

        // vector that is kept in sync with the ArrayString so that we can compare with it
        std::vector<std::string> v;

        for (size_t i = 0; i < 2000; i++) {
            unsigned char rnd = static_cast<unsigned char>(
                random.draw_int<unsigned int>()); //    = 1234 * ((i + 123) * (t + 432) + 423) + 543;

            // Add more often than removing, so that we grow
            if (rnd < 80 && a.size() > 0) {
                size_t del = rnd % a.size();
                a.erase(del);
                v.erase(v.begin() + del);
            }
            else {
                // Generate string with good probability of being empty or realm::null()
                static const char str[] = "This is a test of realm::null() strings";
                size_t len;

                if (random.draw_int<int>() > 100)
                    len = rnd % 15;
                else
                    len = 0;

                StringData sd;
                std::string stdstr;

                if (random.draw_int<int>() > 100) {
                    sd = realm::null();
                    stdstr = "realm::null()";
                }
                else {
                    sd = StringData(str, len);
                    stdstr = std::string(str, len);
                }

                if (random.draw_int<int>() > 100) {
                    a.add(sd);
                    v.push_back(stdstr);
                }
                else if (a.size() > 0) {
                    size_t pos = rnd % a.size();
                    a.insert(pos, sd);
                    v.insert(v.begin() + pos, stdstr);
                }

                CHECK_EQUAL(a.size(), v.size());
                for (size_t a_i = 0; a_i < a.size(); a_i++) {
                    if (v[a_i] == "realm::null()") {
                        CHECK(a.is_null(a_i));
                        CHECK(a.get(a_i).data() == nullptr);
                    }
                    else {
                        CHECK(a.get(a_i) == v[a_i]);
                    }
                }
            }
        }
        a.destroy();
    }
}

#endif // TEST_ARRAY_STRING_LONG
