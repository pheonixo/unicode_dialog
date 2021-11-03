/*
  GObject
    ╰── GInitiallyUnowned
          ╰── GtkWidget
                ╰── GtkContainer
                     ╰── GtkBin
                           ╰── GtkWindow
                ╰── GtkContainer
                      ╰── GtkLayout  you must draw to GTK_LAYOUT(layout)->bin_window?
                ╰── GtkDrawingArea


head________0__1__2_________________________________________
    |______|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|
      row#  c0 c1 c2 ...

*/
#include <gtk/gtk.h>

extern int isprint(int);

#define FONT_NAME "Noto Sans"

  // list of a single side
#define CELL_BORDER 1

#define CELL_WIDTH  32
#define CELL_HEIGHT CELL_WIDTH
#define ROW_ID_WIDTH ((CELL_WIDTH - 4) * 5)
#define FONT_PADDING 6

#define UNMAPPED_REGIONS   ((0x2FF0 - 0x2FE0) + (0xF900 - 0xD800) + (0x10280 - 0x10200) \
+ (0x10400 - 0x103E0) + (0x10600 - 0x105C0) + (0x10800 - 0x107C0) + (0x108E0 - 0x108B0) \
+ (0x10980 - 0x10940) + (0x10AC0 - 0x10AA0) + (0x10C00 - 0x10BB0) + (0x10C80 - 0x10C50) \
+ (0x10E60 - 0x10D40) + (0x10F00 - 0x10EC0) + (0x11280 - 0x11250) + (0x11400 - 0x11380) \
+ (0x11580 - 0x114E0) + (0x11700 - 0x116D0) + (0x11800 - 0x11750) + (0x118A0 - 0x11850) \
+ (0x119A0 - 0x11960) + (0x11C00 - 0x11B00) + (0x11D00 - 0x11CC0) + (0x11EE0 - 0x11DB0) \
+ (0x11FB0 - 0x11F00) + (0x12F90 - 0x12550) + (0x14400 - 0x13440) + (0x16800 - 0x14680) \
+ (0x16E40 - 0x16B90) + (0x16F00 - 0x16EA0) + (0x16FE0 - 0x16FA0) + (0x1AFF0 - 0x18D80) \
+ (0x1BC00 - 0x1B300) + (0x1CF00 - 0x1BCB0) + (0x1D000 - 0x1CFD0) + (0x1D2E0 - 0x1D250) \
+ (0x1D400 - 0x1D380) + (0x1DF00 - 0x1DAB0) + (0x1E100 - 0x1E030) + (0x1E290 - 0x1E150) \
+ (0x1E7E0 - 0x1E300) + (0x1E900 - 0x1E8E0) + (0x1EC70 - 0x1E960) + (0x1ED00 - 0x1ECC0) \
+ (0x1EE00 - 0x1ED50))

typedef GdkRectangle cell;
static cell scell, *selected_cell = NULL;
static int line_no = 0;

static struct _coverage {
  unsigned start, end;
  char *font_name;
} coverage_list[] = {
  {  0x03E2, 0x03EF, "Noto Sans Coptic"  },
  {  0x0530, 0x058F, "Noto Sans Armenian"  },
  {  0x0590, 0x05FF, "Noto Sans Hebrew"  },
  {  0x0600, 0x06FF, "Noto Naskh Arabic"  },
  {  0x0700, 0x074F, "Noto Sans Syriac"  },
  {  0x0750, 0x077F, "Noto Naskh Arabic"  },
  {  0x0780, 0x07BF, "Noto Sans Thaana"  },
  {  0x07C0, 0x07FF, "Noto Sans NKo"  },
  {  0x0800, 0x083F, "Noto Sans Samaritan"  },
  {  0x0840, 0x085F, "Noto Sans Mandaic"  },
  {  0x0860, 0x086F, "Noto Sans Syriac"  },  //missing
  {  0x0870, 0x089F, "Noto Sans Arabic"  },  //missing
  {  0x08A0, 0x08FF, "Noto Sans Arabic"  },
  {  0x0900, 0x097F, "Noto Sans Devanagari"  },
  {  0x0980, 0x09FF, "Noto Sans Bengali"  },
  {  0x0A00, 0x0A7F, "Noto Sans Gurmukhi"  },
  {  0x0A80, 0x0AFF, "Noto Sans Gujarati"  },
  {  0x0B00, 0x0B7F, "Noto Sans Oriya"  },
  {  0x0B80, 0x0BFF, "Noto Sans Tamil"  },
  {  0x0C00, 0x0C7F, "Noto Sans Telugu"  },
  {  0x0C80, 0x0CFF, "Noto Sans Kannada"  },
  {  0x0D00, 0x0D7F, "Noto Sans Malayalam"  },
  {  0x0D80, 0x0DFF, "Noto Sans Sinhala"  },
  {  0x0E00, 0x0E7F, "Noto Sans Thai"  },
  {  0x0E80, 0x0EFF, "Noto Sans Lao"  },
  {  0x0F00, 0x0FFF, "Noto Serif Tibetan"  },
  {  0x1000, 0x109F, "Noto Sans Myanmar"  },
  {  0x10A0, 0x10FF, "Noto Sans Georgian"  },
  {  0x1100, 0x11FF, "Noto Sans CJK KR"  },
  {  0x1200, 0x139F, "Noto Sans Ethiopic"  },
  {  0x13A0, 0x13FF, "Noto Sans Cherokee"  },
  {  0x1400, 0x167F, "Noto Sans Canadian Aboriginal"  },
  {  0x1680, 0x169F, "Noto Sans Ogham"  },
  {  0x16A0, 0x16FF, "Noto Sans Runic"  },
  {  0x1700, 0x171F, "Noto Sans Tagalog"  },
  {  0x1720, 0x173F, "Noto Sans Hanunoo"  },
  {  0x1740, 0x175F, "Noto Sans Buhid"  },
  {  0x1760, 0x177F, "Noto Sans Tagbanwa"  },
  {  0x1780, 0x17FF, "Noto Sans Khmer"  },
  {  0x1800, 0x18AF, "Noto Sans Mongolian"  },
  {  0x18B0, 0x18FF, "Noto Sans Canadian Aboriginal"  },
  {  0x1900, 0x194F, "Noto Sans Limbu"  },
  {  0x1950, 0x197F, "Noto Sans Tai Le"  },
  {  0x1980, 0x19DF, "Noto Sans New Tai Lue"  },
  {  0x19E0, 0x19FF, "Noto Sans Khmer"  },
  {  0x1A00, 0x1A1F, "Noto Sans Buginese"  },
  {  0x1A20, 0x1AAF, "Noto Sans Tai Tham"  },
  {  0x1AB0, 0x1AFF, "Noto Sans"  },
  {  0x1B00, 0x1B7F, "Noto Sans Balinese"  },
  {  0x1B80, 0x1BBF, "Noto Sans Sundanese"  },
  {  0x1BC0, 0x1BFF, "Noto Sans Batak"  },
  {  0x1C00, 0x1C4F, "Noto Sans Lepcha"  },
  {  0x1C50, 0x1C7F, "Noto Sans Ol Chiki"  },
  {  0x1C80, 0x1C8F, "Noto Sans"  },
  {  0x1C90, 0x1CBF, "Noto Sans Georgian"  },
  {  0x1CC0, 0x1CCF, "Noto Sans Sundanese"  },
  {  0x1CD0, 0x1CF6, "Noto Sans Devanagari"  },
  {  0x1CF7, 0x1CF7, "Noto Sans Bengali"  },
  {  0x1CF8, 0x1CFF, "Noto Sans Devanagari"  },
  {  0x1D00, 0x20CF, "Noto Sans"  },
  {  0x20D0, 0x20DC, "Noto Sans Math"  },
  {  0x20DD, 0x20E0, "Noto Sans Symbols"  },
  {  0x20E1, 0x20E1, "Noto Sans Math"  },
  {  0x20E2, 0x20E4, "Noto Sans Symbols"  },
  {  0x20E5, 0x20EF, "Noto Sans Math"  },
  {  0x20F0, 0x20F0, "Noto Sans"  },
  {  0x20F1, 0x20FF, "Noto Sans Math"  },
  {  0x2100, 0x215F, "Noto Sans"  },
  {  0x2160, 0x2183, "Noto Sans Symbols"  },
  {  0x2184, 0x2184, "Noto Sans"  },
  {  0x2185, 0x2188, "Noto Sans Symbols"  },
  {  0x2189, 0x2189, "Noto Sans"  },
  {  0x218A, 0x2199, "Noto Sans Symbols"  },
  {  0x219A, 0x21AE, "Noto Sans Math"  },
  {  0x21AF, 0x21AF, "Noto Sans Symbols 2"  },
  {  0x21B0, 0x21E5, "Noto Sans Math"  },
  {  0x21E6, 0x21F0, "Noto Sans Symbols 2"  },
  {  0x21F1, 0x21F2, "Noto Sans Math"  },
  {  0x21F3, 0x21F3, "Noto Sans Symbols 2"  },
  {  0x21F4, 0x22FF, "Noto Sans Math"  },
  {  0x2300, 0x230F, "Noto Sans Symbols"  },
  {  0x2310, 0x2310, "Noto Sans Math"  },
  {  0x2311, 0x2315, "Noto Sans Symbols"  },
  {  0x2316, 0x2316, "Noto Sans Symbols 2"  },
  {  0x2317, 0x2317, "Noto Sans Symbols"  },
  {  0x2318, 0x2318, "Noto Sans Symbols 2"  },
  {  0x2319, 0x2319, "Noto Sans Math"  },
  {  0x231A, 0x231B, "Noto Sans Symbols 2"  },
  {  0x231C, 0x231F, "Noto Sans Symbols"  },
  {  0x2320, 0x2321, "Noto Sans Math"  },
  {  0x2322, 0x2323, "Noto Sans Symbols"  },
  {  0x2324, 0x2328, "Noto Sans Symbols 2"  },
  {  0x2329, 0x232A, "Noto Sans Symbols"  },
  {  0x232B, 0x232B, "Noto Sans Symbols 2"  },
  {  0x232C, 0x2335, "Noto Sans Symbols"  },
  {  0x2336, 0x237A, "Noto Sans Math"  },
  {  0x237B, 0x237B, "Noto Sans Symbols 2"  },
  {  0x237C, 0x237C, "Noto Sans Symbols"  },
  {  0x237D, 0x237F, "Noto Sans Symbols 2"  },
  {  0x2380, 0x2394, "Noto Sans Symbols"  },
  {  0x2395, 0x2395, "Noto Sans Math"  },
  {  0x2396, 0x239A, "Noto Sans Symbols"  },
  {  0x239B, 0x23B6, "Noto Sans Math"  },
  {  0x23B7, 0x23BD, "Noto Sans Mono"  },
  {  0x23BE, 0x23CD, "Noto Sans Symbols"  },
  {  0x23CE, 0x23CF, "Noto Sans Symbols 2"  },
  {  0x23D0, 0x23DB, "Noto Sans Symbols"  },
  {  0x23DC, 0x23E1, "Noto Sans Math"  },
  {  0x23E2, 0x23E8, "Noto Sans Symbols"  },
  {  0x23E9, 0x2426, "Noto Sans Symbols 2"  },
    // 0x23EB, 0x23EC
    // 0x23F0
    // 0x2427, 0x243F, ""
  {  0x2440, 0x245F, "Noto Sans Symbols 2"  },
  {  0x2460, 0x24FF, "Noto Sans Symbols"  },
  {  0x2500, 0x25FF, "Noto Sans Mono"  },
  {  0x2600, 0x2609, "Noto Sans Symbols 2"  },
  {  0x260A, 0x260D, "Noto Sans Symbols"  },
  {  0x260E, 0x2612, "Noto Sans Symbols 2"  },
  {  0x2613, 0x2613, "Noto Sans Symbols"  },
  {  0x2614, 0x2623, "Noto Sans Symbols 2"  },
  {  0x2624, 0x262F, "Noto Sans Symbols"  },
  {  0x2630, 0x2637, "Noto Sans Symbols 2"  },
  {  0x2638, 0x263B, "Noto Sans Symbols"  },
  {  0x263C, 0x263C, "Noto Sans Symbols 2"  },
  {  0x263D, 0x2653, "Noto Sans Symbols"  },
  {  0x2654, 0x2668, "Noto Sans Symbols 2"  },
  {  0x2669, 0x267E, "Noto Sans Symbols"  },
  {  0x267F, 0x268F, "Noto Sans Symbols 2"  },
  {  0x2690, 0x269D, "Noto Sans Symbols"  },
  {  0x269E, 0x26A1, "Noto Sans Symbols 2"  },
  {  0x26A2, 0x26A9, "Noto Sans Symbols"  },
  {  0x26AA, 0x26AC, "Noto Sans Symbols 2"  },
  {  0x26AD, 0x26BC, "Noto Sans Symbols"  },
  {  0x26BD, 0x26CD, "Noto Sans Symbols 2"  },
  {  0x26CE, 0x26CE, "Noto Sans Symbols"  },
  {  0x26CF, 0x26E1, "Noto Sans Symbols 2"  },
  {  0x26E2, 0x26FF, "Noto Sans Symbols"  },
  {  0x2700, 0x2704, "Noto Sans Symbols 2"  },
    // 0x2705, 0x2705, ""
  {  0x2706, 0x2709, "Noto Sans Symbols 2"  },
    // 0x270A, 0x270A, ""
  {  0x270B, 0x271C, "Noto Sans Symbols 2"  },
  {  0x271D, 0x2721, "Noto Sans Symbols"  },
  {  0x2722, 0x2727, "Noto Sans Symbols 2"  },
    // 0x2728, 0x2728, ""
  {  0x2729, 0x274B, "Noto Sans Symbols 2"  },
    // 0x274C, 0x274C, ""
  {  0x274D, 0x274D, "Noto Sans Symbols 2"  },
    // 0x274E, 0x274E, ""
  {  0x274F, 0x2753, "Noto Sans Symbols 2"  },
    // 0x2754, 0x2755, ""
  {  0x2756, 0x2775, "Noto Sans Symbols 2"  },
  {  0x2776, 0x2793, "Noto Sans Symbols"  },
  {  0x2794, 0x2794, "Noto Sans Symbols 2"  },
    // 0x2795, 0x2797, ""
  {  0x2798, 0x27AF, "Noto Sans Symbols 2"  },
    // 0x27B0, 0x27B0, ""
  {  0x27B1, 0x27BE, "Noto Sans Symbols 2"  },
    // 0x27BF, 0x27BF, ""
  {  0x27C0, 0x27FF, "Noto Sans Math"  },
  {  0x2800, 0x28FF, "Noto Sans Symbols 2"  },
  {  0x2900, 0x2AFF, "Noto Sans Math"  },
  {  0x2B00, 0x2B0D, "Noto Sans Symbols 2"  },
  {  0x2B0E, 0x2B11, "Noto Sans Math"  },
  {  0x2B12, 0x2B2F, "Noto Sans Symbols 2"  },
  {  0x2B30, 0x2B4C, "Noto Sans Math"  },
  {  0x2B4D, 0x2B73, "Noto Sans Symbols 2"  },
  {  0x2B74, 0x2B75, "Noto Sans Math"  },
  {  0x2B76, 0x2B95, "Noto Sans Symbols 2"  },
  {  0x2B96, 0x2B96, "Noto Sans Math"  },
  {  0x2B97, 0x2BFD, "Noto Sans Symbols 2"  },
  {  0x2BFE, 0x2BFE, "Noto Sans Math"  },
  {  0x2BFF, 0x2BFF, "Noto Sans Symbols 2"  },
  {  0x2C00, 0x2C5F, "Noto Sans Glagolitic"  },
  {  0x2C80, 0x2CFF, "Noto Sans Coptic"  },
  {  0x2D00, 0x2D2F, "Noto Sans Georgian"  },
  {  0x2D30, 0x2D7F, "Noto Sans Tifinagh"  },
  {  0x2D80, 0x2DDF, "Noto Sans Ethiopic"  },
  {  0x2DE0, 0x2E7F, "Noto Sans"  },
  {  0x2E80, 0x2FDF, "Noto Sans CJK TC"  },
    //0x2FE0, 0x2FEF, ""  //unmapped
  {  0x2FF0, 0x303F, "Noto Sans CJK TC"  },
  {  0x3040, 0x30FF, "Noto Sans CJK JP"  },
  {  0x3100, 0x312F, "Noto Sans CJK SC"  },
  {  0x3130, 0x318F, "Noto Sans CJK KR"  },
  {  0x3190, 0x319F, "Noto Sans CJK JP"  },
  {  0x31A0, 0x31EF, "Noto Sans CJK SC"  },
    // 0x332c
  {  0x31F0, 0x4DBF, "Noto Sans CJK JP"  },
  {  0x4DC0, 0x4DFF, "Noto Sans Symbols 2"  },
  {  0x4E00, 0x9FFF, "Noto Sans CJK SC"  },
  {  0xA000, 0xA4CF, "Noto Sans Yi"  },
  {  0xA4D0, 0xA4FF, "Noto Sans Lisu"  },
  {  0xA500, 0xA63F, "Noto Sans Vai" },
  {  0xA640, 0xA69F, "Noto Sans" },
  {  0xA6A0, 0xA6FF, "Noto Sans Bamum" },
  {  0xA700, 0xA7FF, "Noto Sans" },
  {  0xA800, 0xA82F, "Noto Sans Syloti Nagri" },
  {  0xA830, 0xA83F, "Noto Sans Devanagari" },
  {  0xA840, 0xA87F, "Noto Sans Phags Pa" },
  {  0xA880, 0xA8DF, "Noto Sans Saurashtra" },
  {  0xA8E0, 0xA8FF, "Noto Sans Devanagari" },
  {  0xA900, 0xA92F, "Noto Sans Kayah Li" },
  {  0xA930, 0xA95F, "Noto Sans Rejang" },
  {  0xA960, 0xA97F, "Noto Sans CJK KR" },
  {  0xA980, 0xA9DF, "Noto Sans Javanese" },
  {  0xA9E0, 0xA9FF, "Noto Sans Myanmar" },
  {  0xAA00, 0xAA5F, "Noto Sans Cham" },
  {  0xAA60, 0xAA7F, "Noto Sans Myanmar" },
  {  0xAA80, 0xAADF, "Noto Sans Tai Viet" },
  {  0xAAE0, 0xAAFF, "Noto Sans Meetei Mayek" },
  {  0xAB00, 0xAB2F, "Noto Sans Ethiopic" },
  //ab70
  {  0xAB30, 0xAB6F, "Noto Sans" },
  {  0xAB70, 0xABBF, "Noto Sans Cherokee" },
  {  0xABC0, 0xABFF, "Noto Sans Meetei Mayek" },
  {  0xAC00, 0xD7FF, "Noto Sans CJK KR" },
    // 0xD800, 0xF8FF, ""  //unmapped
  {  0xF900, 0xFAFF, "Noto Sans CJK SC"  },
  {  0xFB00, 0xFB12, "Noto Sans"  },
  {  0xFB13, 0xFB1C, "Noto Sans Armenian"  },
  {  0xFB1D, 0xFB4F, "Noto Sans Hebrew"  },
  {  0xFB50, 0xFDFF, "Noto Sans Arabic"  },
  {  0xFE00, 0xFE0F, "DejaVu Sans"  },
  {  0xFE10, 0xFE1F, "Noto Sans CJK SC"  },
  {  0xFE20, 0xFE2F, "Noto Sans"  },
  {  0xFE30, 0xFE4F, "Noto Sans CJK SC"  },
  {  0xFE50, 0xFE6F, "Noto Sans CJK SC"  },
  {  0xFE70, 0xFEFF, "Noto Sans Arabic"  },
  {  0xFF00, 0xFFEF, "Noto Sans CJK SC"  },
  {  0xFFF0, 0xFFFF, "Noto Sans"  },
  {  0x10000, 0x1013F, "Noto Sans Linear B"  },
    //0x10060
  {  0x10140, 0x101FF, "Noto Sans Symbols 2"  },
    //0x10200, 0x1027F, ""  //unmapped
  {  0x10280, 0x1029F, "Noto Sans Lycian"  },
  {  0x102A0, 0x102DF, "Noto Sans Carian"  },
  {  0x102E0, 0x102FF, "Noto Sans Coptic"  },
  {  0x10300, 0x1032F, "Noto Sans Old Italic"  },
  {  0x10330, 0x1034F, "Noto Sans Gothic"  },
  {  0x10350, 0x1037F, "Noto Sans Old Permic"  },
  {  0x10380, 0x1039F, "Noto Sans Ugaritic"  },
  {  0x103A0, 0x103DF, "Noto Sans Old Persian"  },
    //0x103E0, 0x103FF, ""  //unmapped
  {  0x10400, 0x1044F, "Noto Sans Deseret"  },
  {  0x10450, 0x1047F, "Noto Sans Shavian"  },
  {  0x10480, 0x104AF, "Noto Sans Osmanya"  },
  {  0x104B0, 0x104FF, "Noto Sans Osage"  },
  {  0x10500, 0x1052F, "Noto Sans Elbasan"  },
  {  0x10530, 0x1056F, "Noto Sans Caucasian Albanian"  },
  {  0x10570, 0x105BF, "Noto Serif Vithkuqi" },
    //0x105C0, 0x105FF, ""  //unmapped
  {  0x10600, 0x1077F, "Noto Sans Linear A"  },
  {  0x10780, 0x107BF, "Noto Sans"  },
    //0x107C0, 0x107FF, ""  //unmapped
  {  0x10800, 0x1083F, "Noto Sans Cypriot"  },
  {  0x10840, 0x1085F, "Noto Sans Imperial Aramaic"  },
  {  0x10860, 0x1087F, "Noto Sans Palmyrene"  },
  {  0x10880, 0x108AF, "Noto Sans Nabataean"  },
    //0x108B0, 0x108DF, ""  //unmapped
  {  0x108E0, 0x108FF, "Noto Sans Hatran"  },
  {  0x10900, 0x1091F, "Noto Sans Phoenician"  },
  {  0x10920, 0x1093F, "Noto Sans Lydian"  },
    //0x10940, 0x1097F, ""  //unmapped
  {  0x10980, 0x109FF, "Noto Sans Meroitic"  },
  {  0x10A00, 0x10A5F, "Noto Sans Kharoshthi"  },
  {  0x10A60, 0x10A7F, "Noto Sans Old South Arabian"  },
  {  0x10A80, 0x10A9F, "Noto Sans Old North Arabian"  },
    //0x10AA0, 0x10ABF, ""  //unmapped
  {  0x10AC0, 0x10AFF, "Noto Sans Manichaean"  },
  {  0x10B00, 0x10B3F, "Noto Sans Avestan"  },
  {  0x10B40, 0x10B5F, "Noto Sans Inscriptional Parthian"  },
  {  0x10B60, 0x10B7F, "Noto Sans Inscriptional Pahlavi"  },
  {  0x10B80, 0x10BAF, "Noto Sans Psalter Pahlavi"  },
    //0x10BB0, 0x10BFF, ""  //unmapped
  {  0x10C00, 0x10C4F, "Noto Sans Old Turkic"  },
    //0x10C50, 0x10C7F, ""  //unmapped
  {  0x10C80, 0x10CFF, "Noto Sans Old Hungarian"  },
  {  0x10D00, 0x10D3F, "Noto Sans Hanifi Rohingya"  },
    //0x10D40, 0x10E5F, ""  //unmapped
  {  0x10E60, 0x10E7F, "Noto Sans Symbols 2"  },
  {  0x10E80, 0x10EBF, "Noto Serif Yezidi"  },
    //0x10EC0, 0x10EFF, ""  //unmapped
  {  0x10F00, 0x10F2F, "Noto Sans Old Sogdian"  },
  {  0x10F30, 0x10F6F, "Noto Sans Sogdian"  },
  {  0x10F70, 0x10FAF, "Noto Sans Old Uyghur"  },  //missing
  {  0x10FB0, 0x10FDF, "Noto Sans Chorasmian"  },  //missing
  {  0x10FE0, 0x10FFF, "Noto Sans Elymaic"  },
  {  0x11000, 0x1107F, "Noto Sans Brahmi"  },
  {  0x11080, 0x110CF, "Noto Sans Kaithi"  },
  {  0x110D0, 0x110FF, "Noto Sans Sora Sompeng"  },
  {  0x11100, 0x1114F, "Noto Sans Chakma"  },
  {  0x11150, 0x1117F, "Noto Sans Mahajani"  },
  {  0x11180, 0x111DF, "Noto Sans Sharada"  },
  {  0x111E0, 0x111FF, "Noto Sans Sinhala"  },
  {  0x11200, 0x1124F, "Noto Sans Khojki"  },
    //0x11250, 0x1127F, ""  //unmapped
  {  0x11280, 0x112AF, "Noto Sans Multani"  },
  {  0x112B0, 0x112FF, "Noto Sans Khudawadi"  },
  {  0x11300, 0x1137F, "Noto Sans Grantha"  },
    //0x11380, 0x113FF, ""  //unmapped
  {  0x11400, 0x1147F, "Noto Sans Newa"  },
  {  0x11480, 0x114DF, "Noto Sans Tirhuta"  },
    //0x114E0, 0x1157F, ""  //unmapped
  {  0x11580, 0x115FF, "Noto Sans Siddham"  },
  {  0x11600, 0x1165F, "Noto Sans Modi"  },
  {  0x11660, 0x1167F, "Noto Sans Mongolian"  },
  {  0x11680, 0x116CF, "Noto Sans Takri"  },
    //0x116D0, 0x116FF, ""  //unmapped
  {  0x11700, 0x1174F, "Noto Serif Ahom"  },
    //0x11750, 0x117FF, ""  //unmapped
  {  0x11800, 0x1184F, "Noto Serif Dogra"  },
    //0x11850, 0x1189F, ""  //unmapped
  {  0x118A0, 0x118FF, "Noto Sans Warang Citi"  },
  {  0x11900, 0x1195F, "Noto Sans Dives Akuru"  },  //missing
    //0x11960, 0x1199F, ""  //unmapped
  {  0x119A0, 0x119FF, "Noto Sans Nandinagari"  },  //missing
  {  0x11A00, 0x11A4F, "Noto Sans Zanabazar Square"  },
  {  0x11A50, 0x11AAF, "Noto Sans Soyombo"  },
  {  0x11AB0, 0x11ABF, "Noto Sans Canadian Aboriginal" },
  {  0x11AC0, 0x11AFF, "Noto Sans Pau Cin Hau"  },
    //0x11B00, 0x11BFF, ""  //unmapped
  {  0x11C00, 0x11C6F, "Noto Sans Bhaiksuki"  },
  {  0x11C70, 0x11CBF, "Noto Sans Marchen"  },
    //0x11CC0, 0x11CFF, ""  //unmapped
  {  0x11D00, 0x11D5F, "Noto Sans Masaram Gondi"  },
  {  0x11D60, 0x11DAF, "Noto Sans Gunjala Gondi"  },
    //0x11DB0, 0x11EDF, ""  //unmapped
  {  0x11EE0, 0x11EFF, "Noto Sans Makasar"  },  //missing
    //0x11F00, 0x11FAF, ""  //unmapped
  {  0x11FB0, 0x11FBF, "Noto Sans Lisu"  },
  {  0x11FC0, 0x11FFF, "Noto Sans Tamil Supplement"  },
  {  0x12000, 0x1254F, "Noto Sans Cuneiform"  },
    //0x12550, 0x12F8F, ""  //unmapped
  {  0x12F90, 0x12FFF, "Noto Sans Cypro Minoan"  },  //missing
  {  0x13000, 0x1342F, "Noto Sans Egyptian Hieroglyphs"  },
  {  0x13430, 0x1343F, "Noto Sans Cuneiform"  },
    //0x13440, 0x143FF, ""  //unmapped
  {  0x14400, 0x1467F, "Noto Sans Anatolian Hieroglyphs"  },
    //0x14680, 0x167FF, ""  //unmapped
  {  0x16800, 0x16A3F, "Noto Sans Bamum"  },
  {  0x16A40, 0x16A6F, "Noto Sans Mro"  },
  {  0x16A70, 0x16ACF, "Noto Sans Tangsa"  },  //missing
  {  0x16AD0, 0x16AFF, "Noto Sans Bassa Vah"  },
  {  0x16B00, 0x16B8F, "Noto Sans Pahawh Hmong"  },
    //0x16B90, 0x16E3F, ""  //unmapped
  {  0x16E40, 0x16E9F, "Noto Sans Medefaidrin"  },
    //0x16EA0, 0x16EFF, ""  //unmapped
  {  0x16F00, 0x16F9F, "Noto Sans Miao"  },
    //0x16FA0, 0x16FDF, ""  //unmapped
  {  0x16FE0, 0x18AFF, "Noto Serif Tangut"  },
  {  0x18B00, 0x18CFF, "Noto Sans Khitan"  },  //missing
  {  0x18D00, 0x18D7F, "Noto Serif Tangut"  },
    //0x18D80, 0x1AFEF, ""  //unmapped
    //{  0x1AFF0, 0x1AFFF, "Kana"  },  //missing
    //{  0x1B000, 0x1B0FF, "Kana"  },  //missing
    //{  0x1B100, 0x1B12F, "Kana"  },  //missing
    //{  0x1B130, 0x1B16F, "Kana"  },  //missing
  {  0x1B170, 0x1B2FF, "Noto Sans Nushu"  },
    //0x1B300, 0x1BBFF, ""  //unmapped
  {  0x1BC00, 0x1BC9F, "Noto Sans Duployan"  },
  {  0x1BCA0, 0x1BCAF, "Noto Sans Duployan"  },
    //0x1BCB0, 0x1CEFF, ""  //unmapped
    //{  0x1CF00, 0x1CFCF, "Znamenny Musical Notation"  },  //missing
    //0x1CFD0, 0x1CFFF, ""  //unmapped
  {  0x1D000, 0x1D24F, "Noto Music"  },
    //0x1D250, 0x1D2DF, ""  //unmapped
  {  0x1D2E0, 0x1D37F, "Noto Sans Symbols 2"  },
    //0x1D380, 0x1D3FF, ""  //unmapped
  {  0x1D400, 0x1D7FF, "Noto Sans Math"  },
  {  0x1D800, 0x1DAAF, "Noto Sans SignWriting"  },
    //0x1DAB0, 0x1DEFF, ""  //unmapped
  {  0x1DF00, 0x1DFFF, "Noto Sans"  },  //missing
  {  0x1E000, 0x1E02F, "Noto Sans Glagolitic"  },
    //0x1E030, 0x1E0FF, ""  //unmapped
  {  0x1E100, 0x1E14F, "Noto Serif Hmong Nyiakeng"  },
    //0x1E150, 0x1E28F, ""  //unmapped
  {  0x1E290, 0x1E2BF, "Noto Sans Toto"  },
  {  0x1E2C0, 0x1E2FF, "Noto Sans Wancho"  },
    //0x1E300, 0x1E7DF, ""  //unmapped
  {  0x1E7E0, 0x1E7FF, "Noto Sans Ethiopic"  },
  {  0x1E800, 0x1E8DF, "Noto Sans Mende Kikakui"  },
    //0x1E8E0, 0x1E8FF, ""  //unmapped
  {  0x1E900, 0x1E95F, "Noto Sans Adlam"  },
    //0x1E960, 0x1EC6F, ""  //unmapped
  {  0x1EC70, 0x1ECBF, "Noto Sans Indic Siyaq Numbers"  },
    //0x1ECC0, 0x1ECFF, ""  //unmapped
    //{  0x1ED00, 0x1ED4F, "Ottoman Siyaq Numbers"  },  //missing
    //0x1ED50, 0x1EDFF, ""  //unmapped
  {  0x1EE00, 0x1EEFF, "Noto Sans Math"  },
    //0x1EF00, 0x1EFFF, ""  //unmapped
  {  0x1F000, 0x1F0FF, "Noto Sans Symbols 2"  },
  {  0x1F100, 0x1F1DF, "Noto Sans CJK SC"  },
  {  0x1F1E0, 0x1F1FF, "Noto Emoji"  },
  {  0x1F200, 0x1F2FF, "Noto Sans CJK SC"  },
  {  0x1F300, 0x1F320, "Noto Emoji"  },
  {  0x1F321, 0x1F32F, "Noto Sans Symbols 2"  },
  {  0x1F330, 0x1F335, "Noto Emoji"  },
  {  0x1F336, 0x1F336, "Noto Sans Symbols 2"  },
  {  0x1F337, 0x1F37C, "Noto Emoji"  },
  {  0x1F37D, 0x1F37F, "Noto Sans Symbols 2"  },
  {  0x1F380, 0x1F393, "Noto Emoji"  },
  {  0x1F394, 0x1F39F, "Noto Sans Symbols 2"  },
  {  0x1F3A0, 0x1F3C4, "Noto Emoji"  },
  {  0x1F3C5, 0x1F3C5, "Noto Color Emoji"  },
  {  0x1F3C6, 0x1F3CA, "Noto Emoji"  },
  {  0x1F3CB, 0x1F3DF, "Noto Sans Symbols 2"  },
  {  0x1F3E0, 0x1F3F0, "Noto Emoji"  },
  {  0x1F3F1, 0x1F3FF, "Noto Sans Symbols 2"  },
  {  0x1F400, 0x1F429, "Noto Emoji"  },
  {  0x1F42A, 0x1F42A, "Noto Color Emoji"  },
  {  0x1F42B, 0x1F43E, "Noto Emoji"  },
  {  0x1F43F, 0x1F43F, "Noto Sans Symbols 2"  },
  {  0x1F440, 0x1F440, "Noto Emoji"  },
  {  0x1F441, 0x1F441, "Noto Sans Symbols 2"  },
  {  0x1F442, 0x1F4F7, "Noto Emoji"  },
  {  0x1F4F8, 0x1F4F8, "Noto Color Emoji"  },
  {  0x1F4F9, 0x1F4FC, "Noto Emoji"  },
  {  0x1F4FD, 0x1F4FF, "Noto Sans Symbols 2"  },
  {  0x1F500, 0x1F53D, "Noto Emoji"  },
  {  0x1F53E, 0x1F54F, "Noto Sans Symbols 2"  },
  {  0x1F550, 0x1F567, "Noto Emoji"  },
  {  0x1F568, 0x1F5FA, "Noto Sans Symbols 2"  },
  {  0x1F5FB, 0x1F640, "Noto Emoji"  },
  {  0x1F641, 0x1F644, "Noto Color Emoji"  },
  {  0x1F645, 0x1F64F, "Noto Emoji"  },
  {  0x1F650, 0x1F67F, "Noto Sans Symbols 2"  },
  {  0x1F680, 0x1F686, "Noto Emoji"  },
  {  0x1F687, 0x1F687, "Noto Sans Symbols 2"  },
  {  0x1F688, 0x1F68C, "Noto Emoji"  },
  {  0x1F68D, 0x1F68D, "Noto Sans Symbols 2"  },
  {  0x1F68E, 0x1F690, "Noto Emoji"  },
  {  0x1F691, 0x1F691, "Noto Sans Symbols 2"  },
  {  0x1F692, 0x1F693, "Noto Emoji"  },
  {  0x1F694, 0x1F694, "Noto Sans Symbols 2"  },
  {  0x1F695, 0x1F697, "Noto Emoji"  },
  {  0x1F698, 0x1F698, "Noto Sans Symbols 2"  },
  {  0x1F699, 0x1F6AC, "Noto Emoji"  },
  {  0x1F6AD, 0x1F6AD, "Noto Sans Symbols 2"  },
  {  0x1F6AE, 0x1F6B1, "Noto Emoji"  },
  {  0x1F6B2, 0x1F6B2, "Noto Sans Symbols 2"  },
  {  0x1F6B3, 0x1F6B8, "Noto Emoji"  },
  {  0x1F6B9, 0x1F6BA, "Noto Sans Symbols 2"  },
  {  0x1F6BB, 0x1F6BB, "Noto Emoji"  },
  {  0x1F6BC, 0x1F6BC, "Noto Sans Symbols 2"  },
  {  0x1F6BD, 0x1F6C5, "Noto Emoji"  },
  {  0x1F6C6, 0x1F6CB, "Noto Sans Symbols 2"  },
  {  0x1F6CC, 0x1F6CC, "Noto Color Emoji"  },
  {  0x1F6CD, 0x1F6CF, "Noto Sans Symbols 2"  },
  {  0x1F6D0, 0x1F6D2, "Noto Color Emoji"  },
  {  0x1F6D3, 0x1F6EA, "Noto Sans Symbols 2"  },
  {  0x1F6EB, 0x1F6EC, "Noto Color Emoji"  },
  {  0x1F6ED, 0x1F6F3, "Noto Sans Symbols 2"  },
  {  0x1F6F4, 0x1F6F6, "Noto Color Emoji"  },
  {  0x1F6F7, 0x1F6FF, "Noto Sans Symbols 2"  },
  {  0x1F700, 0x1F77F, "Noto Sans Symbols"  },
  {  0x1F780, 0x1F8FF, "Noto Sans Symbols 2"  },
  {  0x1F900, 0x1F9FF, "Noto Color Emoji"  },
  {  0x1FA00, 0x1FBFF, "Noto Sans Symbols 2"  },
    //0x1FC00, 0x1FFFF, ""
  // {  0x20000, 0x2A6DF, "Noto Sans CJK Unified Ideographs Extension B"  },
  // {  0x2A700, 0x2B73F, "Noto Sans CJK Unified Ideographs Extension C"  },
  // {  0x2B740, 0x2B81F, "Noto Sans CJK Unified Ideographs Extension D"  },
  // {  0x2B820, 0x2CEAF, "Noto Sans CJK Unified Ideographs Extension E"  },
  // {  0x2CEB0, 0x2EBEF, "Noto Sans CJK Unified Ideographs Extension F"  },
  // {  0x2F800, 0x2FA1F, "Noto Sans CJK Compatibility Ideographs Supplement"  },
  // {  0x30000, 0x3134F, "Noto Sans CJK Unified Ideographs Extension G"  }
};

static int
compare(const void *key0, const void *b0) {

  struct _coverage *b = (struct _coverage *)b0;
  unsigned key = *(unsigned *)key0;
  if (key < b->start) return -1;
  if (key > b->end) return 1;
  return 0;
}

static unsigned
lci_adjust_codepoint_rows(unsigned index_) {

  if (index_ < 0x2FE0)  return index_;
  index_ += 0x0010;
  if (index_ < 0xD800)  return index_;
  index_ += (0xF900 - 0xD800);
  if (index_ < 0x10200)  return index_;
  index_ += (0x10280 - 0x10200);
  if (index_ < 0x103E0)  return index_;
  index_ += (0x10400 - 0x103E0);
  if (index_ < 0x105C0)  return index_;
  index_ += (0x10600 - 0x105C0);
  if (index_ < 0x107C0)  return index_;
  index_ += (0x10800 - 0x107C0);
  if (index_ < 0x108B0)  return index_;
  index_ += (0x108E0 - 0x108B0);
  if (index_ < 0x10940)  return index_;
  index_ += (0x10980 - 0x10940);
  if (index_ < 0x10AA0)  return index_;
  index_ += (0x10AC0 - 0x10AA0);
  if (index_ < 0x10BB0)  return index_;
  index_ += (0x10C00 - 0x10BB0);
  if (index_ < 0x10C50)  return index_;
  index_ += (0x10C80 - 0x10C50);
  if (index_ < 0x10D40)  return index_;
  index_ += (0x10E60 - 0x10D40);
  if (index_ < 0x10EC0)  return index_;
  index_ += (0x10F00 - 0x10EC0);
  if (index_ < 0x11250)  return index_;
  index_ += (0x11280 - 0x11250);
  if (index_ < 0x11380)  return index_;
  index_ += (0x11400 - 0x11380);
  if (index_ < 0x114E0)  return index_;
  index_ += (0x11580 - 0x114E0);
  if (index_ < 0x116D0)  return index_;
  index_ += (0x11700 - 0x116D0);
  if (index_ < 0x11750)  return index_;
  index_ += (0x11800 - 0x11750);
  if (index_ < 0x11850)  return index_;
  index_ += (0x118A0 - 0x11850);
  if (index_ < 0x11960)  return index_;
  index_ += (0x119A0 - 0x11960);
  if (index_ < 0x11B00)  return index_;
  index_ += (0x11C00 - 0x11B00);
  if (index_ < 0x11CC0)  return index_;
  index_ += (0x11D00 - 0x11CC0);
  if (index_ < 0x11DB0)  return index_;
  index_ += (0x11EE0 - 0x11DB0);
  if (index_ < 0x11F00)  return index_;
  index_ += (0x11FB0 - 0x11F00);
  if (index_ < 0x12550)  return index_;
  index_ += (0x12F90 - 0x12550);
  if (index_ < 0x13440)  return index_;
  index_ += (0x14400 - 0x13440);
  if (index_ < 0x14680)  return index_;
  index_ += (0x16800 - 0x14680);
  if (index_ < 0x16B90)  return index_;
  index_ += (0x16E40 - 0x16B90);
  if (index_ < 0x16EA0)  return index_;
  index_ += (0x16F00 - 0x16EA0);
  if (index_ < 0x16FA0)  return index_;
  index_ += (0x16FE0 - 0x16FA0);
  if (index_ < 0x18D80)  return index_;
  index_ += (0x1AFF0 - 0x18D80);
  if (index_ < 0x1B300)  return index_;
  index_ += (0x1BC00 - 0x1B300);
  if (index_ < 0x1BCB0)  return index_;
  index_ += (0x1CF00 - 0x1BCB0);
  if (index_ < 0x1CFD0)  return index_;
  index_ += (0x1D000 - 0x1CFD0);
  if (index_ < 0x1D250)  return index_;
  index_ += (0x1D2E0 - 0x1D250);
  if (index_ < 0x1D380)  return index_;
  index_ += (0x1D400 - 0x1D380);
  if (index_ < 0x1DAB0)  return index_;
  index_ += (0x1DF00 - 0x1DAB0);
  if (index_ < 0x1E030)  return index_;
  index_ += (0x1E100 - 0x1E030);
  if (index_ < 0x1E150)  return index_;
  index_ += (0x1E290 - 0x1E150);
  if (index_ < 0x1E300)  return index_;
  index_ += (0x1E7E0 - 0x1E300);
  if (index_ < 0x1E8E0)  return index_;
  index_ += (0x1E900 - 0x1E8E0);
  if (index_ < 0x1E960)  return index_;
  index_ += (0x1EC70 - 0x1E960);
  if (index_ < 0x1ECC0)  return index_;
  index_ += (0x1ED00 - 0x1ECC0);
  if (index_ < 0x1ED50)  return index_;
  return (index_ + (0x1EE00 - 0x1ED50));
}

static void
lci_check_coverage(PangoCoverage *pc, unsigned index_, cairo_t *cr) {

  if (pango_coverage_get(pc, index_) == 0) {
    struct _coverage *found;
    found = bsearch(&index_, coverage_list, sizeof(coverage_list)/sizeof(struct _coverage),
                    sizeof(struct _coverage), compare);
    if (found == NULL)
      cairo_select_font_face(cr, "Noto Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    else
      cairo_select_font_face(cr, found->font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    return;
  }
  cairo_select_font_face(cr, "Noto Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
}

static gboolean
on_header_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
  (void)user_data;

    // retreive theme designed colors, selected is silly
  GtkStyleContext *context = gtk_widget_get_style_context(widget);
  GdkRGBA normal;
  gtk_style_context_get_color(context, GTK_STATE_FLAG_NORMAL, &normal);

    // set a font
  cairo_set_font_size(cr, (CELL_HEIGHT - FONT_PADDING - (CELL_BORDER * 2)));
  cairo_select_font_face(cr, FONT_NAME, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

  char row_id[8];
  long long *a = (long long *)row_id;
  *a = 0;
    // draw numbers on the top
  for (int idx = 0; idx < 16; idx++) {
    cairo_set_source_rgba(cr, normal.red, normal.green, normal.blue, normal.alpha);
    sprintf(row_id, "%X", idx);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, row_id, &extents);
    double x = ROW_ID_WIDTH + (CELL_WIDTH + (CELL_BORDER * 2)) * idx
             + (CELL_WIDTH + (CELL_BORDER * 2)) / 2 - extents.width / 2;
    double y = CELL_HEIGHT / 2 + extents.height / 2;
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, row_id);
  }
  return TRUE;
}

static gboolean
on_utf8_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
  (void)user_data;

  GtkStyleContext *context = gtk_widget_get_style_context(widget);
  GdkRGBA normal;
  gtk_style_context_get_color(context, GTK_STATE_FLAG_NORMAL, &normal);

    // set a font, approximately ascent + descent
  cairo_set_font_size(cr, (CELL_HEIGHT - FONT_PADDING - (CELL_BORDER * 2)));
  cairo_select_font_face(cr, FONT_NAME, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    //typedef struct { double ascent, descent, height, max_x_advance, max_y_advance; } cairo_font_extents_t;
  cairo_font_extents_t fextents;
  cairo_font_extents(cr, &fextents);
  double text_ascent = fextents.ascent;

    // white background over glyph display area
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    /* typedef struct { int x, y, width, height; } GdkRectangle; */
  GdkRectangle cr_rect;
    //no idea where x, y are relative to... width, height valid
  gtk_widget_get_allocation(widget, &cr_rect);
    //define cairo_rectangle(cairo_t *cr, double x, double y, double width, double height);
  cairo_rectangle(cr, ROW_ID_WIDTH, 0,
                      cr_rect.width - CELL_WIDTH,
                      cr_rect.height);
  cairo_fill(cr);

    // fill where there is a selected cell
  if (selected_cell != NULL) {
    //puts("has selected_cell");
    if ((selected_cell->y >= 0)
        && (selected_cell->y < ((CELL_HEIGHT + (CELL_BORDER * 2)) * 16))) {
      cairo_set_source_rgba(cr, .7, .7, .8, 1.0);
      cairo_rectangle(cr, ROW_ID_WIDTH + selected_cell->x + CELL_BORDER,
                          selected_cell->y + CELL_BORDER,
                          CELL_WIDTH,
                          CELL_HEIGHT);
      cairo_fill(cr);
    }
  }

  char row_id[8];
  long long *a = (long long *)row_id;
  *a = 0;

  double x, y;
  cairo_text_extents_t extents;
  cairo_set_source_rgba(cr, normal.red, normal.green, normal.blue, normal.alpha);

    // draw glyphs in cells
  PangoCoverage *pc = pango_coverage_new();
  for (int row = 0; row < 16; row++) {
    unsigned index_ = ((row + line_no) * 16);
    if (index_ >= 0x0870) {
      index_ = lci_adjust_codepoint_rows(index_);
    }
      // draw row number
    sprintf(row_id, "%06X", index_);
    cairo_select_font_face(cr, FONT_NAME, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_text_extents(cr, row_id, &extents);
    x = ROW_ID_WIDTH / 2 - extents.width / 2;
    y = (CELL_HEIGHT + (CELL_BORDER * 2)) * row
      + (CELL_HEIGHT + (CELL_BORDER * 2)) / 2 + extents.height / 2;
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, row_id);

    for (int column = 0; column < 16; column++) {
        // uses my butchered ctype inorder to map all unicode codepoints
      *row_id = 0;
      if (isprint((index_ + column))) {
        int bytes = g_unichar_to_utf8((index_ + column), row_id);
        row_id[bytes] = 0;
      }
      if ((index_ + column) >= 0x03E2)
        lci_check_coverage(pc, (index_ + column), cr);
      cairo_text_extents(cr, row_id, &extents);
      x = ROW_ID_WIDTH
        + ((CELL_WIDTH + (CELL_BORDER * 2)) * column)
        + ((CELL_WIDTH + (CELL_BORDER * 2)) / 2) - extents.width / 2;
      y = ((CELL_HEIGHT + (CELL_BORDER * 2)) * row) + text_ascent + CELL_BORDER;
      cairo_move_to(cr, x, y);
      cairo_show_text(cr, row_id);
    }
  }
  pango_coverage_unref(pc);
  return TRUE;
}

static void
on_copy_activate(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;
  (void)user_data;
  puts("on_copy_activate");

  char row_id[8];
  long long *a = (long long *)row_id;
  *a = 0;
  int row_height = CELL_HEIGHT + (CELL_BORDER * 2);
  unsigned index_ = (line_no * 16) + ((scell.y / row_height) * 16);
  if (index_ >= 0x0870) {
    index_ = lci_adjust_codepoint_rows(index_);
  }
  int uchar = index_ + (scell.x / row_height);
  g_unichar_to_utf8(uchar, row_id);
  gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), row_id, -1);
}

/* must include upper limit test when using scrolled window,
 * when scrolled at tail, event box will move, even though view can't
 * found problem, gtk setup of max, thier math was off by 2 on max.
 * probly used 8bit floats */
static gboolean
clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
  (void)user_data;

  int row_height = CELL_HEIGHT + (CELL_BORDER * 2);
  int box_x = (int)event->x - ROW_ID_WIDTH;
  int box_y = (int)event->y;


  if ((box_x >= 0) && (box_y >= 0)) {
    scell.x = (box_x / row_height) * row_height;
    scell.y = ((box_y / row_height) * row_height)
            - (line_no * row_height);
    scell.width = row_height;
    scell.height = row_height;
    selected_cell = &scell;
    gtk_widget_queue_draw(widget);
    if (event->button == 3) {
      puts("here");
      GtkWidget *menu = gtk_menu_new();
      GtkWidget *menu_item = gtk_menu_item_new_with_label("Copy");
      gtk_menu_attach_to_widget(GTK_MENU(menu), widget, NULL);
      gtk_menu_attach(GTK_MENU(menu), menu_item, 0, 1, 0, 1);
      gtk_widget_show_all(menu);
      gtk_menu_popup_at_pointer(GTK_MENU(menu), (const GdkEvent *)event);
      g_signal_connect(menu_item, "activate", G_CALLBACK(on_copy_activate), NULL);
    }
    return TRUE;
  }
  selected_cell = NULL;
  gtk_widget_queue_draw(widget);
  return FALSE;
}

static void
on_scroll_changed(GtkAdjustment *adjustment, gpointer user_data) {
  (void)user_data;

    // get the amount changed
  int row_height = CELL_HEIGHT + (CELL_BORDER * 2);
  double adj = gtk_adjustment_get_value(adjustment);
  if (((double)(line_no * row_height) > adj) && (adj > 0))
    adj += row_height;
  int new_line_no = (int)(adj / row_height);

    // adjusment can be smaller than a line when in scrollbar
    // allow it to sum to a line before updating
  if (new_line_no != line_no) {
      // set selected cell update
    if (selected_cell != NULL)
      selected_cell->y += (line_no - new_line_no) * row_height;
      // set view first line update
    line_no = new_line_no;
    gtk_adjustment_set_value(adjustment, (double)(line_no * row_height));
  }
}

int
main(int argc, char *argv[]) {

  gtk_init(&argc, &argv); // initialize Gtk

    // variable constants
  double sbar_width = 12;
  int window_width = ROW_ID_WIDTH + (16 * (CELL_WIDTH + (CELL_BORDER * 2))) + sbar_width;
  int window_height = 17 * (CELL_HEIGHT + (CELL_BORDER * 2));  // header + 16 cells
  int uni_rows = ((0x1FC00 - UNMAPPED_REGIONS) >> 4);  // current coverage

    // main viewport
  GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(main_window), window_width, window_height);
  gtk_window_set_resizable(GTK_WINDOW(main_window), FALSE);
  g_signal_connect(G_OBJECT(main_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

   // Attempting to add a widget with type GtkLayout to a GtkWindow, but as a GtkBin subclass a GtkWindow can only contain one widget at a time; it already contains a widget of type GtkDrawingArea
  GtkWidget *grid = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(main_window), grid);

    // header bar constant view
  GtkWidget *header = gtk_drawing_area_new();
  gtk_widget_set_size_request(header, window_width, CELL_HEIGHT);
  gtk_grid_attach(GTK_GRID(grid), header, 0, 0, window_width, CELL_HEIGHT);
  g_signal_connect(header, "draw", G_CALLBACK(on_header_draw_event), NULL);

    // utf8 scrollable view
  GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scrolled_window), window_width);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), window_height - CELL_HEIGHT);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_grid_attach(GTK_GRID(grid), scrolled_window,
                  0, CELL_HEIGHT, window_width, (16 * (CELL_WIDTH + (CELL_BORDER * 2))));
    // utf8 drawable attachment
  GtkWidget *utf8codes = gtk_layout_new(NULL, NULL);
  gtk_layout_set_size(GTK_LAYOUT(utf8codes), window_width, uni_rows * (CELL_HEIGHT + (CELL_BORDER * 2)));
  gtk_container_add(GTK_CONTAINER(scrolled_window), utf8codes);
  g_signal_connect(utf8codes, "draw", G_CALLBACK(on_utf8_draw_event), NULL);
  int events = gtk_widget_get_events(utf8codes);
  gtk_widget_add_events(utf8codes, events|GDK_BUTTON_PRESS_MASK);
  g_signal_connect(utf8codes, "button-press-event", G_CALLBACK(clicked), NULL);
  GtkAdjustment *vadj = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(utf8codes));
  g_signal_connect(vadj, "value-changed", G_CALLBACK(on_scroll_changed), NULL);

  gtk_widget_show_all(main_window);

// scollbar's value never reaches max, off by 2
// ignores it unless after show all
gtk_adjustment_set_upper(vadj, (uni_rows * (CELL_HEIGHT + (CELL_BORDER * 2))) + 2);

  gtk_main();

  return EXIT_SUCCESS;
}
