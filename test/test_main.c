#include "unity.h"
#include "main.h"
#include "mouse_wrap.h"
void setUp(void)
{
    // Set up code here (if needed)
}

void tearDown(void)
{
    // Tear down code here (if needed)
}

void test_SampleFunction(void)
{
    // Example test
    TEST_ASSERT_EQUAL(1, 1); // Replace with meaningful assertions
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_SampleFunction);

    return UNITY_END();
}