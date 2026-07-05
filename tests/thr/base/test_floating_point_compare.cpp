/**
 * @file test_floating_point_compare.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief `thr::no_nan_inf` 和 `thr::has_nan_inf` 的测试用例和使用示例。
 * @version 0.1.0-1
 * @date 2026-05-02
 * 
 * @copyright cpp-love
 * 
 */

#undef NDEBUG
#include "thr/base/assert_msg.hpp"
#include "thr/base/floating_point_compare.hpp"

int main() {

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    thr::no_nan_inf_f  value1{0.f};
    thr::no_nan_inf_f  value2{0.0001f};
    thr::no_nan_inf_f  value3{-0.0001f};
    thr::no_nan_inf_f  value4{-0.01f};
    thr::has_nan_inf_f value5{std::numeric_limits<float>::quiet_NaN()};
    thr::has_nan_inf_f value6{std::numeric_limits<float>::infinity()};
    // THR_ASSERT_MSG(thr::no_nan_inf_f{value5} != thr::no_nan_inf_f{value6}); // 不可以
    // THR_ASSERT_MSG(value1 != thr::no_nan_inf_f{value6});                     // 不可以
    THR_ASSERT_MSG(value1 == value2);
    THR_ASSERT_MSG(value1 == value3);
    THR_ASSERT_MSG(value4 != value3);
    THR_ASSERT_MSG(value4 < value1);
    THR_ASSERT_MSG(thr::has_nan_inf_f(value1) != value5);
    THR_ASSERT_MSG(thr::has_nan_inf_f(value1) != value6);
    THR_ASSERT_MSG(thr::has_nan_inf_f(value1) == thr::has_nan_inf_f(value2));
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

    return 0;
}