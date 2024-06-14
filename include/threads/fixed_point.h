#include <stdio.h>
#include <stdint.h>

#define FRACTIONAL_BITS 14  // 고정소숫점의 소숫점 이하 비트 수
#define FIXED_ONE (1 << FRACTIONAL_BITS)  // 고정소숫점의 1

typedef int32_t fixed;  // 고정소숫점을 나타내는 자료형

// 고정소숫점 변환 매크로
#define INT_TO_FIXED(n) ((n) << FRACTIONAL_BITS)
#define FIXED_TO_INT(x) ((x) >> FRACTIONAL_BITS)
#define FIXED_TO_INT_ROUND(x) (((x) + (FIXED_ONE >> 1)) >> FRACTIONAL_BITS)

// 고정소숫점 연산 매크로
#define FIXED_ADD(x, y) ((x) + (y))
#define FIXED_SUBTRACT(x, y) ((x) - (y))
#define FIXED_ADD_INT(x, n) ((x) + INT_TO_FIXED(n))
#define FIXED_SUBTRACT_INT(x, n) ((x) - INT_TO_FIXED(n))
#define FIXED_MULTIPLY(x, y) ((fixed)(((int64_t)(x) * (y)) >> FRACTIONAL_BITS))
#define FIXED_MULTIPLY_INT(x, n) ((x) * (n))
#define FIXED_DIVIDE(x, y) ((fixed)(((int64_t)(x) << FRACTIONAL_BITS) / (y)))
#define FIXED_DIVIDE_INT(x, n) ((x) / (n))
