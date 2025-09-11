#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <variant>
#include <any>
#include <ranges>
#include <format>
#include <concepts>
#include <filesystem>
#include <coroutine>
#include <span>
#include <source_location>
#include <expected>

auto foo() { return 5; };

template <typename T>
concept Addable = requires(T a, T b) 
{
    { a + b } -> std::convertible_to<T>;
};

template <Addable T>
T add(T a, T b)
{
    return a + b;
};

consteval int multiply(int a, int b) 
{
    return a * b;
};

constinit int arr[] = {1, 2, 3};

int main()
{
    // C++14
    // 1. Aggregate initialization
    struct Point
    {
        int x, y;
    };
    Point p1{1, 2}; // aggregate initialization
    std::cout << "C++14: Aggregate initialization: (" << p1.x << ", " << p1.y << ")" << std::endl;

    // 2. Generic lambdas
    auto sum = [](auto a, auto b)
    { return a + b; };
    std::cout << "C++14: Generic lambda: " << sum(2, 3) << std::endl;

    // 3. decltype(auto)
    decltype(auto) result = foo();
    std::cout << "C++14: decltype(auto): " << result << std::endl;

    // 4. std::make_unique
    auto ptr = std::make_unique<int>(5);
    std::cout << "C++14: std::make_unique: " << *ptr << std::endl;

    // C++17
    // 1. Structured bindings
    std::pair<int, int> p2{1, 2};
    auto [x, y] = p2; // structured binding
    std::cout << "C++17: Structured binding: (" << x << ", " << y << ")" << std::endl;

    // 2. std::optional
    std::optional<int> opt{5};
    if (opt)
    {
        std::cout << "C++17: std::optional: " << *opt << std::endl;
    }

    // 3. std::variant
    std::variant<int, std::string> var{5};
    if (std::holds_alternative<int>(var))
    {
        std::cout << "C++17: std::variant: " << std::get<int>(var) << std::endl;
    }

    // 4. std::any
    std::any any{5};
    if (any.type() == typeid(int))
    {
        std::cout << "C++17: std::any: " << std::any_cast<int>(any) << std::endl;
    }

    // 5. std::string_view
    std::string str = "Hello, World!";
    std::string_view view = str;
    std::cout << "C++17: std::string_view: " << view << std::endl;

    // 6. constexpr if
    constexpr bool flag = true;
    if constexpr (flag)
    {
        std::cout << "C++17: constexpr if: True" << std::endl;
    }

    // 7. std::filesystem
    std::filesystem::path path = "/path/to/file";
    std::cout << "C++17: std::filesystem: " << path.filename() << std::endl;

    // C++20
    // 1. Concepts
    std::cout << "C++20: Concepts: " << add(2, 3) << std::endl;

    // 2. Ranges
    std::vector<int> vec{1, 2, 3, 4, 5};
    for (auto i : vec | std::views::filter([](int x)
                                           { return x % 2 == 0; }))
    {
        std::cout << "C++20: Ranges: " << i << std::endl;
    }

    // 3. std::span
    std::span<int> span { vec.begin() + 2, 3 };
    for (auto i : span)
    {
        std::cout << "C++20: std::span: " << i << std::endl;
    }

    // 4. std::format
    std::string formatted = std::format("C++20: std::format: {}", 42);
    std::cout << formatted << std::endl;

    // 5. std::source_location
    std::source_location loc = std::source_location::current();
    std::cout << "C++20: std::source_location: " << loc.file_name() << ":" << loc.line() << std::endl;

    // 6. Likely/Unlikely attributes
    int data[] = {1, 2, 3, 4, 5, 0, 6, 7, 8, 9};
    int size = sizeof(data) / sizeof(data[0]);
    for (int i = 0; i < size; i++) 
    {
        if (data[i] != 0) [[likely]]
        {
            // process non-zero data
            std::cout << "C++20: (Likely Case) Processing non-zero data: " << data[i] << std::endl;
        } 
        else [[unlikely]]
        {
            // handle zero data (unlikely case)
            std::cout << "C++20: (Unlikely Case) Handling zero data" << std::endl;
        }
    }

    // 7. std::bit_cast
    int value = 42;
    float float_value = std::bit_cast<float>(value);
    std::cout << "C++20: std::bit_cast: " << float_value << std::endl;

    // 8. consteval
    constexpr int mul = multiply(2, 3);
    static_assert(mul == 6);
    std::cout << "C++20: consteval: " << mul << std::endl;

    // C++23
    // 1. std::expected
    std::expected<int, std::string> res = std::unexpected("no result");
    if (res)
    {
        std::cout << "C++23: std::expected: " << *res << std::endl;
    }
    else
    {
        std::cout << "C++23: std::expected: " << res.error() << std::endl;
    }

    // 2. std::hardware_destructive_interference_size (cache line size)
    std::cout << "C++23: std::hardware_destructive_interference_size: " << std::hardware_destructive_interference_size << std::endl;

    return 0;
}