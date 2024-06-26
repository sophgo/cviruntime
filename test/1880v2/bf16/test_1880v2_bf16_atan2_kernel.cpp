/**
 * \breif atan2 is implemented by atan, you can refer
 * [wiki](https://en.wikipedia.org/wiki/Atan2) for more details
 */

#include "../1880v2_test_util.h"
#define OUT
#define IN
#include <cfloat>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
//#define DBG

/**
 * pre_data means we test fixed pattern, it should be same sa lut
 */
enum TEST_MODE {
  PRE_DATA_COMPARE_FIX = 0,  // pre-data + fix compare
  DATA_COMPARE_ACCURACY,     // generate \range_start to \range_end value that
                             // check epsilon, default set x > 0, y > 0

  DATA_COMPARE_ACCURACY_X_GT_0,         // atan(y/x), x > 0, y = 0
  DATA_COMPARE_ACCURACY_X_LT_0_Y_GE_0,  // atan(y/x) + PI , x < 0 and y >= 0
  DATA_COMPARE_ACCURACY_X_LT_0_Y_LT_0,  // atan(y/x) - PI , x < 0 and y < 0
  DATA_COMPARE_ACCURACY_X_0_Y_GT_0,     // pi / 2, x = 0 and y > 0
  DATA_COMPARE_ACCURACY_X_0_Y_LT_0,     // -pi / 2, x = 0 and y < 0
  DATA_COMPARE_U8,  // generate \range_start to \range_end value that check
                    // epsilon, result bf16->u8
  TEST_MODE_MAX,
};


static TEST_MODE mode;

static u16 test_pattern[] = {
    0x0000, 0x38D2, 0x3952, 0x399D, 0x39D2, 0x3A03, 0x3A1D, 0x3A38, 0x3A52,
    0x3A6C, 0x3A83, 0x3A90, 0x3A9D, 0x3AAA, 0x3AB8, 0x3AC5, 0x3AD2, 0x3ADF,
    0x3AEC, 0x3AF9, 0x3B03, 0x3B0A, 0x3B10, 0x3B17, 0x3B1D, 0x3B24, 0x3B2A,
    0x3B31, 0x3B38, 0x3B3E, 0x3B45, 0x3B4B, 0x3B52, 0x3B58, 0x3B5F, 0x3B65,
    0x3B6C, 0x3B72, 0x3B79, 0x3B80, 0x3B83, 0x3B86, 0x3B8A, 0x3B8D, 0x3B90,
    0x3B93, 0x3B97, 0x3B9A, 0x3B9D, 0x3BA1, 0x3BA4, 0x3BA7, 0x3BAA, 0x3BAE,
    0x3BB1, 0x3BB4, 0x3BB8, 0x3BBB, 0x3BBE, 0x3BC1, 0x3BC5, 0x3BC8, 0x3BCB,
    0x3BCE, 0x3BD2, 0x3BD5, 0x3BD8, 0x3BDC, 0x3BDF, 0x3BE2, 0x3BE5, 0x3BE9,
    0x3BEC, 0x3BEF, 0x3BF2, 0x3BF6, 0x3BF9, 0x3BFC, 0x3C00, 0x3C01, 0x3C03,
    0x3C05, 0x3C06, 0x3C08, 0x3C0A, 0x3C0B, 0x3C0D, 0x3C0F, 0x3C10, 0x3C12,
    0x3C13, 0x3C15, 0x3C17, 0x3C18, 0x3C1A, 0x3C1C, 0x3C1D, 0x3C1F, 0x3C21,
    0x3C22, 0x3C24, 0x3C25, 0x3C27, 0x3C29, 0x3C2A, 0x3C2C, 0x3C2E, 0x3C2F,
    0x3C31, 0x3C33, 0x3C34, 0x3C36, 0x3C38, 0x3C39, 0x3C3B, 0x3C3C, 0x3C3E,
    0x3C40, 0x3C41, 0x3C43, 0x3C45, 0x3C46, 0x3C48, 0x3C4A, 0x3C4B, 0x3C4D,
    0x3C4E, 0x3C50, 0x3C52, 0x3C53, 0x3C55, 0x3C57, 0x3C58, 0x3C5A, 0x3C5C,
    0x3C5D, 0x3C5F, 0x3C60, 0x3C62, 0x3C64, 0x3C65, 0x3C67, 0x3C69, 0x3C6A,
    0x3C6C, 0x3C6E, 0x3C6F, 0x3C71, 0x3C72, 0x3C74, 0x3C76, 0x3C77, 0x3C79,
    0x3C7B, 0x3C7C, 0x3C7E, 0x3C80, 0x3C81, 0x3C81, 0x3C82, 0x3C83, 0x3C84,
    0x3C85, 0x3C86, 0x3C86, 0x3C87, 0x3C88, 0x3C89, 0x3C8A, 0x3C8A, 0x3C8B,
    0x3C8C, 0x3C8D, 0x3C8E, 0x3C8F, 0x3C8F, 0x3C90, 0x3C91, 0x3C92, 0x3C93,
    0x3C93, 0x3C94, 0x3C95, 0x3C96, 0x3C97, 0x3C98, 0x3C98, 0x3C99, 0x3C9A,
    0x3C9B, 0x3C9C, 0x3C9C, 0x3C9D, 0x3C9E, 0x3C9F, 0x3CA0, 0x3CA1, 0x3CA1,
    0x3CA2, 0x3CA3, 0x3CA4, 0x3CA5, 0x3CA5, 0x3CA6, 0x3CA7, 0x3CA8, 0x3CA9,
    0x3CAA, 0x3CAA, 0x3CAB, 0x3CAC, 0x3CAD, 0x3CAE, 0x3CAE, 0x3CAF, 0x3CB0,
    0x3CB1, 0x3CB2, 0x3CB3, 0x3CB3, 0x3CB4, 0x3CB5, 0x3CB6, 0x3CB7, 0x3CB8,
    0x3CB8, 0x3CB9, 0x3CBA, 0x3CBB, 0x3CBC, 0x3CBC, 0x3CBD, 0x3CBE, 0x3CBF,
    0x3CC0, 0x3CC1, 0x3CC1, 0x3CC2, 0x3CC3, 0x3CC4, 0x3CC5, 0x3CC5, 0x3CC6,
    0x3CC7, 0x3CC8, 0x3CC9, 0x3CCA, 0x3CCA, 0x3CCB, 0x3CCC, 0x3CCD, 0x3CCE,
    0x3CCE, 0x3CCF, 0x3CD0, 0x3CD1, 0x3CD2, 0x3CD3, 0x3CD3, 0x3CD4, 0x3CD5,
    0x3CD6, 0x3CD7, 0x3CD7, 0x3CD8, 0x3CD9, 0x3CDA, 0x3CDB, 0x3CDC, 0x3CDC,
    0x3CDD, 0x3CDE, 0x3CDF, 0x3CE0, 0x3CE0, 0x3CE1, 0x3CE2, 0x3CE3, 0x3CE4,
    0x3CE5, 0x3CE5, 0x3CE6, 0x3CE7, 0x3CE8, 0x3CE9, 0x3CE9, 0x3CEA, 0x3CEB,
    0x3CEC, 0x3CED, 0x3CEE, 0x3CEE, 0x3CEF, 0x3CF0, 0x3CF1, 0x3CF2, 0x3CF2,
    0x3CF3, 0x3CF4, 0x3CF5, 0x3CF6, 0x3CF7, 0x3CF7, 0x3CF8, 0x3CF9, 0x3CFA,
    0x3CFB, 0x3CFB, 0x3CFC, 0x3CFD, 0x3CFE, 0x3CFF, 0x3D00, 0x3D00, 0x3D01,
    0x3D01, 0x3D01, 0x3D02, 0x3D02, 0x3D03, 0x3D03, 0x3D03, 0x3D04, 0x3D04,
    0x3D05, 0x3D05, 0x3D06, 0x3D06, 0x3D06, 0x3D07, 0x3D07, 0x3D08, 0x3D08,
    0x3D08, 0x3D09, 0x3D09, 0x3D0A, 0x3D0A, 0x3D0A, 0x3D0B, 0x3D0B, 0x3D0C,
    0x3D0C, 0x3D0C, 0x3D0D, 0x3D0D, 0x3D0E, 0x3D0E, 0x3D0F, 0x3D0F, 0x3D0F,
    0x3D10, 0x3D10, 0x3D11, 0x3D11, 0x3D11, 0x3D12, 0x3D12, 0x3D13, 0x3D13,
    0x3D13, 0x3D14, 0x3D14, 0x3D15, 0x3D15, 0x3D16, 0x3D16, 0x3D16, 0x3D17,
    0x3D17, 0x3D18, 0x3D18, 0x3D18, 0x3D19, 0x3D19, 0x3D1A, 0x3D1A, 0x3D1A,
    0x3D1B, 0x3D1B, 0x3D1C, 0x3D1C, 0x3D1C, 0x3D1D, 0x3D1D, 0x3D1E, 0x3D1E,
    0x3D1F, 0x3D1F, 0x3D1F, 0x3D20, 0x3D20, 0x3D21, 0x3D21, 0x3D21, 0x3D22,
    0x3D22, 0x3D23, 0x3D23, 0x3D23, 0x3D24, 0x3D24, 0x3D25, 0x3D25, 0x3D25,
    0x3D26, 0x3D26, 0x3D27, 0x3D27, 0x3D28, 0x3D28, 0x3D28, 0x3D29, 0x3D29,
    0x3D2A, 0x3D2A, 0x3D2A, 0x3D2B, 0x3D2B, 0x3D2C, 0x3D2C, 0x3D2C, 0x3D2D,
    0x3D2D, 0x3D2E, 0x3D2E, 0x3D2E, 0x3D2F, 0x3D2F, 0x3D30, 0x3D30, 0x3D31,
    0x3D31, 0x3D31, 0x3D32, 0x3D32, 0x3D33, 0x3D33, 0x3D33, 0x3D34, 0x3D34,
    0x3D35, 0x3D35, 0x3D35, 0x3D36, 0x3D36, 0x3D37, 0x3D37, 0x3D38, 0x3D38,
    0x3D38, 0x3D39, 0x3D39, 0x3D3A, 0x3D3A, 0x3D3A, 0x3D3B, 0x3D3B, 0x3D3C,
    0x3D3C, 0x3D3C, 0x3D3D, 0x3D3D, 0x3D3E, 0x3D3E, 0x3D3E, 0x3D3F, 0x3D3F,
    0x3D40, 0x3D40, 0x3D41, 0x3D41, 0x3D41, 0x3D42, 0x3D42, 0x3D43, 0x3D43,
    0x3D43, 0x3D44, 0x3D44, 0x3D45, 0x3D45, 0x3D45, 0x3D46, 0x3D46, 0x3D47,
    0x3D47, 0x3D47, 0x3D48, 0x3D48, 0x3D49, 0x3D49, 0x3D4A, 0x3D4A, 0x3D4A,
    0x3D4B, 0x3D4B, 0x3D4C, 0x3D4C, 0x3D4C, 0x3D4D, 0x3D4D, 0x3D4E, 0x3D4E,
    0x3D4E, 0x3D4F, 0x3D4F, 0x3D50, 0x3D50, 0x3D50, 0x3D51, 0x3D51, 0x3D52,
    0x3D52, 0x3D53, 0x3D53, 0x3D53, 0x3D54, 0x3D54, 0x3D55, 0x3D55, 0x3D55,
    0x3D56, 0x3D56, 0x3D57, 0x3D57, 0x3D57, 0x3D58, 0x3D58, 0x3D59, 0x3D59,
    0x3D59, 0x3D5A, 0x3D5A, 0x3D5B, 0x3D5B, 0x3D5C, 0x3D5C, 0x3D5C, 0x3D5D,
    0x3D5D, 0x3D5E, 0x3D5E, 0x3D5E, 0x3D5F, 0x3D5F, 0x3D60, 0x3D60, 0x3D60,
    0x3D61, 0x3D61, 0x3D62, 0x3D62, 0x3D63, 0x3D63, 0x3D63, 0x3D64, 0x3D64,
    0x3D65, 0x3D65, 0x3D65, 0x3D66, 0x3D66, 0x3D67, 0x3D67, 0x3D67, 0x3D68,
    0x3D68, 0x3D69, 0x3D69, 0x3D69, 0x3D6A, 0x3D6A, 0x3D6B, 0x3D6B, 0x3D6C,
    0x3D6C, 0x3D6C, 0x3D6D, 0x3D6D, 0x3D6E, 0x3D6E, 0x3D6E, 0x3D6F, 0x3D6F,
    0x3D70, 0x3D70, 0x3D70, 0x3D71, 0x3D71, 0x3D72, 0x3D72, 0x3D72, 0x3D73,
    0x3D73, 0x3D74, 0x3D74, 0x3D75, 0x3D75, 0x3D75, 0x3D76, 0x3D76, 0x3D77,
    0x3D77, 0x3D77, 0x3D78, 0x3D78, 0x3D79, 0x3D79, 0x3D79, 0x3D7A, 0x3D7A,
    0x3D7B, 0x3D7B, 0x3D7B, 0x3D7C, 0x3D7C, 0x3D7D, 0x3D7D, 0x3D7E, 0x3D7E,
    0x3D7E, 0x3D7F, 0x3D7F, 0x3D80, 0x3D80, 0x3D80, 0x3D80, 0x3D81, 0x3D81,
    0x3D81, 0x3D81, 0x3D81, 0x3D82, 0x3D82, 0x3D82, 0x3D82, 0x3D82, 0x3D83,
    0x3D83, 0x3D83, 0x3D83, 0x3D83, 0x3D84, 0x3D84, 0x3D84, 0x3D84, 0x3D85,
    0x3D85, 0x3D85, 0x3D85, 0x3D85, 0x3D86, 0x3D86, 0x3D86, 0x3D86, 0x3D86,
    0x3D87, 0x3D87, 0x3D87, 0x3D87, 0x3D87, 0x3D88, 0x3D88, 0x3D88, 0x3D88,
    0x3D88, 0x3D89, 0x3D89, 0x3D89, 0x3D89, 0x3D89, 0x3D8A, 0x3D8A, 0x3D8A,
    0x3D8A, 0x3D8A, 0x3D8B, 0x3D8B, 0x3D8B, 0x3D8B, 0x3D8B, 0x3D8C, 0x3D8C,
    0x3D8C, 0x3D8C, 0x3D8C, 0x3D8D, 0x3D8D, 0x3D8D, 0x3D8D, 0x3D8E, 0x3D8E,
    0x3D8E, 0x3D8E, 0x3D8E, 0x3D8F, 0x3D8F, 0x3D8F, 0x3D8F, 0x3D8F, 0x3D90,
    0x3D90, 0x3D90, 0x3D90, 0x3D90, 0x3D91, 0x3D91, 0x3D91, 0x3D91, 0x3D91,
    0x3D92, 0x3D92, 0x3D92, 0x3D92, 0x3D92, 0x3D93, 0x3D93, 0x3D93, 0x3D93,
    0x3D93, 0x3D94, 0x3D94, 0x3D94, 0x3D94, 0x3D94, 0x3D95, 0x3D95, 0x3D95,
    0x3D95, 0x3D96, 0x3D96, 0x3D96, 0x3D96, 0x3D96, 0x3D97, 0x3D97, 0x3D97,
    0x3D97, 0x3D97, 0x3D98, 0x3D98, 0x3D98, 0x3D98, 0x3D98, 0x3D99, 0x3D99,
    0x3D99, 0x3D99, 0x3D99, 0x3D9A, 0x3D9A, 0x3D9A, 0x3D9A, 0x3D9A, 0x3D9B,
    0x3D9B, 0x3D9B, 0x3D9B, 0x3D9B, 0x3D9C, 0x3D9C, 0x3D9C, 0x3D9C, 0x3D9C,
    0x3D9D, 0x3D9D, 0x3D9D, 0x3D9D, 0x3D9D, 0x3D9E, 0x3D9E, 0x3D9E, 0x3D9E,
    0x3D9F, 0x3D9F, 0x3D9F, 0x3D9F, 0x3D9F, 0x3DA0, 0x3DA0, 0x3DA0, 0x3DA0,
    0x3DA0, 0x3DA1, 0x3DA1, 0x3DA1, 0x3DA1, 0x3DA1, 0x3DA2, 0x3DA2, 0x3DA2,
    0x3DA2, 0x3DA2, 0x3DA3, 0x3DA3, 0x3DA3, 0x3DA3, 0x3DA3, 0x3DA4, 0x3DA4,
    0x3DA4, 0x3DA4, 0x3DA4, 0x3DA5, 0x3DA5, 0x3DA5, 0x3DA5, 0x3DA5, 0x3DA6,
    0x3DA6, 0x3DA6, 0x3DA6, 0x3DA7, 0x3DA7, 0x3DA7, 0x3DA7, 0x3DA7, 0x3DA8,
    0x3DA8, 0x3DA8, 0x3DA8, 0x3DA8, 0x3DA9, 0x3DA9, 0x3DA9, 0x3DA9, 0x3DA9,
    0x3DAA, 0x3DAA, 0x3DAA, 0x3DAA, 0x3DAA, 0x3DAB, 0x3DAB, 0x3DAB, 0x3DAB,
    0x3DAB, 0x3DAC, 0x3DAC, 0x3DAC, 0x3DAC, 0x3DAC, 0x3DAD, 0x3DAD, 0x3DAD,
    0x3DAD, 0x3DAD, 0x3DAE, 0x3DAE, 0x3DAE, 0x3DAE, 0x3DAE, 0x3DAF, 0x3DAF,
    0x3DAF, 0x3DAF, 0x3DB0, 0x3DB0, 0x3DB0, 0x3DB0, 0x3DB0, 0x3DB1, 0x3DB1,
    0x3DB1, 0x3DB1, 0x3DB1, 0x3DB2, 0x3DB2, 0x3DB2, 0x3DB2, 0x3DB2, 0x3DB3,
    0x3DB3, 0x3DB3, 0x3DB3, 0x3DB3, 0x3DB4, 0x3DB4, 0x3DB4, 0x3DB4, 0x3DB4,
    0x3DB5, 0x3DB5, 0x3DB5, 0x3DB5, 0x3DB5, 0x3DB6, 0x3DB6, 0x3DB6, 0x3DB6,
    0x3DB6, 0x3DB7, 0x3DB7, 0x3DB7, 0x3DB7, 0x3DB8, 0x3DB8, 0x3DB8, 0x3DB8,
    0x3DB8, 0x3DB9, 0x3DB9, 0x3DB9, 0x3DB9, 0x3DB9, 0x3DBA, 0x3DBA, 0x3DBA,
    0x3DBA, 0x3DBA, 0x3DBB, 0x3DBB, 0x3DBB, 0x3DBB, 0x3DBB, 0x3DBC, 0x3DBC,
    0x3DBC, 0x3DBC, 0x3DBC, 0x3DBD, 0x3DBD, 0x3DBD, 0x3DBD, 0x3DBD, 0x3DBE,
    0x3DBE, 0x3DBE, 0x3DBE, 0x3DBE, 0x3DBF, 0x3DBF, 0x3DBF, 0x3DBF, 0x3DBF,
    0x3DC0, 0x3DC0, 0x3DC0, 0x3DC0, 0x3DC1, 0x3DC1, 0x3DC1, 0x3DC1, 0x3DC1,
    0x3DC2, 0x3DC2, 0x3DC2, 0x3DC2, 0x3DC2, 0x3DC3, 0x3DC3, 0x3DC3, 0x3DC3,
    0x3DC3, 0x3DC4, 0x3DC4, 0x3DC4, 0x3DC4, 0x3DC4, 0x3DC5, 0x3DC5, 0x3DC5,
    0x3DC5, 0x3DC5, 0x3DC6, 0x3DC6, 0x3DC6, 0x3DC6, 0x3DC6, 0x3DC7, 0x3DC7,
    0x3DC7, 0x3DC7, 0x3DC7, 0x3DC8, 0x3DC8, 0x3DC8, 0x3DC8, 0x3DC8, 0x3DC9,
    0x3DC9, 0x3DC9, 0x3DC9, 0x3DCA, 0x3DCA, 0x3DCA, 0x3DCA, 0x3DCA, 0x3DCB,
    0x3DCB, 0x3DCB, 0x3DCB, 0x3DCB, 0x3DCC, 0x3DCC, 0x3DCC, 0x3DCC, 0x3DCC,
    0x3DCD, 0x3DCE, 0x3DCF, 0x3DD0, 0x3DD1, 0x3DD2, 0x3DD3, 0x3DD4, 0x3DD5,
    0x3DD6, 0x3DD7, 0x3DD8, 0x3DD9, 0x3DDA, 0x3DDB, 0x3DDC, 0x3DDD, 0x3DDE,
    0x3DDF, 0x3DE0, 0x3DE1, 0x3DE2, 0x3DE3, 0x3DE4, 0x3DE5,
};

static u16 golden_bf16[] = {

    0x3fc9, 0x3fc9, 0x3fc9, 0x3fc9, 0x3fc9, 0x3fc8, 0x3fc9, 0x3fc8, 0x3fc8,
    0x3fc8, 0x3fc8, 0x3fc8, 0x3fc8, 0x3fc8, 0x3fc8, 0x3fc7, 0x3fc8, 0x3fc7,
    0x3fc7, 0x3fc7, 0x3fc7, 0x3fc7, 0x3fc7, 0x3fc7, 0x3fc6, 0x3fc6, 0x3fc6,
    0x3fc5, 0x3fc5, 0x3fc5, 0x3fc5, 0x3fc5, 0x3fc5, 0x3fc5, 0x3fc4, 0x3fc5,
    0x3fc4, 0x3fc4, 0x3fc4, 0x3fc4, 0x3fc4, 0x3fc4, 0x3fc4, 0x3fc3, 0x3fc3,
    0x3fc4, 0x3fc3, 0x3fc3, 0x3fc3, 0x3fc3, 0x3fc3, 0x3fc3, 0x3fc3, 0x3fc2,
    0x3fc2, 0x3fc1, 0x3fc1, 0x3fc1, 0x3fc1, 0x3fc1, 0x3fc1, 0x3fc1, 0x3fc0,
    0x3fc1, 0x3fc0, 0x3fc0, 0x3fc0, 0x3fc0, 0x3fc0, 0x3fc0, 0x3fc0, 0x3fbf,
    0x3fc0, 0x3fbf, 0x3fbf, 0x3fbf, 0x3fbf, 0x3fbf, 0x3fc9, 0x3fbe, 0x3fbe,
    0x3fbe, 0x3fbe, 0x3fbe, 0x3fbe, 0x3fbe, 0x3fbe, 0x3fbe, 0x3fbd, 0x3fbd,
    0x3fbc, 0x3fbc, 0x3fbc, 0x3fbc, 0x3fbc, 0x3fbc, 0x3fbc, 0x3fbb, 0x3fbc,
    0x3fbb, 0x3fbb, 0x3fbb, 0x3fbb, 0x3fbb, 0x3fbb, 0x3fba, 0x3fbb, 0x3fba,
    0x3fba, 0x3fba, 0x3fba, 0x3fba, 0x3fba, 0x3fb9, 0x3fba, 0x3fb9, 0x3fb9,
    0x3fb9, 0x3fb9, 0x3fb9, 0x3fb9, 0x3fb8, 0x3fb8, 0x3fb7, 0x3fb7, 0x3fb7,
    0x3fb7, 0x3fb7, 0x3fb7, 0x3fb6, 0x3fb7, 0x3fb6, 0x3fb6, 0x3fb6, 0x3fb6,
    0x3fb6, 0x3fb6, 0x3fb6, 0x3fb5, 0x3fb5, 0x3fb5, 0x3fb5, 0x3fb5, 0x3fb5,
    0x3fb4, 0x3fb4, 0x3fb4, 0x3fb4, 0x3fb4, 0x3fb4, 0x3fb4, 0x3fb4, 0x3fb4,
    0x3fb3, 0x3fb2, 0x3fb2, 0x3fb2, 0x3fb2, 0x3fb2, 0x3fb2, 0x3fb1, 0x3fb1,
    0x3fb1, 0x3fb1, 0x3fb1, 0x3fb1, 0x3fb1, 0x3fb0, 0x3fb0, 0x3fb0, 0x3fb0,
    0x3fb0, 0x3fb0, 0x3faf, 0x3faf, 0x3faf, 0x3faf, 0x3faf, 0x3faf, 0x3faf,
    0x3faf, 0x3fae, 0x3fae, 0x3fad, 0x3fad, 0x3fad, 0x3fad, 0x3fad, 0x3fac,
    0x3fac, 0x3fac, 0x3fac, 0x3fac, 0x3fac, 0x3fac, 0x3fac, 0x3fab, 0x3fab,
    0x3fab, 0x3fab, 0x3fab, 0x3faa, 0x3faa, 0x3faa, 0x3faa, 0x3faa, 0x3faa,
    0x3faa, 0x3faa, 0x3fa9, 0x3fa9, 0x3fa9, 0x3fa9, 0x3fa9, 0x3fa9, 0x3fa7,
    0x3fa7, 0x3fa7, 0x3fa7, 0x3fa7, 0x3fa7, 0x3fa7, 0x3fa6, 0x3fa6, 0x3fa6,
    0x3fa6, 0x3fa5, 0x3fa5, 0x3fa5, 0x3fa5, 0x3fa5, 0x3fa5, 0x3fa5, 0x3fa5,
    0x3fa4, 0x3fa4, 0x3fa4, 0x3fa4, 0x3fa4, 0x3fa3, 0x3fa3, 0x3fa3, 0x3fa3,
    0x3fa3, 0x3fa3, 0x3fa1, 0x3fa1, 0x3fa1, 0x3fa1, 0x3fa1, 0x3fa1, 0x3fa0,
    0x3fa0, 0x3fa0, 0x3fa0, 0x3fa0, 0x3f9f, 0x3f9f, 0x3f9f, 0x3f9f, 0x3f9f,
    0x3f9f, 0x3f9e, 0x3f9e, 0x3f9e, 0x3f9e, 0x3f9e, 0x3f9e, 0x3f9e, 0x3f9d,
    0x3f9d, 0x3f9d, 0x3f9d, 0x3f9d, 0x3f9c, 0x3f9c, 0x3f9c, 0x3f9c, 0x3f9c,
    0x3f9b, 0x3f9b, 0x3f9b, 0x3f9b, 0x3f9b, 0x3f9a, 0x3f9a, 0x3f99, 0x3f99,
    0x3f99, 0x3f99, 0x3f99, 0x3f99, 0x3f98, 0x3f98, 0x3f98, 0x3f97, 0x3f97,
    0x3f97, 0x3f97, 0x3f97, 0x3f96, 0x3f96, 0x3f96, 0x3f96, 0x3f96, 0x3f95,
    0x3f95, 0x3f95, 0x3f95, 0x3f95, 0x3f94, 0x3f94, 0x3fc9, 0x3fc9, 0x3f93,
    0x3f93, 0x3f93, 0x3f93, 0x3f93, 0x3f93, 0x3f93, 0x3f93, 0x3f92, 0x3f92,
    0x3f91, 0x3f91, 0x3f91, 0x3f91, 0x3f91, 0x3f91, 0x3f91, 0x3f90, 0x3f90,
    0x3f90, 0x3f90, 0x3f90, 0x3f8f, 0x3f8f, 0x3f8f, 0x3f8e, 0x3f8e, 0x3f8e,
    0x3f8e, 0x3f8e, 0x3f8d, 0x3f8d, 0x3f8d, 0x3f8c, 0x3f8c, 0x3f8c, 0x3f8c,
    0x3f8c, 0x3f8c, 0x3f8b, 0x3f8b, 0x3f8b, 0x3f8a, 0x3f8a, 0x3f8a, 0x3f8a,
    0x3f8a, 0x3f8a, 0x3f89, 0x3f89, 0x3f89, 0x3f89, 0x3f89, 0x3f88, 0x3f88,
    0x3f88, 0x3f87, 0x3f87, 0x3f87, 0x3f86, 0x3f86, 0x3f86, 0x3f86, 0x3f86,
    0x3f85, 0x3f85, 0x3f85, 0x3f85, 0x3f85, 0x3f84, 0x3f84, 0x3f83, 0x3f83,
    0x3f83, 0x3f83, 0x3f83, 0x3f82, 0x3f82, 0x3f82, 0x3f82, 0x3f82, 0x3f81,
    0x3f81, 0x3f80, 0x3f80, 0x3f80, 0x3f7f, 0x3f7f, 0x3f7f, 0x3f7f, 0x3f7f,
    0x3f7e, 0x3f7e, 0x3f7c, 0x3f7c, 0x3f7b, 0x3f7b, 0x3f7b, 0x3f7a, 0x3f7a,
    0x3f79, 0x3f79, 0x3f79, 0x3f78, 0x3f78, 0x3f77, 0x3f77, 0x3f77, 0x3f75,
    0x3f75, 0x3f74, 0x3f74, 0x3f74, 0x3f72, 0x3f72, 0x3f72, 0x3f72, 0x3f71,
    0x3f71, 0x3f71, 0x3f6f, 0x3f6f, 0x3f6e, 0x3f6e, 0x3f6e, 0x3f6d, 0x3f6d,
    0x3f6c, 0x3f6c, 0x3f6c, 0x3f6b, 0x3f6b, 0x3f69, 0x3f69, 0x3f69, 0x3f68,
    0x3f68, 0x3f67, 0x3f67, 0x3f65, 0x3f65, 0x3f65, 0x3f64, 0x3f64, 0x3f63,
    0x3f63, 0x3f63, 0x3f61, 0x3f61, 0x3f61, 0x3f61, 0x3f61, 0x3f60, 0x3f60,
    0x3f5e, 0x3f5e, 0x3f5e, 0x3f5e, 0x3f5e, 0x3f5c, 0x3f5c, 0x3f5b, 0x3f5b,
    0x3f5b, 0x3f59, 0x3f59, 0x3f58, 0x3f58, 0x3f58, 0x3f57, 0x3f57, 0x3f57,
    0x3f57, 0x3f57, 0x3f55, 0x3f55, 0x3f54, 0x3f54, 0x3f52, 0x3f52, 0x3f52,
    0x3f51, 0x3f51, 0x3f50, 0x3f50, 0x3f50, 0x3f4e, 0x3f4e, 0x3f4d, 0x3f4d,
    0x3f4d, 0x3f4c, 0x3f4c, 0x3f4b, 0x3f4b, 0x3f4b, 0x3f4a, 0x3f4a, 0x3f49,
    0x3f49, 0x3f47, 0x3f47, 0x3f47, 0x3f46, 0x3f46, 0x3f45, 0x3f45, 0x3f45,
    0x3f44, 0x3f44, 0x3f42, 0x3f42, 0x3f42, 0x3f41, 0x3f41, 0x3f40, 0x3f40,
    0x3f40, 0x3f3e, 0x3f3e, 0x3f3d, 0x3f3d, 0x3f3d, 0x3f3d, 0x3f3d, 0x3f3b,
    0x3f3b, 0x3f3a, 0x3f3a, 0x3f3a, 0x3f39, 0x3f39, 0x3f37, 0x3f37, 0x3f37,
    0x3f36, 0x3f36, 0x3f35, 0x3f35, 0x3f34, 0x3f34, 0x3f34, 0x3f33, 0x3f33,
    0x3f31, 0x3f31, 0x3f31, 0x3f30, 0x3f30, 0x3f2f, 0x3f2f, 0x3f2f, 0x3f2e,
    0x3f2e, 0x3f2d, 0x3f2d, 0x3f2d, 0x3f2b, 0x3f2b, 0x3f2a, 0x3f2a, 0x3f2a,
    0x3f2a, 0x3f2a, 0x3f27, 0x3f27, 0x3f26, 0x3f26, 0x3f26, 0x3f25, 0x3f25,
    0x3f25, 0x3f25, 0x3f25, 0x3f23, 0x3f23, 0x3f21, 0x3f21, 0x3f21, 0x3f21,
    0x3f21, 0x3f1f, 0x3f1f, 0x3f1e, 0x3f1e, 0x3f1e, 0x3f1d, 0x3f1d, 0x3f1c,
    0x3f1c, 0x3f1c, 0x3f1b, 0x3f1b, 0x3f1a, 0x3f1a, 0x3f1a, 0x3f18, 0x3f18,
    0x3f17, 0x3f17, 0x3f17, 0x3f16, 0x3f16, 0x3f15, 0x3f15, 0x3f14, 0x3f14,
    0x3f14, 0x3f13, 0x3f13, 0x3f11, 0x3f11, 0x3f11, 0x3f11, 0x3f10, 0x3f0f,
    0x3f0f, 0x3f0f, 0x3f0e, 0x3f0d, 0x3f0d, 0x3f0d, 0x3f0d, 0x3f0c, 0x3f0a,
    0x3f0a, 0x3f0a, 0x3f09, 0x3f09, 0x3f08, 0x3f08, 0x3f08, 0x3f07, 0x3f07,
    0x3f07, 0x3f06, 0x3f06, 0x3f05, 0x3f04, 0x3f04, 0x3f04, 0x3f04, 0x3f03,
    0x3f02, 0x3f02, 0x3f01, 0x3f01, 0x3f00, 0x3eff, 0x3efe, 0x3efe, 0x3efe,
    0x3efc, 0x3efa, 0x3ef9, 0x3ef9, 0x3ef9, 0x3ef7, 0x3ef6, 0x3ef4, 0x3ef4,
    0x3ef4, 0x3ef2, 0x3ef2, 0x3ef0, 0x3ef0, 0x3eef, 0x3eef, 0x3eed, 0x3eeb,
    0x3eeb, 0x3eea, 0x3eea, 0x3ee8, 0x3ee7, 0x3ee7, 0x3ee5, 0x3ee4, 0x3ee4,
    0x3ee4, 0x3ee4, 0x3ee2, 0x3ee0, 0x3ee0, 0x3edf, 0x3edf, 0x3edd, 0x3edd,
    0x3edb, 0x3edb, 0x3edb, 0x3eda, 0x3ed9, 0x3ed6, 0x3ed6, 0x3ed6, 0x3ed5,
    0x3ed3, 0x3ed2, 0x3ed2, 0x3ed1, 0x3ed0, 0x3ecf, 0x3ecf, 0x3ecf, 0x3ece,
    0x3ecd, 0x3eca, 0x3eca, 0x3ec9, 0x3ec9, 0x3ec8, 0x3ec6, 0x3ec6, 0x3ec6,
    0x3ec5, 0x3ec2, 0x3ec2, 0x3ec1, 0x3ec1, 0x3ec1, 0x3ebe, 0x3ebd, 0x3ebd,
    0x3ebd, 0x3ebd, 0x3ebb, 0x3eba, 0x3eba, 0x3eb9, 0x3eb9, 0x3eb6, 0x3eb5,
    0x3eb5, 0x3eb4, 0x3eb3, 0x3eb2, 0x3eb2, 0x3eb2, 0x3eb1, 0x3eb0, 0x3eae,
    0x3eae, 0x3ead, 0x3eac, 0x3eab, 0x3eaa, 0x3eaa, 0x3eaa, 0x3ea9, 0x3ea8,
    0x3ea6, 0x3ea6, 0x3ea5, 0x3ea5, 0x3ea4, 0x3ea2, 0x3ea2, 0x3ea2, 0x3ea1,
    0x3e9f, 0x3e9e, 0x3e9e, 0x3e9e, 0x3e9d, 0x3e9b, 0x3e9a, 0x3e9a, 0x3e99,
    0x3e99, 0x3e97, 0x3e96, 0x3e96, 0x3e95, 0x3e95, 0x3e94, 0x3e93, 0x3e92,
    0x3e91, 0x3e91, 0x3e91, 0x3e90, 0x3e8e, 0x3e8e, 0x3e8d, 0x3e8d, 0x3e8c,
    0x3e8b, 0x3e8a, 0x3e8a, 0x3e88, 0x3e88, 0x3e87, 0x3e87, 0x3e86, 0x3e84,
    0x3e83, 0x3e83, 0x3e82, 0x3e81, 0x3e80, 0x3e7f, 0x3e7f, 0x3e7d, 0x3e7b,
    0x3e79, 0x3e78, 0x3e76, 0x3e76, 0x3e74, 0x3e72, 0x3e71, 0x3e6f, 0x3e6f,
    0x3e6c, 0x3e6c, 0x3e69, 0x3e68, 0x3e67, 0x3e66, 0x3e64, 0x3e63, 0x3e62,
    0x3e61, 0x3e60, 0x3e5e, 0x3e5d, 0x3e5b, 0x3e5a, 0x3e56, 0x3e56, 0x3e55,
    0x3e53, 0x3e52, 0x3e4f, 0x3e4f, 0x3e4e, 0x3e4c, 0x3e4b, 0x3e48, 0x3e47,
    0x3e47, 0x3e45, 0x3e43, 0x3e41, 0x3e40, 0x3e40, 0x3e3e, 0x3e3d, 0x3e3a,
    0x3e3a, 0x3e38, 0x3e38, 0x3e36, 0x3e35, 0x3e33, 0x3e32, 0x3e31, 0x3e2f,
    0x3e2e, 0x3e2c, 0x3e2b, 0x3e2a, 0x3e28, 0x3e27, 0x3e26, 0x3e24, 0x3e24,
    0x3e21, 0x3e20, 0x3e1f, 0x3e1e, 0x3e1c, 0x3e1b, 0x3e19, 0x3e19, 0x3e17,
    0x3e16, 0x3e14, 0x3e14, 0x3e13, 0x3e12, 0x3e10, 0x3e0e, 0x3e0e, 0x3e0c,
    0x3e0b, 0x3e09, 0x3e08, 0x3e07, 0x3e06, 0x3e04, 0x3e03, 0x3e02, 0x3e00,
    0x3e00, 0x3dfd, 0x3dfb, 0x3df8, 0x3df5, 0x3df4, 0x3df1, 0x3ded, 0x3dec,
    0x3de9, 0x3de6, 0x3de5, 0x3de1, 0x3dde, 0x3ddd, 0x3dda, 0x3dd9, 0x3dd5,
    0x3dd2, 0x3dd1, 0x3dce, 0x3dcb, 0x3dca, 0x3dc7, 0x3dc5, 0x3dc3, 0x3dc1,
    0x3dbf, 0x3dbc, 0x3dba, 0x3db8, 0x3db5, 0x3db2, 0x3db1, 0x3dad, 0x3daa,
    0x3da8, 0x3da5, 0x3da3, 0x3da1, 0x3d9f, 0x3d9c, 0x3d9a, 0x3d99, 0x3d96,
    0x3d94, 0x3d91, 0x3d90, 0x3d8d, 0x3d8b, 0x3d89, 0x3d86, 0x3d84, 0x3d82,
    0x3d7e, 0x3d79, 0x3d74, 0x3d70, 0x3d6c, 0x3d67, 0x3d63, 0x3d60, 0x3d5b,
    0x3d57, 0x3d53, 0x3d50, 0x3d49, 0x3d45, 0x3d41, 0x3d3c, 0x3d38, 0x3d34,
    0x3d30, 0x3d2b, 0x3d27, 0x3d22, 0x3d1e, 0x3d19, 0x3d15, 0x3d11, 0x3d0c,
    0x3d08, 0x3d04, 0x3cff, 0x3cf8, 0x3cef, 0x3ce7, 0x3cdf, 0x3cd6, 0x3cce,
    0x3cc4, 0x3cbc, 0x3cb2, 0x3caa, 0x3ca1, 0x3c98, 0x3c8f, 0x3c87, 0x3c7e,
    0x3c6b, 0x3c5a, 0x3c4a, 0x3c39, 0x3c29, 0x3c1a, 0x3c09, 0x3bf4, 0x3bd6,
    0x3bb4, 0x3b95, 0x3b71, 0x3b32, 0x3aec, 0x3a6c, 0x0,
};

// <! gen atan2 f(y, x) = 2 * atan(y / (pow(x*x+y*y, 0.5) + x))
static double _gen_atan2(float y, float x)
{
  return atan2(y, x);
}

static void tl_lut_ref(u16 *ofmap, u16 *ifmap, u16 *ifmap2,
                       tl_shape_t ifmap_shape)
{
  assert(ofmap);

  for (u32 i = 0; i < tl_shape_size(&ifmap_shape); i++) {
    float y = convert_bf16_fp32(ifmap2[i]);
    float x = convert_bf16_fp32(ifmap[i]);
    double v = _gen_atan2(y, x);
    ofmap[i] = convert_fp32_bf16(v);

    if (mode == PRE_DATA_COMPARE_FIX) {
      ofmap[i] = golden_bf16[i];
    } else if (mode == DATA_COMPARE_U8) {
      ofmap[i] = (u8)convert_bf16_s8(ofmap[i]);
    }
  }
}

static bool verify(u16 *ofmap_data, u16 *ref_data, u16 *ifmap, u16 *ifmap2,
                   u64 ifmap_size, float epsilon)
{
  u64 size = ifmap_size;

  for (u64 i = 0; i < size; i++) {
    bool is_close;
    u16 ref = ref_data[i];
    u16 ofmap_data_bf16;
    float ref_f;
    float ofmap_data_f;

    ref_f = convert_bf16_fp32(ref);
    ofmap_data_f = convert_bf16_fp32(ofmap_data[i]);
    ofmap_data_bf16 = ofmap_data[i];

    if (mode == PRE_DATA_COMPARE_FIX) {
      is_close = ofmap_data[i] == ref;
    } else {
      is_close = fabs(ref_f - ofmap_data_f) < epsilon;
    }

    if (!is_close) {
      float y = convert_bf16_fp32(ifmap2[i]);
      float x = convert_bf16_fp32(ifmap[i]);
      fprintf(stderr,
              "comparing failed at ofmap_data[%" PRIu64 "]\n"
              "\tgot %x, exp %x, fp32: got %f exp %f, atan2(%f, %f) = %f"
              "\ty %f(0x%x), x %f(0x%x)\n",
              i, ofmap_data_bf16, ref, ofmap_data_f, ref_f, y, x,
              _gen_atan2(y, x), y, ifmap2[i], x, ifmap[i]);
      exit(-1);
    }
  }

  return true;
}

static void _gen_input(u16 *input_data, u64 ifmap_size, int range_start,
                       int range_end)
{
  std::random_device rd;
  std::mt19937 e2(rd());
  std::uniform_real_distribution<> dist(range_start, range_end);

  float LO = pow(2, range_start);
  float HI = pow(2, range_end);
  for (u64 i = 0; i < ifmap_size; i++) {
    // input range is -8 ~ +8
    int table_hw = 256;
    float input = ((int)i % (range_end - 2)) * (((int)i % 2) ? 1 : -1) + 0.03 +
                  (i % table_hw) * 0.002;
    input = ((int)i % (range_end - 2)) * (((int)i % 2) ? 1 : 1) + 0.03 +
            (i % table_hw) * 0.002;
    input_data[i] = convert_fp32_bf16(input);
    input = dist(e2);
    input = LO + static_cast<float>(rand()) /
                     (static_cast<float>(RAND_MAX / (HI - LO)));
  }
}

static void gen_input(u16 *x, u16 *y, u64 ifmap_size, TEST_MODE mode,
                      int range_start, int range_end)
{

  if (mode == PRE_DATA_COMPARE_FIX) {
    memcpy(x, &test_pattern, sizeof(test_pattern));
  } else {
    range_start = abs(range_start);
    range_end = abs(range_end);
    _gen_input(x, ifmap_size, range_start, range_end);
  }

  // invert for test
  for (u64 i = 0; i < ifmap_size; i++) {
    y[i] = x[(ifmap_size - 1) - i];
  }

  if (mode == DATA_COMPARE_ACCURACY_X_GT_0) {
    // y = any
    u32 i = 0;
    for (; i < ifmap_size / 4; i++) {
      // y < 0
      y[i] = convert_fp32_bf16(-1 * convert_fp32_bf16(y[i]));
      y[i + ifmap_size / 4] = convert_fp32_bf16(0);
    }
  } else if (mode == DATA_COMPARE_ACCURACY_X_LT_0_Y_GE_0) {
    // x < 0 and y >= 0
    for (u32 i = 0; i < ifmap_size; i++) {
      x[i] = convert_fp32_bf16(-1 * convert_fp32_bf16(x[i]));
    }

    for (u32 i = 0; i < ifmap_size / 4; i++) {
      y[i + ifmap_size / 4] = convert_fp32_bf16(0);
    }
  } else if (mode == DATA_COMPARE_ACCURACY_X_LT_0_Y_LT_0) {
    // x < 0 and y < 0
    for (u32 i = 0; i < ifmap_size; i++) {
      x[i] = convert_fp32_bf16(-1 * convert_fp32_bf16(x[i]));
      y[i] = convert_fp32_bf16(-1 * convert_fp32_bf16(y[i]));
    }
  } else if (mode == DATA_COMPARE_ACCURACY_X_0_Y_GT_0) {
    // pi / 2, x = 0 and y > 0
    for (u32 i = 0; i < ifmap_size; i++) {
      x[i] = convert_fp32_bf16(0);
    }
  } else if (mode == DATA_COMPARE_ACCURACY_X_0_Y_LT_0) {
    // -pi / 2, x = 0 and y < 0
    for (u32 i = 0; i < ifmap_size; i++) {
      x[i] = convert_fp32_bf16(0);
      y[i] = convert_fp32_bf16(-1 * convert_fp32_bf16(y[i]));
    }
  }

  if (mode != PRE_DATA_COMPARE_FIX) {
    int i = 0;
    y[i] = convert_fp32_bf16(5.000000);
    x[i++] = convert_fp32_bf16(-125.000000);
    y[i] = convert_fp32_bf16(1.070312);
    x[i++] = convert_fp32_bf16(0.498046);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-8.000000);
    x[i] = convert_fp32_bf16(424.000);
    y[i++] = convert_fp32_bf16(-1.00);
    x[i] = convert_fp32_bf16(2.484375);
    y[i++] = convert_fp32_bf16(-7.531250);
    x[i] = convert_fp32_bf16(-2.484375);
    y[i++] = convert_fp32_bf16(-7.531250);
    x[i] = convert_fp32_bf16(0);
    y[i++] = convert_fp32_bf16(7.531250);
    x[i] = convert_fp32_bf16(0);
    y[i++] = convert_fp32_bf16(-7.531250);
    x[i] = convert_fp32_bf16(0);
    y[i++] = convert_fp32_bf16(0);
    x[i] = convert_fp32_bf16(0);
    y[i++] = convert_fp32_bf16(0.394531);
    y[i] = convert_fp32_bf16(-4.000000);
    x[i++] = convert_fp32_bf16(-64.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-4.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-40.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-53.000000);
    y[i] = convert_fp32_bf16(-9.000000);
    x[i++] = convert_fp32_bf16(-91.000000);
    y[i] = convert_fp32_bf16(12.000000);
    x[i++] = convert_fp32_bf16(-164.000000);
    y[i] = convert_fp32_bf16(-20.000000);
    x[i++] = convert_fp32_bf16(-320.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-71.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-155.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-247.000000);
    y[i] = convert_fp32_bf16(-2.000000);
    x[i++] = convert_fp32_bf16(-118.000000);
    y[i] = convert_fp32_bf16(-2.000000);
    x[i++] = convert_fp32_bf16(-54.000000);
    y[i] = convert_fp32_bf16(-5.000000);
    x[i++] = convert_fp32_bf16(-392.000000);
    y[i] = convert_fp32_bf16(-37.000000);
    x[i++] = convert_fp32_bf16(-520.000000);
    y[i] = convert_fp32_bf16(-1.000000);
    x[i++] = convert_fp32_bf16(-19.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-10.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-8.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-2.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-14.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-2.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-6.000000);
    y[i] = convert_fp32_bf16(-1.000000);
    x[i++] = convert_fp32_bf16(-21.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-14.000000);
    y[i] = convert_fp32_bf16(-1.000000);
    x[i++] = convert_fp32_bf16(-17.000000);
    y[i] = convert_fp32_bf16(-1.000000);
    x[i++] = convert_fp32_bf16(-17.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-8.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-4.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-10.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-8.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-14.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-4.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-2.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-41.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-69.000000);
    y[i] = convert_fp32_bf16(4.000000);
    x[i++] = convert_fp32_bf16(-86.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-41.000000);
    y[i] = convert_fp32_bf16(-2.000000);
    x[i++] = convert_fp32_bf16(-34.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-6.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-41.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-136.000000);
    y[i] = convert_fp32_bf16(-3.000000);
    x[i++] = convert_fp32_bf16(-79.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-38.000000);
    y[i] = convert_fp32_bf16(5.000000);
    x[i++] = convert_fp32_bf16(-173.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-78.000000);
    y[i] = convert_fp32_bf16(-2.000000);
    x[i++] = convert_fp32_bf16(-60.000000);
    y[i] = convert_fp32_bf16(3.000000);
    x[i++] = convert_fp32_bf16(-123.000000);
    y[i] = convert_fp32_bf16(-9.000000);
    x[i++] = convert_fp32_bf16(-280.000000);
    y[i] = convert_fp32_bf16(3.000000);
    x[i++] = convert_fp32_bf16(-39.000000);
    y[i] = convert_fp32_bf16(2.000000);
    x[i++] = convert_fp32_bf16(-524.000000);
    y[i] = convert_fp32_bf16(11.000000);
    x[i++] = convert_fp32_bf16(-376.000000);
    y[i] = convert_fp32_bf16(5.000000);
    x[i++] = convert_fp32_bf16(-131.000000);
    y[i] = convert_fp32_bf16(11.000000);
    x[i++] = convert_fp32_bf16(-324.000000);
    y[i] = convert_fp32_bf16(9.000000);
    x[i++] = convert_fp32_bf16(-125.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-92.000000);
    y[i] = convert_fp32_bf16(-7.000000);
    x[i++] = convert_fp32_bf16(-233.000000);
    y[i] = convert_fp32_bf16(10.000000);
    x[i++] = convert_fp32_bf16(-170.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-4.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-4.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-10.000000);
    y[i] = convert_fp32_bf16(-1.000000);
    x[i++] = convert_fp32_bf16(-23.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-6.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-6.000000);
    y[i] = convert_fp32_bf16(-3.000000);
    x[i++] = convert_fp32_bf16(-37.000000);

    y[i] = convert_fp32_bf16(-9);
    x[i++] = convert_fp32_bf16(-1);

    y[i] = convert_fp32_bf16(7.0);
    x[i++] = convert_fp32_bf16(-1);

    y[i] = convert_fp32_bf16(0);
    x[i++] = convert_fp32_bf16(-1);
  }

#ifdef DBG
  for (u64 i = 0; i < ifmap_size; i++) {
    printf("source[%ld] y %f x %f\n", i, convert_bf16_fp32(y[i]),
           convert_bf16_fp32(x[i]));
  }
#endif /* ifdef DBG */
}

static void testbench(CVI_RT_HANDLE *ctx, bmk_ctx_t *bmk)
{
  // TODO: check more shape / align
  bmk1880v2_chip_info_t chip_info = bmk1880v2_chip_info();

  u32 input_n = 1;
  u32 input_c = chip_info.npu_num;
  u32 input_h = 16;
  u32 input_w = 16;
  float epsilon = 0.1;
  int range_start = -8;
  int range_end = 8;

  if (mode == PRE_DATA_COMPARE_FIX) {
    input_h = 4;
    input_w = 8;
  }

  fmt_t fmt = FMT_BF16;

  tl_shape_t ifmap_shape = {input_n, input_c, input_h, input_w};
  tl_shape_t ofmap_shape = ifmap_shape;

  // get lut table shape and size
  tl_shape_t table_shape;
  u64 table_bytesize = bf16_lut_tbl_bytesize(bmk, &table_shape, fmt);

  // get input / output size
  u64 ifmap_size = tl_shape_size(&ifmap_shape);
  u64 ofmap_size = tl_shape_size(&ofmap_shape);
  int data_type_size = bytesize_of_fmt(fmt);
  u64 ifmap_bytesize = ifmap_size * data_type_size;
  u64 ofmap_bytesize = ofmap_size * data_type_size;

  // atan2 was two inputs
  tl_t *tl_ifmap = alloc_tl(bmk, ifmap_shape, fmt, /*align*/1);
  tl_t *tl_ifmap2 = alloc_tl(bmk, ifmap_shape, fmt, /*align*/1);
  tl_t *tl_ofmap_bf16 = alloc_tl(bmk, ofmap_shape, fmt, /*align*/1);
  tl_t *out = tl_ofmap_bf16;

  // atan buf
  tl_t *tl_y0_buf = alloc_tl(bmk, table_shape, fmt, /*align*/1);
  tl_t *tl_slope_buf = alloc_tl(bmk, table_shape, fmt, /*align*/1);
  tl_t *tl_invert_buf = alloc_tl(bmk, table_shape, fmt, /*align*/1);
  tl_t *tl_pos_neg_buf = alloc_tl(bmk, table_shape, fmt, /*align*/1);

  // reciprocal buf
  tl_t *tl_reciprocal_table_answer =
      alloc_tl(bmk, table_shape, fmt, /*align*/1);
  tl_t *tl_reciprocal_table_answer_mantissa =
      alloc_tl(bmk, table_shape, fmt, /*align*/1);

  // temp buf
  tl_t *tl_buf = alloc_tl(bmk, ofmap_shape, fmt, /*align*/1);
  tl_t *tl_buf2 = alloc_tl(bmk, tl_ofmap_bf16->shape, fmt, /*align*/1);
  tl_t *tl_buf3 = alloc_tl(bmk, tl_ofmap_bf16->shape, fmt, /*align*/1);
  tl_t *tl_buf4 = alloc_tl(bmk, tl_ofmap_bf16->shape, fmt, /*align*/1);
  tl_t *tl_buf5 = alloc_tl(bmk, tl_ofmap_bf16->shape, fmt, /*align*/1);
  tl_t *tl_buf6 = alloc_tl(bmk, tl_ofmap_bf16->shape, fmt, /*align*/1);

  // sqrt buf
  tl_t *tl_sqrt_table_answer = alloc_tl(bmk, table_shape, fmt, /*align*/1);
  tl_t *tl_sqrt_table_answer_mantissa =
      alloc_tl(bmk, table_shape, fmt, /*align*/1);

  // search '0' index
  tl_t *tl_0_idx_table = alloc_tl(bmk, table_shape, fmt, /*align*/1);

  u16 *input_data = (u16 *)xmalloc(ifmap_bytesize);
  u16 *input_data2 = (u16 *)xmalloc(ifmap_bytesize);
  u16 *ref_data = (u16 *)xmalloc(ofmap_bytesize);

  // for reciprocal
  u16 *table_reciprocal_data = (u16 *)xmalloc(table_bytesize);
  u16 *table_reciprocal_data_mantissa = (u16 *)xmalloc(table_bytesize);

  // for atan
  u16 *table_data_atan_y0 = (u16 *)xmalloc(table_bytesize);
  u16 *table_data_atan_slope = (u16 *)xmalloc(table_bytesize);
  u16 *table_data_atan_invert = (u16 *)xmalloc(table_bytesize);
  u16 *table_data_atan_pos_neg = (u16 *)xmalloc(table_bytesize);

  // for sqrt
  u16 *sqrt_table_data = (u16 *)xmalloc(table_bytesize);
  u16 *sqrt_table_data_mantissa = (u16 *)xmalloc(table_bytesize);

  // for search '0' index
  u16 *idx_0_table_data = (u16 *)xmalloc(table_bytesize);

  // init input / ref
  // input_data is x, input_data2 is y
  gen_input(input_data, input_data2, ifmap_size, mode, range_start, range_end);
  tl_lut_ref(ref_data, input_data, input_data2, ifmap_shape);

  // init lut table
  bf16_reciprocal_tbl(table_reciprocal_data, table_reciprocal_data_mantissa,
                      &table_shape);
  bf16_sqrt_tbl(sqrt_table_data, sqrt_table_data_mantissa, &table_shape);
  bf16_atan_tbl(table_data_atan_y0, table_data_atan_slope,
                table_data_atan_invert, table_data_atan_pos_neg, &table_shape);
  bf16_gen_0_tbl(idx_0_table_data, &table_shape);

  // sys->local
  put_bf16_tensor_g2l(ctx, bmk, tl_ifmap, (u16 *)input_data, fmt);
  put_bf16_tensor_g2l(ctx, bmk, tl_ifmap2, (u16 *)input_data2, fmt);
  put_bf16_tensor_g2l(ctx, bmk, tl_reciprocal_table_answer,
                      (u16 *)table_reciprocal_data, fmt);
  put_bf16_tensor_g2l(ctx, bmk, tl_reciprocal_table_answer_mantissa,
                      (u16 *)table_reciprocal_data_mantissa, fmt);

  put_bf16_tensor_g2l(ctx, bmk, tl_y0_buf, (u16 *)table_data_atan_y0, fmt);
  put_bf16_tensor_g2l(ctx, bmk, tl_slope_buf, (u16 *)table_data_atan_slope,
                      fmt);
  put_bf16_tensor_g2l(ctx, bmk, tl_invert_buf, (u16 *)table_data_atan_invert,
                      fmt);
  put_bf16_tensor_g2l(ctx, bmk, tl_pos_neg_buf, (u16 *)table_data_atan_pos_neg,
                      fmt);
  put_bf16_tensor_g2l(ctx, bmk, tl_sqrt_table_answer, (u16 *)sqrt_table_data,
                      fmt);
  put_bf16_tensor_g2l(ctx, bmk, tl_sqrt_table_answer_mantissa,
                      (u16 *)sqrt_table_data_mantissa, fmt);
  put_bf16_tensor_g2l(ctx, bmk, tl_0_idx_table, (u16 *)idx_0_table_data, fmt);

  bf16_atan2_emit(bmk, tl_ifmap2, tl_ifmap, tl_buf, tl_buf2, tl_buf3, tl_buf4,
                  tl_buf5, tl_buf6, tl_y0_buf, tl_slope_buf, tl_invert_buf,
                  tl_pos_neg_buf, tl_reciprocal_table_answer,
                  tl_reciprocal_table_answer_mantissa, tl_sqrt_table_answer,
                  tl_sqrt_table_answer_mantissa, tl_0_idx_table,
                  OUT tl_ofmap_bf16, fmt);

  u16 *ofmap_data = (u16 *)get_bf16_tensor_l2g(ctx, bmk, out, out->fmt);
  verify(ofmap_data, ref_data, input_data, input_data2, ifmap_size, epsilon);

  free_tl(bmk, tl_0_idx_table);
  free_tl(bmk, tl_sqrt_table_answer_mantissa);
  free_tl(bmk, tl_sqrt_table_answer);
  free_tl(bmk, tl_buf6);
  free_tl(bmk, tl_buf5);
  free_tl(bmk, tl_buf4);
  free_tl(bmk, tl_buf3);
  free_tl(bmk, tl_buf2);
  free_tl(bmk, tl_buf);
  free_tl(bmk, tl_reciprocal_table_answer_mantissa);
  free_tl(bmk, tl_reciprocal_table_answer);
  free_tl(bmk, tl_pos_neg_buf);
  free_tl(bmk, tl_invert_buf);
  free_tl(bmk, tl_slope_buf);
  free_tl(bmk, tl_y0_buf);
  free_tl(bmk, tl_ofmap_bf16);
  free_tl(bmk, tl_ifmap2);
  free_tl(bmk, tl_ifmap);

  free(input_data);
  free(input_data2);
  free(ref_data);
  free(table_reciprocal_data);
  free(table_reciprocal_data_mantissa);

  free(table_data_atan_y0);
  free(table_data_atan_slope);
  free(table_data_atan_invert);
  free(table_data_atan_pos_neg);
  free(sqrt_table_data);
  free(sqrt_table_data_mantissa);
  free(idx_0_table_data);
  free(ofmap_data);
}

int main()
{
  CVI_RT_HANDLE ctx;
  bmk_ctx_t *bmk;
  int round_mode;

  round_mode = set_store_feround();

  test_init(&ctx, &bmk);

  // for (int i = PRE_DATA_COMPARE_FIX; i < TEST_MODE_MAX; i++)
  // for (int i = PRE_DATA_COMPARE_FIX; i < DATA_COMPARE_ACCURACY; i++) {
  // for (int i = DATA_COMPARE_ACCURACY; i < DATA_COMPARE_U8; i++) {
  // for (int i = DATA_COMPARE_ACCURACY; i < DATA_COMPARE_ACCURACY_X_GT_0; i++)
  // {
  for (int i = PRE_DATA_COMPARE_FIX; i < DATA_COMPARE_U8; i++) {
    mode = static_cast<TEST_MODE>(i);
    printf("test mode %d...\n", mode);
    testbench(&ctx, bmk);
  }
  printf("pass\n");

  test_exit(&ctx);
  restore_feround(round_mode);
  return 0;
}
