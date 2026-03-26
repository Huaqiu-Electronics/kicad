#include "get_pcb_thickness.h"
#include <iostream>
#include <vector>
#include <iomanip>
struct TestCase
{
    int         layer{};
    double      input{};
    std::string expected{};
    std::string description{};
};

inline auto test_cases()
{
    std::vector<TestCase> cases = { // --- 容差匹配测试 (1.6mm 窗口: 1.44 - 1.76) ---
                                    { 4, 1.6, "1.6", "精确匹配" },
                                    { 4, 1.601, "1.6", "略高于标称值 (10%容差内)" },
                                    { 4, 1.76, "1.6", "容差上限边界" },
                                    { 4, 1.44, "1.6", "容差下限边界" },

                                    // --- 容差匹配测试 (2.0mm 窗口: 1.8 - 2.2) ---
                                    { 4, 1.8, "2.0", "下个档位的容差下限" },
                                    { 4, 2.19, "2.0", "容差上限内" },

                                    // --- 向上对齐逻辑测试 (落在线外) ---
                                    { 4, 1.35, "1.6", "1.2的上限是1.32, 1.35超出容差且大于1.2 -> 取1.6" },
                                    { 4, 0.9, "1.0", "0.8的上限是0.88, 0.9超出容差且大于0.8 -> 取1.0" },

                                    // --- 层数差异测试 ---
                                    { 12, 1.0, "1.6", "12层最小厚度是1.6，输入1.0应向上取到1.6" },
                                    { 6, 0.75, "0.8", "6层最小0.8, 0.75在0.8的10%容差内" },

                                    // --- 边界与异常测试 ---
                                    { 4, 5.0, "2.0", "超出该层最大厚度限制 -> 取最大值" },
                                    { 99, 1.6, "3.2", "无效层数 -> 默认兜底值" }
    };

    int passed = 0;
    std::cout << std::left << std::setw( 10 ) << "Layer" << std::setw( 10 ) << "Input" << std::setw( 12 ) << "Expected"
              << std::setw( 12 ) << "Actual"
              << "Result" << std::endl;
    std::cout << std::string( 60, '-' ) << std::endl;

    for( const auto& tc : cases )
    {
        std::string actual = get_pcb_thickness( tc.layer, tc.input );
        bool        isOk = ( actual == tc.expected );
        if( isOk )
            passed++;

        std::cout << std::left << std::setw( 10 ) << tc.layer << std::setw( 10 ) << tc.input << std::setw( 12 )
                  << tc.expected << std::setw( 12 ) << actual << ( isOk ? "PASS" : "FAIL" ) << " (" << tc.description
                  << ")" << std::endl;
    }

    std::cout << std::string( 60, '-' ) << std::endl;
    std::cout << "Summary: " << passed << "/" << cases.size() << " tests passed." << std::endl;

    return ( passed == cases.size() ) ? 0 : 1;
}
int main()
{
    std::vector<TestCase> cases = {

        // 容差命中
        { 1, 1.44, "1.6" },
        { 1, 1.60, "1.6" },
        { 1, 1.76, "1.6" },
        { 1, 1.601, "1.6" },

        { 1, 1.8, "2.0" },
        { 1, 2.0, "2.0" },
        { 1, 2.2, "2.0" },

        // fallback
        { 1, 1.77, "2.0" },
        { 1, 1.79, "2.0" },
        { 1, 1.11, "1.2" },

        // 小尺寸
        { 1, 0.54, "0.6" },
        { 1, 0.66, "0.6" },
        { 1, 0.67, "0.8" },

        // 超范围
        { 1, 3.5, "2.0" },

        // 多层
        { 6, 1.05, "1.0" },
        { 6, 2.3, "2.5" },
        { 10, 1.7, "1.6" },
        { 10, 2.3, "2.5" },

        // layer 不存在
        { 99, 1.6, "3.2" },
    };

    int pass = 0;
    int fail = 0;

    for( const auto& tc : cases )
    {
        std::string result = get_pcb_thickness( tc.layer, tc.input );

        if( result == tc.expected )
        {
            pass++;
        }
        else
        {
            fail++;
            std::cout << "[FAIL] layer=" << tc.layer << ", input=" << tc.input << ", expected=" << tc.expected
                      << ", got=" << result << std::endl;
        }
    }

    std::cout << "\n=== RESULT ===\n";
    std::cout << "PASS: " << pass << "\n";
    std::cout << "FAIL: " << fail << "\n";

    auto test_result = test_cases();

    return fail == 0 && test_result == 0 ? 0 : 1;
}
