#include <catch.hpp>

#define ENABLE_MEMORY_PROFILING
#include "utils/instance_manager.h"

/*! \brief Struct for testing InstanceManager only!
 *  \private */
typedef struct {
    const int a;    //!< Dummy field a
    unsigned b;     //!< Dummy field b
} TestStruct_t;

TEST_CASE("InstanceManager tests", "[InstanceManager]") {
    InstanceManager<TestStruct_t, 4> im;
    TestStruct_t s1 = {1, 11};
    TestStruct_t s2 = {2, 12};
    TestStruct_t s3 = {3, 13};
    TestStruct_t s4 = {4, 14};
    TestStruct_t s5 = {5, 15};

    TestStruct_t* p_s1;
    TestStruct_t* p_s2;
    TestStruct_t* p_s3;
    TestStruct_t* p_s4;
    TestStruct_t* p_s5;

    // Allocate and free some instances
    REQUIRE(im.numEntriesFilled == 0);
    p_s1 = im.alloc(s1);
    REQUIRE(im.numEntriesFilled == 1);
    p_s2 = im.alloc(s2);
    REQUIRE(im.numEntriesFilled == 2);
    p_s3 = im.alloc(s3);
    REQUIRE(im.numEntriesFilled == 3);
    p_s4 = im.alloc(s4);
    REQUIRE(im.numEntriesFilled == 4);
    p_s5 = im.alloc(s5);
    // Overflow
    REQUIRE(p_s5 == nullptr);
    REQUIRE(im.numEntriesFilled == 4);
    im.free(p_s3);
    REQUIRE(im.numEntriesFilled == 3);
    p_s5 = im.alloc(s5);
    REQUIRE(im.numEntriesFilled == 4);

    // Loop through the instances and check contents
    im.resetIndex();
    TestStruct_t* p_inst = im.getNextInstance();
    REQUIRE(p_inst == p_s1);
    REQUIRE(p_inst->a == p_s1->a);
    REQUIRE(p_inst->b == p_s1->b);
    p_inst = im.getNextInstance();
    REQUIRE(p_inst == p_s2);
    REQUIRE(p_inst->a == p_s2->a);
    REQUIRE(p_inst->b == p_s2->b);
    p_inst = im.getNextInstance();
    REQUIRE(p_inst == p_s5);
    REQUIRE(p_inst->a == p_s5->a);
    REQUIRE(p_inst->b == p_s5->b);
    p_inst = im.getNextInstance();
    REQUIRE(p_inst == p_s4);
    REQUIRE(p_inst->a == p_s4->a);
    REQUIRE(p_inst->b == p_s4->b);
    p_inst = im.getNextInstance();
    REQUIRE(p_inst == nullptr);
    // Test fetching instance that's out of bounds repeatedly
    p_inst = im.getNextInstance();
    REQUIRE(p_inst == nullptr);
}

TEST_CASE("InstanceManager invalid freeing", "[InstanceManager]") {
    InstanceManager<TestStruct_t, 4> im;
    TestStruct_t s1 = {1, 11};
    TestStruct_t s2 = {2, 12};
    TestStruct_t* p_s1;
    TestStruct_t* p_s2;

    // Allocate and free some instances
    REQUIRE(im.numEntriesFilled == 0);
    p_s1 = im.alloc(s1);
    REQUIRE(im.numEntriesFilled == 1);
    p_s2 = im.alloc(s2);
    REQUIRE(im.numEntriesFilled == 2);
    // Free unaligned address
    im.free((TestStruct_t*) (((uint8_t*) p_s1) + 1));
    REQUIRE(im.numEntriesFilled == 2);
    // Free nullptr
    im.free((TestStruct_t*) nullptr);
    REQUIRE(im.numEntriesFilled == 2);
    // Free an instance twice
    im.free(p_s1);
    REQUIRE(im.numEntriesFilled == 1);
    im.free(p_s1);
    REQUIRE(im.numEntriesFilled == 1);
    im.free(p_s2);
    REQUIRE(im.numEntriesFilled == 0);
    im.free(p_s2);
    REQUIRE(im.numEntriesFilled == 0);
}

TEST_CASE("InstanceManager fetch instances with gaps", "[InstanceManager]") {
    InstanceManager<TestStruct_t, 4> im;
    TestStruct_t s1 = {1, 11};
    TestStruct_t s2 = {2, 12};
    TestStruct_t s3 = {3, 13};
    TestStruct_t s4 = {4, 14};
    TestStruct_t s5 = {5, 15};
    TestStruct_t s6 = {6, 16};

    TestStruct_t* p_s1;
    TestStruct_t* p_s2;
    TestStruct_t* p_s3;
    TestStruct_t* p_s4;
    TestStruct_t* p_s6;

    // Allocate and free some instances
    p_s1 = im.alloc(s1);
    p_s2 = im.alloc(s2);
    p_s3 = im.alloc(s3);
    p_s4 = im.alloc(s4);
    im.alloc(s5);
    im.free(p_s3);
    im.free(p_s2);
    // Only p1 and p4 should be allocated now, check them:
    im.resetIndex();
    TestStruct_t* p_inst = im.getNextInstance();
    REQUIRE(p_inst == p_s1);
    REQUIRE(p_inst->a == p_s1->a);
    REQUIRE(p_inst->b == p_s1->b);
    p_inst = im.getNextInstance();
    REQUIRE(p_inst == p_s4);
    REQUIRE(p_inst->a == p_s4->a);
    REQUIRE(p_inst->b == p_s4->b);
    p_inst = im.getNextInstance();
    REQUIRE(p_inst == nullptr);
    // Add one (spot 2) and remove the last one and check again
    p_s6 = im.alloc(s6);
    im.free(p_s4);
    im.resetIndex();
    p_inst = im.getNextInstance();
    REQUIRE(p_inst == p_s1);
    REQUIRE(p_inst->a == p_s1->a);
    REQUIRE(p_inst->b == p_s1->b);
    im.free(p_s1); // Free inbetween
    p_inst = im.getNextInstance();
    REQUIRE(p_inst == p_s6);
    REQUIRE(p_inst->a == p_s6->a);
    REQUIRE(p_inst->b == p_s6->b);
    p_inst = im.getNextInstance();
    REQUIRE(p_inst == nullptr);
}
