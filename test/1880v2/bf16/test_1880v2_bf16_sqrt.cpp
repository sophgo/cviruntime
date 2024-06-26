/**
 */
#include "../1880v2_test_util.h"
#define OUT
#define IN
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <cfloat>
//#include <boost/math/special_functions/next.hpp>
//#define DBG

using namespace std;
//TODO: get from ctx
static u32 channel = 32; //<! 1880v2 hardcode

//<! 1880v2 hw config
static u32 table_h = 32;
static u32 table_w = 8;
static u32 table_hw = table_h * table_w;

// fix range
const static int exp_start = -62;
const static int exp_end = 63;
#if 0
static int AlmostEqual(float a, float b, float tolerance)
{
    float absA = fabsf(a);
    float absB = fabsf(b);
    return fabsf(a - b) <= tolerance * fmaxf(absA, absB);
}
static bool AlmostEqual2sComplement(float A, float B, int maxUlps)    
{    
    // Make sure maxUlps is non-negative and small enough that the    
    // default NAN won't compare as equal to anything.    
    assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);    
    int aInt = *(int*)&A;    
    // Make aInt lexicographically ordered as a twos-complement int    
    if (aInt < 0)    
        aInt = 0x80000000 - aInt;    
    // Make bInt lexicographically ordered as a twos-complement int    
    int bInt = *(int*)&B;    
    if (bInt < 0)    
        bInt = 0x80000000 - bInt;    
    int intDiff = abs(aInt - bInt);    
    if (intDiff <= maxUlps)    
        return true;    
    return false;    
}
#endif
// http://www.enseignement.polytechnique.fr/informatique/INF478/docs/Cpp/en/cpp/types/numeric_limits/epsilon.html
template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    almost_equal(T x, T y, int ulp)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) < std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
    // unless the result is subnormal
           || std::abs(x-y) < std::numeric_limits<T>::min();
}
/**
 * pre_data means we test fixed pattern, it should be same sa lut
 */
enum TEST_MODE {
  PRE_DATA_COMPARE_FIX = 0, // pre-data + fix compare
  GEN_POW_20_DATA_MAX_ERROR, //generate 2^-20 ~ 2^20 value that check epsilon
  GEN_POW_20_DATA_MAX_ERROR_U8, //generate 2^-20 ~ 2^20 value that check epsilon, result bf16->u8
  TEST_MODE_MAX,
};

static TEST_MODE mode;

static u16 test_pattern[] = {
  0x0000,
  0x38D2,
  0x3952,
  0x399D,
  0x39D2,
  0x3A03,
  0x3A1D,
  0x3A38,
  0x3A52,
  0x3A6C,
  0x3A83,
  0x3A90,
  0x3A9D,
  0x3AAA,
  0x3AB8,
  0x3AC5,
  0x3AD2,
  0x3ADF,
  0x3AEC,
  0x3AF9,
  0x3B03,
  0x3B0A,
  0x3B10,
  0x3B17,
  0x3B1D,
  0x3B24,
  0x3B2A,
  0x3B31,
  0x3B38,
  0x3B3E,
  0x3B45,
  0x3B4B,
  0x3B52,
  0x3B58,
  0x3B5F,
  0x3B65,
  0x3B6C,
  0x3B72,
  0x3B79,
  0x3B80,
  0x3B83,
  0x3B86,
  0x3B8A,
  0x3B8D,
  0x3B90,
  0x3B93,
  0x3B97,
  0x3B9A,
  0x3B9D,
  0x3BA1,
  0x3BA4,
  0x3BA7,
  0x3BAA,
  0x3BAE,
  0x3BB1,
  0x3BB4,
  0x3BB8,
  0x3BBB,
  0x3BBE,
  0x3BC1,
  0x3BC5,
  0x3BC8,
  0x3BCB,
  0x3BCE,
  0x3BD2,
  0x3BD5,
  0x3BD8,
  0x3BDC,
  0x3BDF,
  0x3BE2,
  0x3BE5,
  0x3BE9,
  0x3BEC,
  0x3BEF,
  0x3BF2,
  0x3BF6,
  0x3BF9,
  0x3BFC,
  0x3C00,
  0x3C01,
  0x3C03,
  0x3C05,
  0x3C06,
  0x3C08,
  0x3C0A,
  0x3C0B,
  0x3C0D,
  0x3C0F,
  0x3C10,
  0x3C12,
  0x3C13,
  0x3C15,
  0x3C17,
  0x3C18,
  0x3C1A,
  0x3C1C,
  0x3C1D,
  0x3C1F,
  0x3C21,
  0x3C22,
  0x3C24,
  0x3C25,
  0x3C27,
  0x3C29,
  0x3C2A,
  0x3C2C,
  0x3C2E,
  0x3C2F,
  0x3C31,
  0x3C33,
  0x3C34,
  0x3C36,
  0x3C38,
  0x3C39,
  0x3C3B,
  0x3C3C,
  0x3C3E,
  0x3C40,
  0x3C41,
  0x3C43,
  0x3C45,
  0x3C46,
  0x3C48,
  0x3C4A,
  0x3C4B,
  0x3C4D,
  0x3C4E,
  0x3C50,
  0x3C52,
  0x3C53,
  0x3C55,
  0x3C57,
  0x3C58,
  0x3C5A,
  0x3C5C,
  0x3C5D,
  0x3C5F,
  0x3C60,
  0x3C62,
  0x3C64,
  0x3C65,
  0x3C67,
  0x3C69,
  0x3C6A,
  0x3C6C,
  0x3C6E,
  0x3C6F,
  0x3C71,
  0x3C72,
  0x3C74,
  0x3C76,
  0x3C77,
  0x3C79,
  0x3C7B,
  0x3C7C,
  0x3C7E,
  0x3C80,
  0x3C81,
  0x3C81,
  0x3C82,
  0x3C83,
  0x3C84,
  0x3C85,
  0x3C86,
  0x3C86,
  0x3C87,
  0x3C88,
  0x3C89,
  0x3C8A,
  0x3C8A,
  0x3C8B,
  0x3C8C,
  0x3C8D,
  0x3C8E,
  0x3C8F,
  0x3C8F,
  0x3C90,
  0x3C91,
  0x3C92,
  0x3C93,
  0x3C93,
  0x3C94,
  0x3C95,
  0x3C96,
  0x3C97,
  0x3C98,
  0x3C98,
  0x3C99,
  0x3C9A,
  0x3C9B,
  0x3C9C,
  0x3C9C,
  0x3C9D,
  0x3C9E,
  0x3C9F,
  0x3CA0,
  0x3CA1,
  0x3CA1,
  0x3CA2,
  0x3CA3,
  0x3CA4,
  0x3CA5,
  0x3CA5,
  0x3CA6,
  0x3CA7,
  0x3CA8,
  0x3CA9,
  0x3CAA,
  0x3CAA,
  0x3CAB,
  0x3CAC,
  0x3CAD,
  0x3CAE,
  0x3CAE,
  0x3CAF,
  0x3CB0,
  0x3CB1,
  0x3CB2,
  0x3CB3,
  0x3CB3,
  0x3CB4,
  0x3CB5,
  0x3CB6,
  0x3CB7,
  0x3CB8,
  0x3CB8,
  0x3CB9,
  0x3CBA,
  0x3CBB,
  0x3CBC,
  0x3CBC,
  0x3CBD,
  0x3CBE,
  0x3CBF,
  0x3CC0,
  0x3CC1,
  0x3CC1,
  0x3CC2,
  0x3CC3,
  0x3CC4,
  0x3CC5,
  0x3CC5,
  0x3CC6,
  0x3CC7,
  0x3CC8,
  0x3CC9,
  0x3CCA,
  0x3CCA,
  0x3CCB,
  0x3CCC,
  0x3CCD,
  0x3CCE,
  0x3CCE,
  0x3CCF,
  0x3CD0,
  0x3CD1,
  0x3CD2,
  0x3CD3,
  0x3CD3,
  0x3CD4,
  0x3CD5,
  0x3CD6,
  0x3CD7,
  0x3CD7,
  0x3CD8,
  0x3CD9,
  0x3CDA,
  0x3CDB,
  0x3CDC,
  0x3CDC,
  0x3CDD,
  0x3CDE,
  0x3CDF,
  0x3CE0,
  0x3CE0,
  0x3CE1,
  0x3CE2,
  0x3CE3,
  0x3CE4,
  0x3CE5,
  0x3CE5,
  0x3CE6,
  0x3CE7,
  0x3CE8,
  0x3CE9,
  0x3CE9,
  0x3CEA,
  0x3CEB,
  0x3CEC,
  0x3CED,
  0x3CEE,
  0x3CEE,
  0x3CEF,
  0x3CF0,
  0x3CF1,
  0x3CF2,
  0x3CF2,
  0x3CF3,
  0x3CF4,
  0x3CF5,
  0x3CF6,
  0x3CF7,
  0x3CF7,
  0x3CF8,
  0x3CF9,
  0x3CFA,
  0x3CFB,
  0x3CFB,
  0x3CFC,
  0x3CFD,
  0x3CFE,
  0x3CFF,
  0x3D00,
  0x3D00,
  0x3D01,
  0x3D01,
  0x3D01,
  0x3D02,
  0x3D02,
  0x3D03,
  0x3D03,
  0x3D03,
  0x3D04,
  0x3D04,
  0x3D05,
  0x3D05,
  0x3D06,
  0x3D06,
  0x3D06,
  0x3D07,
  0x3D07,
  0x3D08,
  0x3D08,
  0x3D08,
  0x3D09,
  0x3D09,
  0x3D0A,
  0x3D0A,
  0x3D0A,
  0x3D0B,
  0x3D0B,
  0x3D0C,
  0x3D0C,
  0x3D0C,
  0x3D0D,
  0x3D0D,
  0x3D0E,
  0x3D0E,
  0x3D0F,
  0x3D0F,
  0x3D0F,
  0x3D10,
  0x3D10,
  0x3D11,
  0x3D11,
  0x3D11,
  0x3D12,
  0x3D12,
  0x3D13,
  0x3D13,
  0x3D13,
  0x3D14,
  0x3D14,
  0x3D15,
  0x3D15,
  0x3D16,
  0x3D16,
  0x3D16,
  0x3D17,
  0x3D17,
  0x3D18,
  0x3D18,
  0x3D18,
  0x3D19,
  0x3D19,
  0x3D1A,
  0x3D1A,
  0x3D1A,
  0x3D1B,
  0x3D1B,
  0x3D1C,
  0x3D1C,
  0x3D1C,
  0x3D1D,
  0x3D1D,
  0x3D1E,
  0x3D1E,
  0x3D1F,
  0x3D1F,
  0x3D1F,
  0x3D20,
  0x3D20,
  0x3D21,
  0x3D21,
  0x3D21,
  0x3D22,
  0x3D22,
  0x3D23,
  0x3D23,
  0x3D23,
  0x3D24,
  0x3D24,
  0x3D25,
  0x3D25,
  0x3D25,
  0x3D26,
  0x3D26,
  0x3D27,
  0x3D27,
  0x3D28,
  0x3D28,
  0x3D28,
  0x3D29,
  0x3D29,
  0x3D2A,
  0x3D2A,
  0x3D2A,
  0x3D2B,
  0x3D2B,
  0x3D2C,
  0x3D2C,
  0x3D2C,
  0x3D2D,
  0x3D2D,
  0x3D2E,
  0x3D2E,
  0x3D2E,
  0x3D2F,
  0x3D2F,
  0x3D30,
  0x3D30,
  0x3D31,
  0x3D31,
  0x3D31,
  0x3D32,
  0x3D32,
  0x3D33,
  0x3D33,
  0x3D33,
  0x3D34,
  0x3D34,
  0x3D35,
  0x3D35,
  0x3D35,
  0x3D36,
  0x3D36,
  0x3D37,
  0x3D37,
  0x3D38,
  0x3D38,
  0x3D38,
  0x3D39,
  0x3D39,
  0x3D3A,
  0x3D3A,
  0x3D3A,
  0x3D3B,
  0x3D3B,
  0x3D3C,
  0x3D3C,
  0x3D3C,
  0x3D3D,
  0x3D3D,
  0x3D3E,
  0x3D3E,
  0x3D3E,
  0x3D3F,
  0x3D3F,
  0x3D40,
  0x3D40,
  0x3D41,
  0x3D41,
  0x3D41,
  0x3D42,
  0x3D42,
  0x3D43,
  0x3D43,
  0x3D43,
  0x3D44,
  0x3D44,
  0x3D45,
  0x3D45,
  0x3D45,
  0x3D46,
  0x3D46,
  0x3D47,
  0x3D47,
  0x3D47,
  0x3D48,
  0x3D48,
  0x3D49,
  0x3D49,
  0x3D4A,
  0x3D4A,
  0x3D4A,
  0x3D4B,
  0x3D4B,
  0x3D4C,
  0x3D4C,
  0x3D4C,
  0x3D4D,
  0x3D4D,
  0x3D4E,
  0x3D4E,
  0x3D4E,
  0x3D4F,
  0x3D4F,
  0x3D50,
  0x3D50,
  0x3D50,
  0x3D51,
  0x3D51,
  0x3D52,
  0x3D52,
  0x3D53,
  0x3D53,
  0x3D53,
  0x3D54,
  0x3D54,
  0x3D55,
  0x3D55,
  0x3D55,
  0x3D56,
  0x3D56,
  0x3D57,
  0x3D57,
  0x3D57,
  0x3D58,
  0x3D58,
  0x3D59,
  0x3D59,
  0x3D59,
  0x3D5A,
  0x3D5A,
  0x3D5B,
  0x3D5B,
  0x3D5C,
  0x3D5C,
  0x3D5C,
  0x3D5D,
  0x3D5D,
  0x3D5E,
  0x3D5E,
  0x3D5E,
  0x3D5F,
  0x3D5F,
  0x3D60,
  0x3D60,
  0x3D60,
  0x3D61,
  0x3D61,
  0x3D62,
  0x3D62,
  0x3D63,
  0x3D63,
  0x3D63,
  0x3D64,
  0x3D64,
  0x3D65,
  0x3D65,
  0x3D65,
  0x3D66,
  0x3D66,
  0x3D67,
  0x3D67,
  0x3D67,
  0x3D68,
  0x3D68,
  0x3D69,
  0x3D69,
  0x3D69,
  0x3D6A,
  0x3D6A,
  0x3D6B,
  0x3D6B,
  0x3D6C,
  0x3D6C,
  0x3D6C,
  0x3D6D,
  0x3D6D,
  0x3D6E,
  0x3D6E,
  0x3D6E,
  0x3D6F,
  0x3D6F,
  0x3D70,
  0x3D70,
  0x3D70,
  0x3D71,
  0x3D71,
  0x3D72,
  0x3D72,
  0x3D72,
  0x3D73,
  0x3D73,
  0x3D74,
  0x3D74,
  0x3D75,
  0x3D75,
  0x3D75,
  0x3D76,
  0x3D76,
  0x3D77,
  0x3D77,
  0x3D77,
  0x3D78,
  0x3D78,
  0x3D79,
  0x3D79,
  0x3D79,
  0x3D7A,
  0x3D7A,
  0x3D7B,
  0x3D7B,
  0x3D7B,
  0x3D7C,
  0x3D7C,
  0x3D7D,
  0x3D7D,
  0x3D7E,
  0x3D7E,
  0x3D7E,
  0x3D7F,
  0x3D7F,
  0x3D80,
  0x3D80,
  0x3D80,
  0x3D80,
  0x3D81,
  0x3D81,
  0x3D81,
  0x3D81,
  0x3D81,
  0x3D82,
  0x3D82,
  0x3D82,
  0x3D82,
  0x3D82,
  0x3D83,
  0x3D83,
  0x3D83,
  0x3D83,
  0x3D83,
  0x3D84,
  0x3D84,
  0x3D84,
  0x3D84,
  0x3D85,
  0x3D85,
  0x3D85,
  0x3D85,
  0x3D85,
  0x3D86,
  0x3D86,
  0x3D86,
  0x3D86,
  0x3D86,
  0x3D87,
  0x3D87,
  0x3D87,
  0x3D87,
  0x3D87,
  0x3D88,
  0x3D88,
  0x3D88,
  0x3D88,
  0x3D88,
  0x3D89,
  0x3D89,
  0x3D89,
  0x3D89,
  0x3D89,
  0x3D8A,
  0x3D8A,
  0x3D8A,
  0x3D8A,
  0x3D8A,
  0x3D8B,
  0x3D8B,
  0x3D8B,
  0x3D8B,
  0x3D8B,
  0x3D8C,
  0x3D8C,
  0x3D8C,
  0x3D8C,
  0x3D8C,
  0x3D8D,
  0x3D8D,
  0x3D8D,
  0x3D8D,
  0x3D8E,
  0x3D8E,
  0x3D8E,
  0x3D8E,
  0x3D8E,
  0x3D8F,
  0x3D8F,
  0x3D8F,
  0x3D8F,
  0x3D8F,
  0x3D90,
  0x3D90,
  0x3D90,
  0x3D90,
  0x3D90,
  0x3D91,
  0x3D91,
  0x3D91,
  0x3D91,
  0x3D91,
  0x3D92,
  0x3D92,
  0x3D92,
  0x3D92,
  0x3D92,
  0x3D93,
  0x3D93,
  0x3D93,
  0x3D93,
  0x3D93,
  0x3D94,
  0x3D94,
  0x3D94,
  0x3D94,
  0x3D94,
  0x3D95,
  0x3D95,
  0x3D95,
  0x3D95,
  0x3D96,
  0x3D96,
  0x3D96,
  0x3D96,
  0x3D96,
  0x3D97,
  0x3D97,
  0x3D97,
  0x3D97,
  0x3D97,
  0x3D98,
  0x3D98,
  0x3D98,
  0x3D98,
  0x3D98,
  0x3D99,
  0x3D99,
  0x3D99,
  0x3D99,
  0x3D99,
  0x3D9A,
  0x3D9A,
  0x3D9A,
  0x3D9A,
  0x3D9A,
  0x3D9B,
  0x3D9B,
  0x3D9B,
  0x3D9B,
  0x3D9B,
  0x3D9C,
  0x3D9C,
  0x3D9C,
  0x3D9C,
  0x3D9C,
  0x3D9D,
  0x3D9D,
  0x3D9D,
  0x3D9D,
  0x3D9D,
  0x3D9E,
  0x3D9E,
  0x3D9E,
  0x3D9E,
  0x3D9F,
  0x3D9F,
  0x3D9F,
  0x3D9F,
  0x3D9F,
  0x3DA0,
  0x3DA0,
  0x3DA0,
  0x3DA0,
  0x3DA0,
  0x3DA1,
  0x3DA1,
  0x3DA1,
  0x3DA1,
  0x3DA1,
  0x3DA2,
  0x3DA2,
  0x3DA2,
  0x3DA2,
  0x3DA2,
  0x3DA3,
  0x3DA3,
  0x3DA3,
  0x3DA3,
  0x3DA3,
  0x3DA4,
  0x3DA4,
  0x3DA4,
  0x3DA4,
  0x3DA4,
  0x3DA5,
  0x3DA5,
  0x3DA5,
  0x3DA5,
  0x3DA5,
  0x3DA6,
  0x3DA6,
  0x3DA6,
  0x3DA6,
  0x3DA7,
  0x3DA7,
  0x3DA7,
  0x3DA7,
  0x3DA7,
  0x3DA8,
  0x3DA8,
  0x3DA8,
  0x3DA8,
  0x3DA8,
  0x3DA9,
  0x3DA9,
  0x3DA9,
  0x3DA9,
  0x3DA9,
  0x3DAA,
  0x3DAA,
  0x3DAA,
  0x3DAA,
  0x3DAA,
  0x3DAB,
  0x3DAB,
  0x3DAB,
  0x3DAB,
  0x3DAB,
  0x3DAC,
  0x3DAC,
  0x3DAC,
  0x3DAC,
  0x3DAC,
  0x3DAD,
  0x3DAD,
  0x3DAD,
  0x3DAD,
  0x3DAD,
  0x3DAE,
  0x3DAE,
  0x3DAE,
  0x3DAE,
  0x3DAE,
  0x3DAF,
  0x3DAF,
  0x3DAF,
  0x3DAF,
  0x3DB0,
  0x3DB0,
  0x3DB0,
  0x3DB0,
  0x3DB0,
  0x3DB1,
  0x3DB1,
  0x3DB1,
  0x3DB1,
  0x3DB1,
  0x3DB2,
  0x3DB2,
  0x3DB2,
  0x3DB2,
  0x3DB2,
  0x3DB3,
  0x3DB3,
  0x3DB3,
  0x3DB3,
  0x3DB3,
  0x3DB4,
  0x3DB4,
  0x3DB4,
  0x3DB4,
  0x3DB4,
  0x3DB5,
  0x3DB5,
  0x3DB5,
  0x3DB5,
  0x3DB5,
  0x3DB6,
  0x3DB6,
  0x3DB6,
  0x3DB6,
  0x3DB6,
  0x3DB7,
  0x3DB7,
  0x3DB7,
  0x3DB7,
  0x3DB8,
  0x3DB8,
  0x3DB8,
  0x3DB8,
  0x3DB8,
  0x3DB9,
  0x3DB9,
  0x3DB9,
  0x3DB9,
  0x3DB9,
  0x3DBA,
  0x3DBA,
  0x3DBA,
  0x3DBA,
  0x3DBA,
  0x3DBB,
  0x3DBB,
  0x3DBB,
  0x3DBB,
  0x3DBB,
  0x3DBC,
  0x3DBC,
  0x3DBC,
  0x3DBC,
  0x3DBC,
  0x3DBD,
  0x3DBD,
  0x3DBD,
  0x3DBD,
  0x3DBD,
  0x3DBE,
  0x3DBE,
  0x3DBE,
  0x3DBE,
  0x3DBE,
  0x3DBF,
  0x3DBF,
  0x3DBF,
  0x3DBF,
  0x3DBF,
  0x3DC0,
  0x3DC0,
  0x3DC0,
  0x3DC0,
  0x3DC1,
  0x3DC1,
  0x3DC1,
  0x3DC1,
  0x3DC1,
  0x3DC2,
  0x3DC2,
  0x3DC2,
  0x3DC2,
  0x3DC2,
  0x3DC3,
  0x3DC3,
  0x3DC3,
  0x3DC3,
  0x3DC3,
  0x3DC4,
  0x3DC4,
  0x3DC4,
  0x3DC4,
  0x3DC4,
  0x3DC5,
  0x3DC5,
  0x3DC5,
  0x3DC5,
  0x3DC5,
  0x3DC6,
  0x3DC6,
  0x3DC6,
  0x3DC6,
  0x3DC6,
  0x3DC7,
  0x3DC7,
  0x3DC7,
  0x3DC7,
  0x3DC7,
  0x3DC8,
  0x3DC8,
  0x3DC8,
  0x3DC8,
  0x3DC8,
  0x3DC9,
  0x3DC9,
  0x3DC9,
  0x3DC9,
  0x3DCA,
  0x3DCA,
  0x3DCA,
  0x3DCA,
  0x3DCA,
  0x3DCB,
  0x3DCB,
  0x3DCB,
  0x3DCB,
  0x3DCB,
  0x3DCC,
  0x3DCC,
  0x3DCC,
  0x3DCC,
  0x3DCC,
  0x3DCD,
  0x3DCE,
  0x3DCF,
  0x3DD0,
  0x3DD1,
  0x3DD2,
  0x3DD3,
  0x3DD4,
  0x3DD5,
  0x3DD6,
  0x3DD7,
  0x3DD8,
  0x3DD9,
  0x3DDA,
  0x3DDB,
  0x3DDC,
  0x3DDD,
  0x3DDE,
  0x3DDF,
  0x3DE0,
  0x3DE1,
  0x3DE2,
  0x3DE3,
  0x3DE4,
  0x3DE5,
};

static u16 sigmode_golden_bf16[] = {
  0x0,
  0x3c24,
  0x3c68,
  0x3c8e,
  0x3ca4,
  0x3cb7,
  0x3cc8,
  0x3cd9,
  0x3ce8,
  0x3cf6,
  0x3d01,
  0x3d08,
  0x3d0e,
  0x3d14,
  0x3d19,
  0x3d1f,
  0x3d24,
  0x3d29,
  0x3d2e,
  0x3d33,
  0x3d37,
  0x3d3c,
  0x3d40,
  0x3d45,
  0x3d48,
  0x3d4d,
  0x3d51,
  0x3d55,
  0x3d59,
  0x3d5d,
  0x3d61,
  0x3d64,
  0x3d68,
  0x3d6b,
  0x3d6f,
  0x3d72,
  0x3d76,
  0x3d79,
  0x3d7c,
  0x3d80,
  0x3d81,
  0x3d83,
  0x3d85,
  0x3d86,
  0x3d88,
  0x3d89,
  0x3d8b,
  0x3d8c,
  0x3d8e,
  0x3d90,
  0x3d91,
  0x3d92,
  0x3d94,
  0x3d95,
  0x3d97,
  0x3d98,
  0x3d99,
  0x3d9b,
  0x3d9c,
  0x3d9d,
  0x3d9f,
  0x3da0,
  0x3da1,
  0x3da2,
  0x3da4,
  0x3da5,
  0x3da6,
  0x3da8,
  0x3da9,
  0x3daa,
  0x3dab,
  0x3dad,
  0x3dae,
  0x3daf,
  0x3db0,
  0x3db1,
  0x3db3,
  0x3db4,
  0x3db5,
  0x3db6,
  0x3db7,
  0x3db9,
  0x3db9,
  0x3dbb,
  0x3dbc,
  0x3dbd,
  0x3dbe,
  0x3dbf,
  0x3dc0,
  0x3dc1,
  0x3dc2,
  0x3dc3,
  0x3dc5,
  0x3dc5,
  0x3dc7,
  0x3dc8,
  0x3dc8,
  0x3dca,
  0x3dcb,
  0x3dcc,
  0x3dcd,
  0x3dce,
  0x3dcf,
  0x3dd0,
  0x3dd1,
  0x3dd2,
  0x3dd3,
  0x3dd4,
  0x3dd5,
  0x3dd6,
  0x3dd7,
  0x3dd8,
  0x3dd9,
  0x3dda,
  0x3ddb,
  0x3ddb,
  0x3ddd,
  0x3dde,
  0x3dde,
  0x3ddf,
  0x3de1,
  0x3de1,
  0x3de2,
  0x3de3,
  0x3de4,
  0x3de5,
  0x3de6,
  0x3de7,
  0x3de8,
  0x3de8,
  0x3dea,
  0x3deb,
  0x3deb,
  0x3dec,
  0x3ded,
  0x3dee,
  0x3def,
  0x3def,
  0x3df1,
  0x3df2,
  0x3df2,
  0x3df3,
  0x3df4,
  0x3df5,
  0x3df6,
  0x3df7,
  0x3df7,
  0x3df8,
  0x3df9,
  0x3dfa,
  0x3dfb,
  0x3dfb,
  0x3dfc,
  0x3dfd,
  0x3dfe,
  0x3dff,
  0x3e00,
  0x3e00,
  0x3e00,
  0x3e01,
  0x3e01,
  0x3e02,
  0x3e02,
  0x3e03,
  0x3e03,
  0x3e03,
  0x3e04,
  0x3e04,
  0x3e05,
  0x3e05,
  0x3e05,
  0x3e06,
  0x3e06,
  0x3e07,
  0x3e07,
  0x3e07,
  0x3e08,
  0x3e08,
  0x3e09,
  0x3e09,
  0x3e09,
  0x3e0a,
  0x3e0a,
  0x3e0b,
  0x3e0b,
  0x3e0b,
  0x3e0b,
  0x3e0c,
  0x3e0c,
  0x3e0d,
  0x3e0d,
  0x3e0d,
  0x3e0e,
  0x3e0e,
  0x3e0f,
  0x3e0f,
  0x3e10,
  0x3e10,
  0x3e10,
  0x3e10,
  0x3e11,
  0x3e11,
  0x3e11,
  0x3e12,
  0x3e12,
  0x3e13,
  0x3e13,
  0x3e14,
  0x3e14,
  0x3e14,
  0x3e14,
  0x3e15,
  0x3e15,
  0x3e15,
  0x3e16,
  0x3e16,
  0x3e17,
  0x3e17,
  0x3e17,
  0x3e17,
  0x3e18,
  0x3e18,
  0x3e19,
  0x3e19,
  0x3e19,
  0x3e19,
  0x3e1a,
  0x3e1a,
  0x3e1b,
  0x3e1b,
  0x3e1b,
  0x3e1c,
  0x3e1c,
  0x3e1c,
  0x3e1d,
  0x3e1d,
  0x3e1d,
  0x3e1e,
  0x3e1e,
  0x3e1e,
  0x3e1f,
  0x3e1f,
  0x3e1f,
  0x3e20,
  0x3e20,
  0x3e20,
  0x3e21,
  0x3e21,
  0x3e21,
  0x3e22,
  0x3e22,
  0x3e22,
  0x3e22,
  0x3e23,
  0x3e23,
  0x3e24,
  0x3e24,
  0x3e24,
  0x3e24,
  0x3e25,
  0x3e25,
  0x3e26,
  0x3e26,
  0x3e26,
  0x3e26,
  0x3e27,
  0x3e27,
  0x3e27,
  0x3e28,
  0x3e28,
  0x3e28,
  0x3e29,
  0x3e29,
  0x3e29,
  0x3e29,
  0x3e2a,
  0x3e2a,
  0x3e2a,
  0x3e2b,
  0x3e2b,
  0x3e2b,
  0x3e2c,
  0x3e2c,
  0x3e2c,
  0x3e2d,
  0x3e2d,
  0x3e2d,
  0x3e2d,
  0x3e2e,
  0x3e2e,
  0x3e2f,
  0x3e2f,
  0x3e2f,
  0x3e2f,
  0x3e30,
  0x3e30,
  0x3e30,
  0x3e30,
  0x3e31,
  0x3e31,
  0x3e31,
  0x3e32,
  0x3e32,
  0x3e32,
  0x3e33,
  0x3e33,
  0x3e33,
  0x3e33,
  0x3e34,
  0x3e34,
  0x3e34,
  0x3e35,
  0x3e35,
  0x3e35,
  0x3e36,
  0x3e36,
  0x3e36,
  0x3e36,
  0x3e36,
  0x3e37,
  0x3e37,
  0x3e37,
  0x3e38,
  0x3e38,
  0x3e39,
  0x3e39,
  0x3e39,
  0x3e39,
  0x3e39,
  0x3e3a,
  0x3e3a,
  0x3e3b,
  0x3e3b,
  0x3e3b,
  0x3e3b,
  0x3e3b,
  0x3e3c,
  0x3e3c,
  0x3e3c,
  0x3e3d,
  0x3e3d,
  0x3e3d,
  0x3e3d,
  0x3e3d,
  0x3e3e,
  0x3e3e,
  0x3e3f,
  0x3e3f,
  0x3e3f,
  0x3e3f,
  0x3e3f,
  0x3e40,
  0x3e40,
  0x3e41,
  0x3e41,
  0x3e41,
  0x3e41,
  0x3e41,
  0x3e42,
  0x3e42,
  0x3e42,
  0x3e43,
  0x3e43,
  0x3e43,
  0x3e43,
  0x3e44,
  0x3e44,
  0x3e44,
  0x3e45,
  0x3e45,
  0x3e45,
  0x3e45,
  0x3e45,
  0x3e46,
  0x3e46,
  0x3e47,
  0x3e47,
  0x3e47,
  0x3e47,
  0x3e47,
  0x3e48,
  0x3e48,
  0x3e48,
  0x3e48,
  0x3e48,
  0x3e49,
  0x3e49,
  0x3e4a,
  0x3e4a,
  0x3e4a,
  0x3e4a,
  0x3e4a,
  0x3e4b,
  0x3e4b,
  0x3e4b,
  0x3e4c,
  0x3e4c,
  0x3e4c,
  0x3e4c,
  0x3e4c,
  0x3e4d,
  0x3e4d,
  0x3e4e,
  0x3e4e,
  0x3e4e,
  0x3e4e,
  0x3e4e,
  0x3e4f,
  0x3e4f,
  0x3e4f,
  0x3e4f,
  0x3e4f,
  0x3e50,
  0x3e50,
  0x3e51,
  0x3e51,
  0x3e51,
  0x3e51,
  0x3e51,
  0x3e52,
  0x3e52,
  0x3e52,
  0x3e52,
  0x3e52,
  0x3e53,
  0x3e53,
  0x3e53,
  0x3e54,
  0x3e54,
  0x3e54,
  0x3e54,
  0x3e55,
  0x3e55,
  0x3e55,
  0x3e55,
  0x3e55,
  0x3e56,
  0x3e56,
  0x3e56,
  0x3e57,
  0x3e57,
  0x3e57,
  0x3e57,
  0x3e57,
  0x3e58,
  0x3e58,
  0x3e58,
  0x3e58,
  0x3e59,
  0x3e59,
  0x3e59,
  0x3e5a,
  0x3e5a,
  0x3e5a,
  0x3e5a,
  0x3e5a,
  0x3e5b,
  0x3e5b,
  0x3e5b,
  0x3e5b,
  0x3e5b,
  0x3e5c,
  0x3e5c,
  0x3e5d,
  0x3e5d,
  0x3e5d,
  0x3e5d,
  0x3e5d,
  0x3e5e,
  0x3e5e,
  0x3e5e,
  0x3e5e,
  0x3e5e,
  0x3e5f,
  0x3e5f,
  0x3e5f,
  0x3e5f,
  0x3e5f,
  0x3e60,
  0x3e60,
  0x3e61,
  0x3e61,
  0x3e61,
  0x3e61,
  0x3e61,
  0x3e62,
  0x3e62,
  0x3e62,
  0x3e62,
  0x3e62,
  0x3e63,
  0x3e63,
  0x3e63,
  0x3e63,
  0x3e63,
  0x3e64,
  0x3e64,
  0x3e65,
  0x3e65,
  0x3e65,
  0x3e65,
  0x3e65,
  0x3e66,
  0x3e66,
  0x3e66,
  0x3e66,
  0x3e66,
  0x3e67,
  0x3e67,
  0x3e67,
  0x3e67,
  0x3e67,
  0x3e68,
  0x3e68,
  0x3e68,
  0x3e68,
  0x3e68,
  0x3e69,
  0x3e69,
  0x3e6a,
  0x3e6a,
  0x3e6a,
  0x3e6a,
  0x3e6a,
  0x3e6b,
  0x3e6b,
  0x3e6b,
  0x3e6b,
  0x3e6b,
  0x3e6c,
  0x3e6c,
  0x3e6c,
  0x3e6c,
  0x3e6c,
  0x3e6d,
  0x3e6d,
  0x3e6d,
  0x3e6d,
  0x3e6d,
  0x3e6e,
  0x3e6e,
  0x3e6e,
  0x3e6e,
  0x3e6e,
  0x3e6f,
  0x3e6f,
  0x3e6f,
  0x3e6f,
  0x3e6f,
  0x3e70,
  0x3e70,
  0x3e71,
  0x3e71,
  0x3e71,
  0x3e71,
  0x3e71,
  0x3e72,
  0x3e72,
  0x3e72,
  0x3e72,
  0x3e72,
  0x3e73,
  0x3e73,
  0x3e73,
  0x3e73,
  0x3e73,
  0x3e74,
  0x3e74,
  0x3e74,
  0x3e74,
  0x3e74,
  0x3e75,
  0x3e75,
  0x3e75,
  0x3e75,
  0x3e76,
  0x3e76,
  0x3e76,
  0x3e76,
  0x3e76,
  0x3e77,
  0x3e77,
  0x3e77,
  0x3e77,
  0x3e77,
  0x3e78,
  0x3e78,
  0x3e78,
  0x3e78,
  0x3e78,
  0x3e79,
  0x3e79,
  0x3e79,
  0x3e79,
  0x3e79,
  0x3e7a,
  0x3e7a,
  0x3e7a,
  0x3e7a,
  0x3e7a,
  0x3e7b,
  0x3e7b,
  0x3e7b,
  0x3e7b,
  0x3e7b,
  0x3e7c,
  0x3e7c,
  0x3e7c,
  0x3e7c,
  0x3e7c,
  0x3e7d,
  0x3e7d,
  0x3e7d,
  0x3e7d,
  0x3e7d,
  0x3e7e,
  0x3e7e,
  0x3e7e,
  0x3e7e,
  0x3e7f,
  0x3e7f,
  0x3e7f,
  0x3e7f,
  0x3e7f,
  0x3e80,
  0x3e80,
  0x3e80,
  0x3e80,
  0x3e80,
  0x3e80,
  0x3e80,
  0x3e80,
  0x3e80,
  0x3e81,
  0x3e81,
  0x3e81,
  0x3e81,
  0x3e81,
  0x3e81,
  0x3e81,
  0x3e81,
  0x3e81,
  0x3e81,
  0x3e82,
  0x3e82,
  0x3e82,
  0x3e82,
  0x3e82,
  0x3e82,
  0x3e82,
  0x3e82,
  0x3e82,
  0x3e83,
  0x3e83,
  0x3e83,
  0x3e83,
  0x3e83,
  0x3e83,
  0x3e83,
  0x3e83,
  0x3e83,
  0x3e83,
  0x3e84,
  0x3e84,
  0x3e84,
  0x3e84,
  0x3e84,
  0x3e84,
  0x3e84,
  0x3e84,
  0x3e84,
  0x3e84,
  0x3e85,
  0x3e85,
  0x3e85,
  0x3e85,
  0x3e85,
  0x3e85,
  0x3e85,
  0x3e85,
  0x3e85,
  0x3e85,
  0x3e86,
  0x3e86,
  0x3e86,
  0x3e86,
  0x3e86,
  0x3e86,
  0x3e86,
  0x3e86,
  0x3e86,
  0x3e87,
  0x3e87,
  0x3e87,
  0x3e87,
  0x3e87,
  0x3e87,
  0x3e87,
  0x3e87,
  0x3e87,
  0x3e87,
  0x3e88,
  0x3e88,
  0x3e88,
  0x3e88,
  0x3e88,
  0x3e88,
  0x3e88,
  0x3e88,
  0x3e88,
  0x3e88,
  0x3e89,
  0x3e89,
  0x3e89,
  0x3e89,
  0x3e89,
  0x3e89,
  0x3e89,
  0x3e89,
  0x3e89,
  0x3e89,
  0x3e8a,
  0x3e8a,
  0x3e8a,
  0x3e8a,
  0x3e8a,
  0x3e8a,
  0x3e8a,
  0x3e8a,
  0x3e8a,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8b,
  0x3e8c,
  0x3e8c,
  0x3e8c,
  0x3e8c,
  0x3e8c,
  0x3e8c,
  0x3e8c,
  0x3e8c,
  0x3e8c,
  0x3e8c,
  0x3e8d,
  0x3e8d,
  0x3e8d,
  0x3e8d,
  0x3e8d,
  0x3e8d,
  0x3e8d,
  0x3e8d,
  0x3e8d,
  0x3e8d,
  0x3e8e,
  0x3e8e,
  0x3e8e,
  0x3e8e,
  0x3e8e,
  0x3e8e,
  0x3e8e,
  0x3e8e,
  0x3e8e,
  0x3e8f,
  0x3e8f,
  0x3e8f,
  0x3e8f,
  0x3e8f,
  0x3e8f,
  0x3e8f,
  0x3e8f,
  0x3e8f,
  0x3e8f,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e90,
  0x3e91,
  0x3e91,
  0x3e91,
  0x3e91,
  0x3e91,
  0x3e91,
  0x3e91,
  0x3e91,
  0x3e91,
  0x3e91,
  0x3e92,
  0x3e92,
  0x3e92,
  0x3e92,
  0x3e92,
  0x3e92,
  0x3e92,
  0x3e92,
  0x3e92,
  0x3e93,
  0x3e93,
  0x3e93,
  0x3e93,
  0x3e93,
  0x3e93,
  0x3e93,
  0x3e93,
  0x3e93,
  0x3e93,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e94,
  0x3e95,
  0x3e95,
  0x3e95,
  0x3e95,
  0x3e95,
  0x3e95,
  0x3e95,
  0x3e95,
  0x3e95,
  0x3e95,
  0x3e96,
  0x3e96,
  0x3e96,
  0x3e96,
  0x3e96,
  0x3e96,
  0x3e96,
  0x3e96,
  0x3e96,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e97,
  0x3e98,
  0x3e98,
  0x3e98,
  0x3e98,
  0x3e98,
  0x3e98,
  0x3e98,
  0x3e98,
  0x3e98,
  0x3e98,
  0x3e99,
  0x3e99,
  0x3e99,
  0x3e99,
  0x3e99,
  0x3e99,
  0x3e99,
  0x3e99,
  0x3e99,
  0x3e99,
  0x3e99,
  0x3e99,
  0x3e99,
  0x3e99,
  0x3e9a,
  0x3e9a,
  0x3e9a,
  0x3e9a,
  0x3e9a,
  0x3e9a,
  0x3e9a,
  0x3e9a,
  0x3e9a,
  0x3e9a,
  0x3e9b,
  0x3e9b,
  0x3e9b,
  0x3e9b,
  0x3e9b,
  0x3e9b,
  0x3e9b,
  0x3e9b,
  0x3e9b,
  0x3e9b,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9c,
  0x3e9d,
  0x3e9d,
  0x3e9d,
  0x3e9d,
  0x3e9d,
  0x3e9d,
  0x3e9d,
  0x3e9d,
  0x3e9d,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9e,
  0x3e9f,
  0x3e9f,
  0x3e9f,
  0x3e9f,
  0x3e9f,
  0x3e9f,
  0x3e9f,
  0x3e9f,
  0x3e9f,
  0x3e9f,
  0x3ea0,
  0x3ea0,
  0x3ea0,
  0x3ea0,
  0x3ea0,
  0x3ea0,
  0x3ea0,
  0x3ea0,
  0x3ea0,
  0x3ea0,
  0x3ea0,
  0x3ea0,
  0x3ea0,
  0x3ea0,
  0x3ea1,
  0x3ea1,
  0x3ea1,
  0x3ea1,
  0x3ea1,
  0x3ea1,
  0x3ea1,
  0x3ea1,
  0x3ea1,
  0x3ea1,
  0x3ea2,
  0x3ea2,
  0x3ea2,
  0x3ea2,
  0x3ea2,
  0x3ea2,
  0x3ea2,
  0x3ea3,
  0x3ea3,
  0x3ea4,
  0x3ea4,
  0x3ea4,
  0x3ea5,
  0x3ea5,
  0x3ea6,
  0x3ea6,
  0x3ea6,
  0x3ea7,
  0x3ea7,
  0x3ea7,
  0x3ea8,
  0x3ea8,
  0x3ea9,
  0x3ea9,
  0x3ea9,
  0x3eaa,
  0x3eaa,
  0x3eaa,
  0x3eab,
  0x3eab,
};

// <! gen invert sqrt
static double _gen_sqrt(int base, int p) {
  // y = x ^ 0.5
  double f = (double) (pow(base, p * 0.5));

  if (isnan(f)) {
    assert(0);
  }
  return f;
}

static void tl_lut_ref(
    u16 *ofmap,
    u16 *ifmap,
    tl_shape_t ifmap_shape
    )
{
  for (u32 i = 0; i < tl_shape_size(&ifmap_shape); i++) {
	if (mode == PRE_DATA_COMPARE_FIX) {
      ofmap[i] = sigmode_golden_bf16[i];
    }
	else if (mode == GEN_POW_20_DATA_MAX_ERROR_U8) {
	  u16 v = convert_fp32_bf16(pow(convert_bf16_fp32(ifmap[i]), 0.5));
      ofmap[i] = (u8) convert_bf16_s8(v);
	}
    else {
      ofmap[i] = convert_fp32_bf16(pow(convert_bf16_fp32(ifmap[i]), 0.5));
    }
  }
}

static void gen_sqrt(u16 *table_data, u64 table_size) {
  //<! 32*8 table, duplicate `channel` times;
  int half = table_size / channel / 2;
  u64 idx = 0;
  assert(table_size);
  assert(half == 128);

  // prepare channel 0
  double s = 0.0;
  table_data[idx] = convert_fp32_bf16(s); // 0^0.5 = 0
#ifdef DBG
  printf("t [%" PRIu64 "] is %f(%.8lf)[idx:%f][2^%f] bf %x\n", idx, convert_bf16_fp32(table_data[idx]), s, (float)exp_start, (float)(exp_start/2), table_data[idx]);
#endif
  idx++;

  // > 0, exp from 0 -62 -61 ..  62  63
  for (int i = 0; i < half; i++) {
    //float exp = round((exp_start + i) / 2) * 2;
    int shift = (exp_start + i);
    bool is_odd = (shift % 2);
    float exp = shift;
    if (is_odd) {
      exp = shift > 0 ? exp - 1 : exp - 1;
    }

    double s = _gen_sqrt(2, exp);
    table_data[idx] = convert_fp32_bf16(s);
#ifdef DBG
    printf("t [%" PRIu64 "] is %f [idx:%f][2^%f(%f)] bf %x\n", idx,
        convert_bf16_fp32(table_data[idx]),
        float(exp_start + i), exp/2, (exp_start + i) / 2.0, 
        table_data[idx]);
#endif
    idx++;
  }

  //// idx = 127 dont care
#if 0
  s = _gen_sqrt(2, -0);
  table_data[idx] = convert_fp32_bf16(s);
#if 1
  printf("t [%" PRIu64 "] is %f[%d] bf %x\n", idx, convert_bf16_fp32(table_data[idx]), 0, table_data[idx]);
#endif
  idx++;

  for (int i = 1; i < half; i++) {
    float exp = exp_start + i;
    double s = _gen_sqrt(-2, exp);
    table_data[idx] = convert_fp32_bf16(s);
#ifdef DBG
    printf("t [%" PRIu64 "] is %f(%e - %.8lf)[(-2)^%f] bf %x\n", idx, convert_bf16_fp32(table_data[idx]), s, s, exp, table_data[idx]);
#endif
    idx++;
  }

  // idx = 255 dont care
  //s = _gen_sqrt(2, 0);
  //table_data[idx] = convert_fp32_bf16(s);
  //printf("t [%" PRIu64 "] is %f[%d]\n", idx, convert_bf16_fp32(table_data[idx]), 0);
  //idx++;
#endif

  // duplicate channel #1 to #31
  //TODO: tensor copy
  for (u32 i = 1; i < channel; i++) {
    memcpy(&table_data[i * table_hw], &table_data[0], sizeof(u16) * table_hw);
  }
}

static void gen_sqrt_mantissa(u16 IN *table_data, u16* OUT table_mantissa, u64 table_size) {

  u32 half = table_size / channel / 2;
  assert(half == 128);
  assert(table_data);

  int idx = 0;
  double d;
  for (u32 i = 0; i < half; i++) {
    d = 1 + i * 1 / 128.0;
    d = (double) pow(d, 0.5);
    table_mantissa[128+idx] = convert_fp32_bf16(d);
#ifdef DBG
    //printf(", [%u] is %lf\n", i+128, d);
#endif /* ifdef DBG */

    //13=2^3x1.625=(2^2)x(2^1x1.625)
    d = 2 * (1 + i * 1 / 128.0);
    d = (double) pow(d, 0.5);
    table_mantissa[idx] = convert_fp32_bf16(d);
#ifdef DBG
    //printf("mantissa [%u] is %lf", i, d);
#endif /* ifdef DBG */
    idx++;
  }
#ifdef DBG
  for (u32 i = 0; i < 2 * half; i++) {
	printf("mantissa [%u] is %lf, 0x%x\n", i, convert_bf16_fp32(table_mantissa[i]),
		table_mantissa[i]);
  }
#endif /* ifdef DBG */

  // duplicate channel #1 to #31
  //TODO: tensor copy
  for (u64 i = 1; i < channel; i++) {
    memcpy(&table_mantissa[table_hw * i], &table_mantissa[0], sizeof(u16) * table_hw);
  }
}

static bool verify(u16 *ofmap_data, u16 *ref_data, u16* ifmap, u64 ifmap_size) {
  u64 size = ifmap_size;

  for (u64 i = 0; i < size; i++) {
    bool is_close;
	u16 ref;
	u16 ofmap_data_bf16;
	float ref_f;
	float ofmap_data_f;
	u32 shift;
    
	if (mode == GEN_POW_20_DATA_MAX_ERROR_U8) {
	  shift = (i%2)*8;
	  ref = ref_data[i];
	  ofmap_data_bf16 = (u16)ofmap_data[i/2];
	  ofmap_data_f = (float)(ofmap_data[i/2] >> shift);
	  ref_f = (float)(ref);

      is_close = ((u8)(ofmap_data[i/2] >> shift)) == (u8)ref;

	  //printf("[%" PRIu64 "] of is %x ref is %x\n", i, (u8)(ofmap_data[i/2] >> shift), (u8)ref);
	}
	else {
	  ref = ref_data[i];
	  ref_f = convert_bf16_fp32(ref);
	  ofmap_data_f = convert_bf16_fp32(ofmap_data[i]);
	  ofmap_data_bf16 = ofmap_data[i];

	  if (mode == PRE_DATA_COMPARE_FIX) {
		is_close = ofmap_data[i] == ref;
	  }
	  else {
		is_close = almost_equal(ref_f, ofmap_data_f, 1);
		is_close = fabs(ref_f-ofmap_data_f) < 0.001;
	  }
	}

#if 0
	if (i == 0) {
	  fprintf(stderr,
		  "input, ofmap, ref, diff, diff / ref_f\n");
    }

    fprintf(stderr,
        "%.16f, %f, %lf, %lf, %lf\n",
        convert_bf16_fp32(ifmap[i]),
        ofmap_data_f, ref_f, fabs(ref_f - ofmap_data_f), fabs(ref_f - ofmap_data_f) / ref_f);
    //if (ofmap_data[i] != ref && fabs(ref_f-ofmap_data_f) > 0.07) 
    //if (ofmap_data[i] != ref && AlmostEqual2sComplement(ref_f, ofmap_data_f, 1))
    //if (ofmap_data[i] != ref && AlmostEqual(ref_f, ofmap_data_f, FLT_EPSILON))
#endif
    if (!is_close) {
      fprintf(stderr,
          "comparing failed at ofmap_data[%" PRIu64 "](input:%e), got %x, exp %x, fp32: got %e exp %e\n",
          i, convert_bf16_fp32(ifmap[i]),
          ofmap_data_bf16, ref, ofmap_data_f, ref_f);
      exit(-1);
    }
  }

  return true;
}

static void test_tl_int8_lut_bf16(CVI_RT_HANDLE *ctx, bmk_ctx_t *bmk)
{
  // TODO: check more shape / align
  tl_shape_t ifmap_shape;
  if (mode == PRE_DATA_COMPARE_FIX) {
    ifmap_shape = {1, channel, 4, 8};
  }
  else {
    ifmap_shape = {1, channel, 16, 16};
  }

  tl_shape_t table_shape = {1, channel, table_h, table_w}; // hard code for hw, hw:32x8
  tl_shape_t ofmap_shape = ifmap_shape;

  u64 ifmap_size = tl_shape_size(&ifmap_shape);
  u64 table_size = tl_shape_size(&table_shape);
  u64 ofmap_size = tl_shape_size(&ofmap_shape);

  fmt_t fmt = FMT_BF16;

  int data_type_size = bytesize_of_fmt(fmt);
  u64 ifmap_bytesize  =  ifmap_size * data_type_size;
  u64 table_bytesize  =  table_size * data_type_size;
  u64 ofmap_bytesize  =  ofmap_size * data_type_size;

  // hw ONLY support index in int8
  u16 *ifmap = (u16 *)xmalloc(ifmap_bytesize);
  memset(ifmap, 0x00, ifmap_bytesize);

  u16 *ifmap_mantissa = (u16 *)xmalloc(ifmap_bytesize);
  memset(ifmap_mantissa, 0x00, ifmap_bytesize);

  if (mode == PRE_DATA_COMPARE_FIX) {
    memcpy(ifmap, &test_pattern, sizeof(test_pattern));
  }
  else {
    for (u64 i = 0; i < ifmap_size; i++) {
      srand (static_cast <unsigned> (time(0)));
      std::random_device rd;
      std::mt19937 e2(rd());
      float LO = pow(2, -10);
      float HI = pow(2, 10);
      //std::uniform_real_distribution<> dist(pow(2,-62), pow(2,63));
      for (u64 i = 0; i < ifmap_size; i++) {
        //float r3 = dist(e2);
        float r3 = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
        ifmap[i] = convert_fp32_bf16(r3);
      }
    }
  }

#ifdef DBG
  for (u64 i = 0; i < ifmap_size; i++) {
    printf("source if[%" PRIu64 "] bf16 %f 0x%x, log2f is %f\n", i, convert_bf16_fp32(ifmap[i]), ifmap[i], floor(log2((convert_bf16_fp32(ifmap[i])))));
  }
#endif /* ifdef DBG */

  u16 *table_data = (u16 *)xmalloc(table_bytesize);
  gen_sqrt (table_data, table_size);

  u16 *table_data_mantissa = (u16 *)xmalloc(table_bytesize);
  gen_sqrt_mantissa(table_data, table_data_mantissa, table_size);

  u16 *ref_data = (u16 *)xmalloc(ofmap_bytesize);
  tl_lut_ref(ref_data, ifmap, ifmap_shape);

  tl_t *tl_ifmap =
    alloc_tl(bmk,ifmap_shape, fmt, /*align*/1);
  tl_t *tl_table_answer =
    alloc_tl(bmk, table_shape, fmt, /*align*/1);
  tl_t *tl_table_answer_mantissa =
    alloc_tl(bmk, table_shape, fmt, /*align*/1);

  tl_t *tl_ofmap_exp =
    alloc_tl(bmk,ofmap_shape, fmt, /*align*/1);
  tl_t *tl_ofmap_mantissa =
    alloc_tl(bmk,ofmap_shape, fmt, /*align*/1);
  tl_t *tl_ofmap_exp_val =
    alloc_tl(bmk,ofmap_shape, fmt, /*align*/1);

  tl_t *tl_ofmap_exp_val_u8 = nullptr;
  tl_t *out = tl_ofmap_exp_val;

  if (mode == GEN_POW_20_DATA_MAX_ERROR_U8) {
	tl_ofmap_exp_val_u8 =
	  alloc_tl(bmk,ofmap_shape, FMT_U8, /*align*/1);
  }
  
  // <! FIXME: prepare it
  bmk1880v2_tdma_tg2l_tensor_copy_param_t copy_p1, copy_p2, copy_p3;
  prepare_put_bf16_tensor_g2l(ctx, bmk, tl_ifmap, ifmap, fmt, &copy_p1);
  prepare_put_bf16_tensor_g2l(ctx, bmk, tl_table_answer, table_data, fmt, &copy_p2);
  prepare_put_bf16_tensor_g2l(ctx, bmk, tl_table_answer_mantissa, table_data_mantissa, fmt, &copy_p3);

  launch_put_bf16_tensor_g2l(ctx, bmk, copy_p1.src, &copy_p1); // input
  launch_put_bf16_tensor_g2l(ctx, bmk, copy_p2.src, &copy_p2); // table value
  launch_put_bf16_tensor_g2l(ctx, bmk, copy_p3.src, &copy_p3); // table mantissa

  bmk1880v2_tdma_l2l_tensor_copy_param_t p10;
  u16 *ofmap_data;

  // remove low 8 bits by int8 copy with stride
  // <! get index(pow)
  memset(&p10, 0x00, sizeof(p10));
  p10.dst = tl_ofmap_exp;
  p10.src = tl_ifmap;
  p10.mv_lut_idx = true;
  bmk1880v2_tdma_l2l_bf16_tensor_copy(bmk, &p10);
  p10.mv_lut_idx = false;
  test_submit(ctx);

  // <! get f(x0) = 2^(x0*-0.5)
  bmk1880v2_tiu_lookup_table_param_t p12;
  memset(&p12, 0, sizeof(p12));
  p12.ofmap = tl_ofmap_exp_val;
  p12.ifmap = tl_ofmap_exp;
  p12.table = tl_table_answer;
  bmk1880v2_tiu_lookup_table(bmk, &p12);

  // <! get mantissa value
  p12.ofmap = tl_ofmap_mantissa;
  p12.ifmap = tl_ifmap;
  p12.table = tl_table_answer_mantissa;
  bmk1880v2_tiu_lookup_table(bmk, &p12);

  // sqrt = (2^exp) * mantissa
  bmk1880v2_tiu_element_wise_mul_param_t p1;
  memset(&p1, 0, sizeof(p1));
  p1.res_high = NULL;
  p1.res_low = tl_ofmap_exp_val;
  p1.a = tl_ofmap_mantissa;
  p1.b_is_const = 0;
  p1.b = tl_ofmap_exp_val;
  p1.rshift_bits = 0;
  p1.relu_enable = 0;
  bmk1880v2_tiu_element_wise_mul(bmk, &p1);

  if (mode == GEN_POW_20_DATA_MAX_ERROR_U8) {
	p10.dst = tl_ofmap_exp_val_u8;
	p10.src = tl_ofmap_exp_val;
	bmk1880v2_tdma_l2l_bf16_tensor_copy(bmk, &p10);
	out = tl_ofmap_exp_val_u8;
  }

  test_submit(ctx);

  ofmap_data = (u16*)get_bf16_tensor_l2g(ctx, bmk, out, out->fmt);
  verify(ofmap_data, ref_data, ifmap, ifmap_size);

  if (tl_ofmap_exp_val_u8) {
	free_tl(bmk, tl_ofmap_exp_val_u8);
  }
  free_tl(bmk, tl_ofmap_exp_val);
  free_tl(bmk, tl_ofmap_mantissa);
  free_tl(bmk, tl_ofmap_exp);
  free_tl(bmk, tl_table_answer_mantissa);
  free_tl(bmk, tl_table_answer);
  free_tl(bmk, tl_ifmap);

  free(ifmap);
  free(ifmap_mantissa);
  free(table_data);
  free(table_data_mantissa);
  free(ref_data);
  free(ofmap_data);
}

int main()
{
  CVI_RT_HANDLE ctx;
  bmk_ctx_t *bmk;
  int round_mode;

  round_mode = set_store_feround();

  test_init(&ctx, &bmk);

  for (int i = PRE_DATA_COMPARE_FIX; i < TEST_MODE_MAX; i++) {
  //for (int i = PRE_DATA_COMPARE_FIX; i < GEN_POW_20_DATA_MAX_ERROR_U8; i++) {
  //for (int i = GEN_POW_20_DATA_MAX_ERROR_U8; i < TEST_MODE_MAX; i++) {
    mode = static_cast<TEST_MODE>(i);
    printf ("test mode %d...\n", mode);
    test_tl_int8_lut_bf16(&ctx, bmk);
  }

  test_exit(&ctx);
  restore_feround(round_mode);
  return 0;
}
