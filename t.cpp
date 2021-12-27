#include <type_traits>
#include <stdio.h>
#include <stdint.h>

namespace Pts
{
    class Identifier
    {
    protected:
        constexpr Identifier() :val(0) {}
        constexpr Identifier(int x) : val(x) {}
        const uint32_t val;
    public:
        constexpr uint32_t operator+() const { return val; }
        //constexpr operator int() const { return val; }
        //constexpr operator long() const { return val; }
        constexpr operator uint32_t() const { return val; }
    };

    class InputId_T : public Identifier
    {
    public:
        constexpr InputId_T() :Identifier() {}
        constexpr InputId_T(int x) : Identifier(x) {}

    };
    class OutputId_T: public Identifier
    {
    public:
        constexpr OutputId_T() :Identifier() {}
        constexpr OutputId_T(int x) : Identifier(x) {}
    };
}
class Foo
{
public:
    static void test(Pts::InputId_T x)
    {
        printf("Input %d\n", +x );
    }
    static void test(Pts::OutputId_T x)
    {
        printf("Output %d\n", +x );
    }
    static void test(Pts::Identifier x)
    {
        printf("Identifier %d\n", +x );
    }
};

constexpr Pts::InputId_T  PT1(0);
constexpr Pts::InputId_T  PT11(1);
constexpr Pts::OutputId_T PT2(2);

static_assert(! std::is_same<decltype(PT1), decltype(PT2)>::value, "no type difference");
typedef struct
{
    Pts::InputId_T in1;
    Pts::InputId_T in2;
    Pts::OutputId_T ou1;
} MyPts_T;


const char* strings[3] = { "p1", "p2", "p3" };

    
class Chassis
{
public:
    const char** m_list;
    Chassis(const char** strings) 
        : m_list(strings)
    {
    }

    const char* operator() (const Pts::InputId_T idx) { return m_list[idx]; }
    const char* operator() (const Pts::OutputId_T idx) { return "hello"; }
};
Chassis chassis(strings);
//MyPts_T list;

int main()
{
    Foo::test(PT1);
    Foo::test(PT2);
    printf("chassis p1=%s, p2=%s, p=%s\n", chassis(PT2), chassis(PT11), chassis(PT1));
    return 0;
}
