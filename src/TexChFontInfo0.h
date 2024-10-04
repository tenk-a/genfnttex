#ifndef TEXFONTINFO0_H_INCLUDED
#define TEXFONTINFO0_H_INCLUDED

#ifdef __cplusplus
#include <cassert>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TexChFontInfoHeader0 {
    unsigned	    id;
    unsigned	    chCount;
    unsigned short  texW;
    unsigned short  texH;
    unsigned short  fontW;
    unsigned short  fontH;
  #ifdef __cplusplus
    unsigned	    texChW() const { return texW / fontW; }
    unsigned	    texChH() const { return texH / fontH; }
    unsigned	    texPageChSize() const { return texChW() * texChH(); }
    unsigned	    texPageSize() const { unsigned n = texPageChSize(); return (chCount + n - 1) / n; }
    void    	    setFontBaseWH(unsigned w, unsigned h) { /*dummy*/ }
  #endif
} TexChFontInfoHeader0;

typedef struct TexChFontInfo0 {
    unsigned	    code;
    unsigned	    index;
    unsigned short  x;
    unsigned short  y;
    unsigned short  w;
    unsigned short  h;
} TexChFontInfo0;

#ifdef __cplusplus
} //extern "C"
#endif

#ifdef __cplusplus
class TexChFontInfoTable0 {
public:
    TexChFontInfoHeader0 const* header() const { return (TexChFontInfoHeader0 const*)this; }
    unsigned	    	    	infoSize() const { return header()->chCount; }
    TexChFontInfo0  	const*	infoTop() const { return (TexChFontInfo0 const*)((unsigned char const*)header() + sizeof(TexChFontInfoHeader0)); }
    TexChFontInfo0  	const&	infoIdx(unsigned idx) const { assert(idx < header()->chCount); return infoTop()[idx]; }

    TexChFontInfo0  	const*	searchCharCode(unsigned charCode) const {
    	unsigned high = infoSize();
    	if (high) {
    	    TexChFontInfo0 const* tbl = infoTop();
    	    unsigned	low = 0;
    	    while (low < high) {
    	    	unsigned mid = (low + high - 1) / 2;
    	    	TexChFontInfo0 const* midp = &tbl[mid];
    	    	unsigned midCode = midp->code;
    	    	if (charCode < midCode)
    	    	    high = mid;
    	    	else if (charCode > midCode)
    	    	    low  = mid + 1;
    	    	else
    	    	    return midp;
    	    }
    	}
    	return NULL;
    }

    bool charCodeToPageUVWH(unsigned charCode, unsigned& rPage, unsigned& rU, unsigned& rV, unsigned& rW, unsigned& rH) const {
    	TexChFontInfo0 const* fnd = searchCharCode(charCode);
    	if (!fnd)
    	    return false;
    	TexChFontInfoHeader0 const* hdr = header();
    	unsigned texChW     = hdr->texChW();
    	unsigned texChH     = hdr->texChH();
    	unsigned pageChSize = texChW * texChH;
    	unsigned idx	    = fnd->index;
    	unsigned page	    = idx / pageChSize;
    	unsigned ofs	    = idx % pageChSize;
    	unsigned cx 	    = ofs % texChW;
    	unsigned cy 	    = ofs / texChW;

    	rPage = page;
    	rU    = cx * hdr->fontW + fnd->x;
    	//rV  = cy * hdr->fontH + fnd->y;
    	rV    = cy * hdr->fontH;
    	rW    = fnd->w;
    	//rH  = fnd->h;
    	rH    = hdr->fontH;
    	return true;
    }
};
#endif

#endif
