/**
 *  @file   FontGetter.cpp
 *  @brief  get font images
 *  @author Masashi KITAMURA
 *  @date   2017-09
 */
#include "FontGetter.hpp"
#include <stdio.h>
#include <string.h>
#include <map>
#include <windows.h>
#include <assert.h>
#include "TexChFontInfo.h"


void Font::getPara(TexChFontInfo& dst) const
{
	dst.x	= this->x;
	dst.y	= this->y;
	dst.w	= this->w;
	dst.h	= this->h;
	dst.ox	= this->ox;
	dst.oy	= this->oy;
}

void Font::getPara(TexChFontInfo0& dst) const
{
	dst.x	= this->x;
	dst.y	= this->y;
	dst.w	= this->w;
	dst.h	= this->h;
}


FontGetter::FontGetter(char const* ttfname
			, unsigned fontW,  unsigned fontH
			, unsigned cellW,  unsigned cellH
			, unsigned mul,    unsigned bpp
			, unsigned weight, bool     italic
			, unsigned resizeMode
) :   ttfname_(strdup(ttfname ? ttfname : ""))
    , fontW_(fontW)
    , fontH_(fontH)
    , cellW_(cellW)
    , cellH_(cellH)
    , mul_(mul)
    , bpp_(bpp)
    , tone_(1 << bpp)
    , weight_(weight)	// 0..9
    , italic_(italic)
    , resizeMode_(uint8_t(resizeMode))
{
    assert(1 <= bpp && bpp <= 8);
    if (mul_ == 0)
    	mul_ = 1;
    if (fontH_ == 0)
    	fontH_ = fontW;
    if (cellW_ < fontW_)
    	cellW_ = fontW_;
    if (cellH_ < fontH_)
    	cellH_ = fontH_;
}

FontGetter::~FontGetter() {
    free(ttfname_);
}


/** get fonts
 */
bool FontGetter::get(FontVec& rFonts) {
    LOGFONTW	logfont = {0};	    	    	    	    // �t�H���g�f�[�^
    logfont.lfHeight	    	= (fontW_ * mul_) * (-1);   // �t�H���g�̍���
    logfont.lfWidth 	    	= 0;	    	    	    // �t�H���g�̕��i���ρj
    logfont.lfEscapement    	= 0;	    	    	    // ������������̊p�x
    logfont.lfOrientation   	= 0;	    	    	    // �x�[�X���C���̊p�x
    logfont.lfWeight	    	= (weight_) ? weight_*100 : FW_DONTCARE;	// �t�H���g�̑���
    logfont.lfItalic	    	= (italic_) ? TRUE : FALSE; // �Α̂ɂ��邩�ǂ���
    logfont.lfUnderline     	= FALSE;    	    	    // ������t���邩�ǂ���
    logfont.lfStrikeOut     	= FALSE;    	    	    // ����������t���邩�ǂ���
    //logfont.lfCharSet     	= SHIFTJIS_CHARSET; 	    // �����Z�b�g�̎��ʎq
    logfont.lfCharSet	    	= DEFAULT_CHARSET;  	    // �����Z�b�g�̎��ʎq
    logfont.lfOutPrecision  	= OUT_DEFAULT_PRECIS;	    // �o�͐��x
    logfont.lfClipPrecision 	= CLIP_DEFAULT_PRECIS;	    // �N���b�s���O���x
    logfont.lfQuality	    	= ANTIALIASED_QUALITY;	    // �o�͕i��
    logfont.lfPitchAndFamily	= DEFAULT_PITCH;    	    // �s�b�`�ƃt�@�~��
    wchar_t wbuf[0x4000] = {0};
    //mbstowcs(wbuf, ttfname_, strlen(ttfname_));
    ::MultiByteToWideChar(CP_OEMCP,0,ttfname_, int(strlen(ttfname_))+1, wbuf, 0x4000);
    wcsncpy( logfont.lfFaceName, wbuf, 31 );	     // �t�H���g��
    logfont.lfFaceName[31] = 0;

    HFONT   new_hfont	= ::CreateFontIndirectW(&logfont);
    if (new_hfont == 0) {
    	fprintf(stderr, "ERROR: bad font data\n");
    	return false;
    }

    HDC     hdc     	= ::CreateCompatibleDC(NULL);
    HFONT   old_hfont	= (HFONT)::SelectObject( hdc, new_hfont );

    TEXTMETRIC	tm  	= {0};
    ::GetTextMetricsW( hdc, &tm );

    for (unsigned no = 0; no < rFonts.size(); ++no) {
    	rFonts[no].data.resize(cellW_ * cellH_);
		if (resizeMode_ <= 1)
    		getFontResizeMode1(hdc, tm, rFonts[no]);
    	else
    		getFontResizeMode2(hdc, tm, rFonts[no]);
    	adjustFontSize(rFonts[no]);
    }

    ::SelectObject( hdc, old_hfont );
    ::DeleteObject(new_hfont);
    ::DeleteObject(old_hfont);
    ::DeleteDC(hdc);

    return true;
}


/** get font data
 */
bool FontGetter::getFontResizeMode1(void* hdc0, struct tagTEXTMETRICW& tm, Font& font) {
    HDC     	hdc 	= (HDC)hdc0;
    UINT    	nChar	= font.ch;
    static MAT2 const mat2 = {
    	{ 0, 1, }, { 0, 0, },
    	{ 0, 0, }, { 0, 1, }
    };
    GLYPHMETRICS    gm = {0};
    DWORD size	= ::GetGlyphOutlineW(
    	hdc,	    	    // �f�o�C�X�R���e�L�X�g
    	nChar,	    	    // ���������������̐����l�i�P�����́j
    	GGO_GRAY8_BITMAP,   // �擾����f�[�^�̃t�H�[�}�b�g
    	&gm,	    	    // GLYPHMETRICS�\���̂ւ̃A�h���X
    	0,  	    	    // �擾����o�b�t�@�̃T�C�Y
    	NULL,	    	    // �擾����o�b�t�@�i�̈�쐬�ς݁j
    	&mat2 );    	    // �����ւ̍s��f�[�^
    if (!size)
    	return false;
    glyphOutlineBuf_.clear();
    glyphOutlineBuf_.resize(size);
    int rc = ::GetGlyphOutlineW( hdc, nChar, GGO_GRAY8_BITMAP
    				, &gm, size, (LPVOID)&glyphOutlineBuf_[0], &mat2 );
    if(rc <= 0) {
    	return false;
    }

    int pitch	    = (gm.gmBlackBoxX + 3) & ~3;

    int dw  	    = gm.gmBlackBoxX;
    int dh  	    = gm.gmBlackBoxY;
    int offset_x    = gm.gmptGlyphOrigin.x;
    int offset_y    = tm.tmAscent - gm.gmptGlyphOrigin.y;
    if (tm.tmInternalLeading != 0) {
    	offset_y    = offset_y - tm.tmInternalLeading; //tm.tmDescent;
    }
    int ox = 0;
    int oy = 0;

    //if (offset_y < 0) {
    //	offset_y     = 0;
    //}

    dw	     = (dw+(mul_-1)) / mul_;
    dh	     = (dh+(mul_-1)) / mul_;
    offset_x = (offset_x) / mul_;
    offset_y = (offset_y) / mul_;

	if (cellW_ > fontW_)
		offset_x += (cellW_ - fontW_) / 2;
	if (cellH_ > fontH_)
		offset_y += (cellH_ - fontH_) / 2;

    if (offset_x + dw > int(cellW_)) {
		ox       = offset_x + dw - cellW_;
    	offset_x = int(cellW_) - dw;
		if (offset_x < 0)
			offset_x = 0;
	} else if (offset_x + dw + offset_x < int(cellW_)) {
		int dif = cellW_ - (offset_x + dw + offset_x);
		offset_x += dif / 2;
	}
	if (offset_x < 0) {
		ox = offset_x;
    	offset_x = 0;
    }

    if (offset_y + dh > int(cellH_)) {
		oy       = offset_y + dh - cellH_;
    	offset_y = int(cellH_) - dh;
		if (offset_y < 0)
			offset_y = 0;
    } else if (offset_y < 0) {
		oy = offset_y;
    	offset_y = 0;
    }

	font.ox = ox;
	font.oy = oy;

	unsigned fontH = fontW_;
	unsigned fontW = fontW_;

    if (mul_ == 1) {
		if (fontW < pitch) {
			fontW = pitch;
			if (fontW > cellW_)
				fontW = cellW_;
		}
    	for ( unsigned j = 0 ; j < unsigned(dh) && j < cellH_ && j < fontH; ++j ) {
    	    for ( unsigned i = 0 ; i < unsigned(dw) && i < cellW_ && i < fontW; ++i ) {
    	    	unsigned alp  = glyphOutlineBuf_[j * pitch + i];
    	    	alp   = (alp * (tone_-1) ) / 64;
    	    	font.data[((j+offset_y) * cellW_) + (i + offset_x)]   = alp;
    	    }
    	}
    }else {
		if (fontW < pitch/mul_) {
			fontW = pitch/mul_;
			if (fontW > cellW_)
				fontW = cellW_;
		}
    	for ( unsigned j = 0 ; j < unsigned(dh) && j < cellH_ && j < fontH; ++j ) {
    	    for ( unsigned i = 0 ; i < unsigned(dw) && i < cellW_ && i < fontW; ++i ) {
    	    	unsigned total = 0;
    	    	for(unsigned y = 0 ; y < mul_ && y+(j*mul_) < gm.gmBlackBoxY ; ++y) {
    	    	    for(unsigned x = 0 ; x < mul_ && x+(i*mul_) < gm.gmBlackBoxX ; ++x) {
    	    	    	uint8_t alp = glyphOutlineBuf_[ (y + j * mul_) * pitch + (x + i * mul_) ];
    	    	    	total  += alp;
    	    	    }
    	    	}
    	    	font.data[(j + offset_y) * cellW_ +  (i + offset_x)]  = (total * (tone_-1)) / (mul_ * mul_ * 64);
    	    }
    	}
    }
    return true;
}

/** get font data
 */
bool FontGetter::getFontResizeMode2(void* hdc0, struct tagTEXTMETRICW& tm, Font& font) {
    HDC     	hdc 	= (HDC)hdc0;
    UINT    	nChar	= font.ch;
    static MAT2 const mat2 = {
    	{ 0, 1, }, { 0, 0, },
    	{ 0, 0, }, { 0, 1, }
    };

    GLYPHMETRICS    gm = {0};
    DWORD size	= ::GetGlyphOutlineW(
    	hdc,	    	    // �f�o�C�X�R���e�L�X�g
    	nChar,	    	    // ���������������̐����l�i�P�����́j
    	GGO_GRAY8_BITMAP,   // �擾����f�[�^�̃t�H�[�}�b�g
    	&gm,	    	    // GLYPHMETRICS�\���̂ւ̃A�h���X
    	0,  	    	    // �擾����o�b�t�@�̃T�C�Y
    	NULL,	    	    // �擾����o�b�t�@�i�̈�쐬�ς݁j
    	&mat2 );    	    // �����ւ̍s��f�[�^
    if (!size)
    	return false;

    glyphOutlineBuf_.clear();
    glyphOutlineBuf_.resize(size);
    int rc = ::GetGlyphOutlineW( hdc, nChar, GGO_GRAY8_BITMAP, &gm, size, (LPVOID)&glyphOutlineBuf_[0], &mat2 );
    if(rc <= 0) {
    	return false;
    }

	// GLAY8 �� 0..64 �� 65 �i�K. ����� 0..255 �ɒ���.
	for (unsigned i = 0; i < size; ++i) {
		unsigned c = glyphOutlineBuf_[i] * 255U / 64U;
		if (c > 255)	// �O�̈�.
			c = 255;
		glyphOutlineBuf_[i] = c;
	}
    unsigned srcWm  	= unsigned(gm.gmBlackBoxX);
    unsigned srcHm  	= unsigned(gm.gmBlackBoxY);
	unsigned cellWm	= unsigned(gm.gmCellIncX);
	if (cellWm < srcWm)
		cellWm = srcWm;
	unsigned cellHm	= unsigned(tm.tmHeight);
	if (cellHm < srcHm)
		cellHm = srcHm;

    int tgtXm	= gm.gmptGlyphOrigin.x;
    if (tgtXm < 0) tgtXm = 0;
	if (tgtXm + int(srcWm) > int(cellWm))
		tgtXm = cellWm - srcWm;

    int tgtYm	= tm.tmAscent - gm.gmptGlyphOrigin.y;
    if (tm.tmInternalLeading != 0) {
    	tgtYm   = tgtYm - tm.tmInternalLeading;
    }

    if (tgtYm < 0) tgtYm = 0;
	if (tgtYm + int(srcHm) > int(cellHm))
		tgtYm = cellHm - srcHm;

	unsigned fontW = fontW_;
	unsigned fontH = fontH_;
	unsigned cellW = cellW_;
	unsigned cellH = cellH_;

    int ox = 0;
    int oy = 0;

	unsigned m  = mul_;
	unsigned tgtW = (srcWm + m - 1) / m;
	unsigned tgtH = (srcHm + m - 1) / m;
	if (tgtW > fontW)
		tgtW = fontW;
	if (tgtH > fontH)
		tgtH = fontH;

    int      tgtX = tgtXm / m;
	int      tgtY = tgtYm / m;

	if (cellW > fontW)
		tgtX += (cellW - fontW) / 2;
	if (cellH > fontH)
		tgtH += (cellH - fontH) / 2;

    if (tgtX + int(tgtW) > int(cellW)-1) {
		ox   = tgtX + tgtW - cellW;
		tgtX = cellW - tgtW;
	} else if (tgtX + int(tgtW) + tgtX < int(cellW)) {
		int dif = cellW - (tgtX + tgtW + tgtX);
		tgtX += dif / 2;
	}
    if (tgtX < 0) {
		ox   = tgtX;
		tgtX = 0;
	}

    if (tgtY + int(tgtH) > int(cellH)-1) {
		oy   = tgtY + tgtH - cellH;
		tgtY = cellH - tgtH;
	}
    if (tgtY < 0) {
		oy   = tgtY;
    	tgtY = 0;
    }

	font.ox = ox;
	font.oy = oy;

	unsigned tone	= tone_;
	if (tone == 0)
		tone = 256;
 #if 1
	if (m == 1 && (cellWm <= fontW && cellHm <= fontH)) {
        unsigned pitch	= (tgtW + 3) & ~3;
		for ( unsigned j = 0; j < unsigned(srcHm); ++j ) {
		    for ( unsigned i = 0; i < unsigned(srcWm); ++i ) {
		    	unsigned a = glyphOutlineBuf_[j * pitch + i];
    	    	a   = (a * (tone-1)) / 255U;
				font.data[(tgtY + j) * cellW + (tgtX + i)] = a;
		    }
		}
	} else
 #endif
	{
	    uint8_t* dst	= &font.data[tgtY*cellW+tgtX];
	    unsigned pitch	= (srcWm + 3) & ~3;
		resizeBilinearReduc(dst, tgtW, tgtH, cellW, &glyphOutlineBuf_[0], srcWm, srcHm, pitch, tone);
	}

    return true;
}


/** calc font size
 */
bool FontGetter::adjustFontSize(Font& rFont) {
    unsigned x0 = cellW_;
    unsigned y0 = cellH_;
    unsigned x1 = 0;
    unsigned y1 = 0;
    unsigned w	= cellW_;
    unsigned h	= cellH_;
    for (unsigned y = 0; y < h; ++y) {
    	for (unsigned x = 0; x < w; ++x) {
    	    unsigned c = rFont.data[y * w + x];
    	    if (c) {
    	    	if (x0 > x) x0 = x;
    	    	if (y0 > y) y0 = y;
    	    	if (x1 < x) x1 = x;
    	    	if (y1 < y) y1 = y;
    	    }
    	}
    }
    if (x0 == cellW_ && y0 == cellH_ && x1 == 0 && y1 == 0) {
    	rFont.x = 0;
    	rFont.y = 0;
    	rFont.w = cellW_;
    	rFont.h = cellH_;
    	return	false;
    }
    w = 1+x1-x0;
    h = 1+y1-y0;
    rFont.x = x0;
    rFont.y = y0;
    rFont.w = w;
    rFont.h = h;
    return true;
}


#if 1
struct Str {
    enum { SIZE = 260 };
    Str() { memset(str_, 0, SIZE); }
    Str(char const* str) { strncpy(str_,str,SIZE); str_[SIZE-1] = 0; }
    Str(Str const& r) { memcpy(str_, r.str_, SIZE); }
    bool operator<(Str const& r) const { return strcmp(str_, r.str_) < 0; }
    char const* c_str() const { return str_; }
private:
    char    str_[SIZE];
};
typedef std::map<Str, unsigned> FontNames;
#else
#include <string>
typedef std::map<std::string, unsigned> FontNames;
#endif



static int CALLBACK enumFontFamExProc(
  ENUMLOGFONTEXW*   lpelfe, 	// �_���I�ȃt�H���g�f�[�^
  NEWTEXTMETRICEXW* lpntme, 	// �����I�ȃt�H���g�f�[�^
  unsigned/*DWORD*/ FontType,	// �t�H���g�̎��
  LPARAM    	    lParam  	// �A�v���P�[�V������`�̃f�[�^
){
    char buf[0x1000];
    WideCharToMultiByte(0,0,lpelfe->elfLogFont.lfFaceName, 32, buf, 0x1000, 0, 0);
    FontNames* pFontNames = (FontNames*)lParam;
    (*pFontNames)[buf] = 1;
    return 1;
}


/** print font list
 */
void FontGetter::printFontInfo() {
    LOGFONTW	logfont = {0};
    logfont.lfCharSet	= DEFAULT_CHARSET;
    HDC			hdc     = ::CreateCompatibleDC(NULL);

    FontNames	fntNames;
    EnumFontFamiliesExW(
      hdc,  	    	    	    	// �f�o�C�X�R���e�L�X�g�̃n���h��
      &logfont,     	    	    	// �t�H���g���
      (FONTENUMPROCW)enumFontFamExProc, // �R�[���o�b�N�֐�
      (LPARAM)&fntNames,    	    	// �ǉ��f�[�^
      0     	    	    	    	// ���g�p�G�K�� 0 ���w��
    );

    for (FontNames::iterator ite = fntNames.begin(); ite != fntNames.end(); ++ite) {
    	printf("%s\n", ite->first.c_str());
    }
}



// from https://github.com/tenk-a/bmptg/blob/master/src/Proc/pix32_resizeBilinear.c

#define USE_SUM_I64
#ifdef USE_SUM_I64
typedef int64_t         sum_t;
#define DBL_TO_SUM(x)   (sum_t)((x) * 4096.0)
#else
typedef double          sum_t;
#define DBL_TO_SUM(x)   (x)
#endif
#define CALC_WEI(d)     (((d) < 1.f) ? (((d) - 2.0)*(d)*(d) + 1.0) : (((-(d) + 5.0)*(d) - 8.0)*(d) + 4.0))  // a=-1

/** Bilinear �g��k��
 */
void  FontGetter::resizeBilinearReduc( uint8_t* dst, unsigned dstW, unsigned dstH, unsigned dstPitch
								,uint8_t const* src, unsigned srcW, unsigned srcH, unsigned srcPitch
								, unsigned tone)
{
    // �g�債������ �����{�ŏk�����邽�߂̐����{�������߂�.
    double rscaleX  = (double)srcW / dstW;
    double rscaleY  = (double)srcH / dstH;
    double mw       = (rscaleX <= 1.0) ? 1 : (int)rscaleX + 1;
    double mh       = (rscaleY <= 1.0) ? 1 : (int)rscaleY + 1;
    double mrscaleX = rscaleX / mw;
    double mrscaleY = rscaleY / mh;
    int	   scaleType= (mrscaleX == 1.0) * 2 + (mrscaleY == 1.0);
    double const R = 0.5;

    for (uint32_t dstY = 0; dstY < dstH; ++dstY) {
        for (uint32_t dstX = 0; dstX < dstW; ++dstX) {
            sum_t a = 0;
            sum_t wei_total = 0;
            for (double my = dstY*mh; my < (dstY+1)*mh; ++my) {
                for (double mx = dstX*mw; mx < (dstX+1)*mw; ++mx) {
                    double x0 = (mx + 0.5) * mrscaleX;
                    double y0 = (my + 0.5) * mrscaleY;
                    int    x1 = (int)(x0 - R);
                    int    x2 = (int)(x0 + R);
                    int    y1 = (int)(y0 - R);
                    int    y2 = (int)(y0 + R);
                    int    x, y;

                    if (x1 < 0)
                        x1 = 0;
                    if (x2 >= (int)srcW)
                        x2 = srcW - 1;
                    if (y1 < 0)
                        y1 = 0;
                    if (y2 >= (int)srcH)
                        y2 = srcH - 1;

                    if (scaleType == 0) {   // (rscaleX != 1.0 && rscaleY != 1.0)
                        for (y = y1; y <= y2; ++y) {
                            for (x = x1; x <= x2; ++x) {
                                uint32_t c    = src[y * srcPitch + x];
                                double   lenX = fabs((x + 0.5) - x0);
                                double   lenY = fabs((y + 0.5) - y0);
                                double   weiX = CALC_WEI(lenX);
                                double   weiY = CALC_WEI(lenY);
                                sum_t    wei  = DBL_TO_SUM( weiX * weiY );

                                wei_total += wei;
                                a += c * wei;
                            }
                        }
                    } else if (scaleType == 1) { // (rscaleX != 1.0 && rscaleY == 1.0)
                        y = (int)my; //y0;
                        for (x = x1; x <= x2; ++x) {
                            uint32_t c    = src[y * srcPitch + x];
                            double   lenX = fabs((x + 0.5) - x0);
                            sum_t    wei  = DBL_TO_SUM( CALC_WEI(lenX) );

                            wei_total += wei;
                            a += c * wei;
                        }
                    } else {    // (scaleType == 2) // (rscaleX == 1.0 && rscaleY != 1.0)
                        x = (int)mx; //x0;
                        for (y = y1; y <= y2; ++y) {
                            uint32_t c    = src[y * srcPitch + x];
                            double   lenY = fabs((y + 0.5) - y0);
                            sum_t    wei  = DBL_TO_SUM( CALC_WEI(lenY) );

                            wei_total += wei;
                            a += c * wei;
                        }
                    }
                }
            }

            a /= wei_total;
            uint32_t ia = uint32_t(a);
            if (ia > 255U)
            	ia = 255U;
			ia = ia * (tone-1) / 255U;
            dst[dstY*dstPitch + dstX] = uint8_t(ia);
        }
    }
}
