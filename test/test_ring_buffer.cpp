#include <catch.hpp>

#include "utils/common.h"

#include "utils/ring_buffer.h"

TEST_CASE("Ring buffer tests", "[RingBuffer]") {
    RingBuffer<uint32_t, 4> rb;
    uint32_t v1 = 1;
    uint32_t v3 = 3;
    uint32_t v4 = 4;
    uint32_t v5 = 5;
    uint32_t temp = 0xFF;

    REQUIRE(rb.getNumEntriesFilled() == 0);
    REQUIRE(rb.push(v1)   == RingBuffer<uint32_t, 4>::Error::OK);
    REQUIRE(rb.push(2)    == RingBuffer<uint32_t, 4>::Error::OK);
    REQUIRE(rb.push(v3)   == RingBuffer<uint32_t, 4>::Error::OK);
    REQUIRE(rb.getNumEntriesFilled() == 3);
    REQUIRE(rb.push(v4)   == RingBuffer<uint32_t, 4>::Error::OK_LAST);
    REQUIRE(rb.getNumEntriesFilled() == 4);
    REQUIRE(rb.push(v5)   == RingBuffer<uint32_t, 4>::Error::NOK_OVERFLOW);
    REQUIRE(rb.getNumEntriesFilled() == 4);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 4>::Error::OK);
    REQUIRE(temp == v1);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 4>::Error::OK);
    REQUIRE(temp == 2);
    REQUIRE(rb.getNumEntriesFilled() == 2);
    REQUIRE(rb.push(v5)   == RingBuffer<uint32_t, 4>::Error::OK);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 4>::Error::OK);
    REQUIRE(temp == v3);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 4>::Error::OK);
    REQUIRE(temp == v4);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 4>::Error::OK_LAST);
    REQUIRE(rb.getNumEntriesFilled() == 0);
    REQUIRE(temp == v5);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 4>::Error::NOK_EMPTY);
    REQUIRE(rb.getNumEntriesFilled() == 0);
}

TEST_CASE("Ring buffer test 2", "[RingBuffer]") {
    RingBuffer<uint32_t, 3> rb;
    uint32_t temp = 0xFF;

    REQUIRE(rb.push(1)    == RingBuffer<uint32_t, 3>::Error::OK);
    REQUIRE(rb.push(2)    == RingBuffer<uint32_t, 3>::Error::OK);
    REQUIRE(rb.push(3)    == RingBuffer<uint32_t, 3>::Error::OK_LAST);
    REQUIRE(rb.push(4)    == RingBuffer<uint32_t, 3>::Error::NOK_OVERFLOW);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 3>::Error::OK);
    REQUIRE(temp == 1);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 3>::Error::OK);
    REQUIRE(temp == 2);
    REQUIRE(rb.push(4)    == RingBuffer<uint32_t, 3>::Error::OK);
    REQUIRE(rb.push(5)    == RingBuffer<uint32_t, 3>::Error::OK_LAST);
    REQUIRE(rb.push(6)    == RingBuffer<uint32_t, 3>::Error::NOK_OVERFLOW);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 3>::Error::OK);
    REQUIRE(temp == 3);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 3>::Error::OK);
    REQUIRE(temp == 4);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 3>::Error::OK_LAST);
    REQUIRE(temp == 5);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 3>::Error::NOK_EMPTY);
}

TEST_CASE("Ring buffer size 1", "[RingBuffer]") {
    RingBuffer<uint32_t, 1> rb;
    uint32_t temp = 0xFF;

    REQUIRE(rb.push(1)    == RingBuffer<uint32_t, 1>::Error::OK_LAST);
    REQUIRE(rb.push(2)    == RingBuffer<uint32_t, 1>::Error::NOK_OVERFLOW);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 1>::Error::OK_LAST);
    REQUIRE(temp == 1);
    REQUIRE(rb.push(2)    == RingBuffer<uint32_t, 1>::Error::OK_LAST);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 1>::Error::OK_LAST);
    REQUIRE(temp == 2);
    REQUIRE(rb.pop(&temp) == RingBuffer<uint32_t, 1>::Error::NOK_EMPTY);
}
