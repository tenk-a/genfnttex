	"#ifndef TEXFONTINFO_H_INCLUDED\n"
	"#define TEXFONTINFO_H_INCLUDED\n"
	"\n"
	"#ifdef __cplusplus\n"
	"#include <cassert>\n"
	"#endif\n"
	"\n"
	"#ifdef __cplusplus\n"
	"extern \"C\" {\n"
	"#endif\n"
	"\n"
	"typedef struct TexChFontInfoHeader {\n"
	"    unsigned\t    id;\n"
	"    unsigned\t    chCount;\n"
	"    unsigned short  texW;\n"
	"    unsigned short  texH;\n"
	"    unsigned short  fontW;\n"
	"    unsigned short  fontH;\n"
	"  #ifdef __cplusplus\n"
	"    unsigned\t    texChW() const { return texW / fontW; }\n"
	"    unsigned\t    texChH() const { return texH / fontH; }\n"
	"    unsigned\t    texPageChSize() const { return texChW() * texChH(); }\n"
	"    unsigned\t    texPageSize() const { unsigned n = texPageChSize(); return (chCount + n - 1) / n; }\n"
	"  #endif\n"
	"} TexChFontInfoHeader;\n"
	"\n"
	"typedef struct TexChFontInfo {\n"
	"    unsigned\t    code;\n"
	"    unsigned\t    index;\n"
	"    unsigned short  x;\n"
	"    unsigned short  y;\n"
	"    unsigned short  w;\n"
	"    unsigned short  h;\n"
	"} TexChFontInfo;\n"
	"\n"
	"#ifdef __cplusplus\n"
	"} //extern \"C\"\n"
	"#endif\n"
	"\n"
	"#ifdef __cplusplus\n"
	"class TexChFontInfoTable {\n"
	"public:\n"
	"    TexChFontInfoHeader const*\theader() const { return (TexChFontInfoHeader const*)this; }\n"
	"    unsigned\t    \t    \tinfoSize() const { return header()->chCount; }\n"
	"    TexChFontInfo   \tconst*\tinfoTop() const { return (TexChFontInfo const*)((unsigned char const*)header() + sizeof(TexChFontInfoHeader)); }\n"
	"    TexChFontInfo   \tconst&\tinfoIdx(unsigned idx) const { assert(idx < header()->chCount); return infoTop()[idx]; }\n"
	"\n"
	"    TexChFontInfo   \tconst*\tsearchCharCode(unsigned charCode) const {\n"
	"    \tunsigned high = infoSize();\n"
	"    \tif (high) {\n"
	"    \t    TexChFontInfo const* tbl = infoTop();\n"
	"    \t    unsigned\tlow = 0;\n"
	"    \t    while (low < high) {\n"
	"    \t    \tunsigned mid = (low + high - 1) / 2;\n"
	"    \t    \tTexChFontInfo const* midp = &tbl[mid];\n"
	"    \t    \tunsigned midCode = midp->code;\n"
	"    \t    \tif (charCode < midCode)\n"
	"    \t    \t    high = mid;\n"
	"    \t    \telse if (charCode > midCode)\n"
	"    \t    \t    low  = mid + 1;\n"
	"    \t    \telse\n"
	"    \t    \t    return midp;\n"
	"    \t    }\n"
	"    \t}\n"
	"    \treturn NULL;\n"
	"    }\n"
	"\n"
	"    bool charCodeToPageUVWH(unsigned charCode, unsigned& rPage, unsigned& rU, unsigned& rV, unsigned& rW, unsigned& rH) const {\n"
	"    \tTexChFontInfo const* fnd = searchCharCode(charCode);\n"
	"    \tif (!fnd)\n"
	"    \t    return false;\n"
	"    \tTexChFontInfoHeader const* hdr = header();\n"
	"    \tunsigned texChW     = hdr->texChW();\n"
	"    \tunsigned texChH     = hdr->texChH();\n"
	"    \tunsigned pageChSize = texChW * texChH;\n"
	"    \tunsigned idx\t    = fnd->index;\n"
	"    \tunsigned page\t    = idx / pageChSize;\n"
	"    \tunsigned ofs\t    = idx % pageChSize;\n"
	"    \tunsigned cx \t    = ofs % texChW;\n"
	"    \tunsigned cy \t    = ofs / texChW;\n"
	"\n"
	"    \trPage = page;\n"
	"    \trU    = cx * hdr->fontW + fnd->x;\n"
	"    \t//rV  = cy * hdr->fontH + fnd->y;\n"
	"    \trV    = cy * hdr->fontH;\n"
	"    \trW    = fnd->w;\n"
	"    \t//rH  = fnd->h;\n"
	"    \trH    = hdr->fontH;\n"
	"    \treturn true;\n"
	"    }\n"
	"};\n"
	"#endif\n"
	"\n"
	"#endif\n"