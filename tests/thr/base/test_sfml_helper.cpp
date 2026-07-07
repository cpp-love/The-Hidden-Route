/**
 * @file test_sfml_helper.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief `std::formatter` 格式化模板类对 SFML 类的特化的测试用例与使用示例
 * @version 0.1.0-1
 * @date 2026-05-02
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/base/sfml_helper.hpp"
#include <SFML/System/Vector2.hpp>
#include <print>

int main() {

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    sf::Vector2f      vec1{10, 10};
    sf::Vector2i      vec2{11, 11};
    sf::Vector2<long> vec3{13, 10};
    std::println("vec1 is {:.3f}, vec2 is {:d}, vec3 is {:d}", vec1, vec2, vec3);
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

    return 0;
}