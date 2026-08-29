#include "options_dialog.h"
#include "resource.h"
#include "mouse_wrap.h"
#include "darkmode.h"
#include "multimonitor_edges.h"
#include "multimonitor_contour.h"
#include <windowsx.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Theme palette (ARGB for GDI+, converted to COLORREF where GDI needs it)
// ---------------------------------------------------------------------------

typedef struct {
    ARGB bg;          // dialog background
    ARGB card;        // preview card fill
    ARGB cardBorder;  // preview card 1px border
    ARGB text;        // primary text
    ARGB subtle;      // secondary text
    ARGB monTop;      // monitor gradient top
    ARGB monBottom;   // monitor gradient bottom
    ARGB monBorder;   // monitor outline
    ARGB edgeWrap;    // green
    ARGB edgeDelayed; // yellow
    ARGB edgeNoWrap;  // red
    ARGB cursorDot;
    ARGB cursorRing;
    ARGB track;       // slider channel (unfilled)
    ARGB thumbRing;   // slider thumb outer ring
} Theme;

static const Theme THEME_LIGHT = {
    0xFFF3F3F3, 0xFFFFFFFF, 0xFFE0E0E0, 0xFF1B1B1B, 0xFF6B6B6B,
    0xFF5A8FD6, 0xFF3C6DB4, 0xFF2E5A9C,
    0xFF1DA84A, 0xFFE0B000, 0xFFD62828,
    0xFFFF3C3C, 0xFFFFFFFF,
    0xFF8A8A8A, 0xFFFFFFFF
};

static const Theme THEME_DARK = {
    0xFF202020, 0xFF2B2B2B, 0xFF3A3A3A, 0xFFFFFFFF, 0xFFA0A0A0,
    0xFF4B7FC4, 0xFF2F5A94, 0xFF1F4070,
    0xFF2ECC71, 0xFFF0C830, 0xFFE05050,
    0xFFFFFFFF, 0xFF202020,
    0xFF9A9A9A, 0xFF454545
};

static COLORREF ToColorRef(ARGB c) {
    return RGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
}

static ARGB WithAlpha(ARGB c, BYTE a) {
    return (c & 0x00FFFFFF) | ((ARGB)a << 24);
}

#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef DWMSBT_MAINWINDOW
#define DWMSBT_MAINWINDOW 2
#endif

// ---------------------------------------------------------------------------
// Dialog state
// ---------------------------------------------------------------------------

#define IDT_CURSOR_TRACK  50   // timer ID for live cursor dot + hover tracking
#define CURSOR_TRACK_MS   50   // refresh interval (ms)

#define MAX_PREVIEW_MONITORS 16
#define MAX_CONTOUR_EDGES    128

typedef struct {
    me_Rect    rects[MAX_PREVIEW_MONITORS];
    SIZE_T     count;
    me_Edge    contourEdges[MAX_CONTOUR_EDGES];
    EdgeState  edgeState[MAX_CONTOUR_EDGES];
    SIZE_T     contourCount;

    BOOL       dark;
    const Theme* theme;
    ARGB       accent;          // system accent colour (slider)
    HBRUSH     hBrushBg;
    HWND       hTip;
    HFONT      hFontUi;         // system message font at current DPI
    HFONT      hFontLabel;      // slightly larger semibold, monitor numbers
    UINT       dpi;
    int        hoverIdx;        // contour edge under the cursor, or -1

    // Cached preview transform for hit testing (set during draw)
    double     scale;
    int        offX, offY;
    int        bbLeft, bbTop;
} OptionsDlgData;

static int Px(const OptionsDlgData* d, int at96) { return MulDiv(at96, (int)d->dpi, 96); }

// ---------------------------------------------------------------------------
// Fonts / theme helpers
// ---------------------------------------------------------------------------

static HFONT CreateUiFont(UINT dpi, int weight, int percent)
{
    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(ncm);
    LOGFONTW lf;
    if (SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi)) {
        lf = ncm.lfMessageFont;
    } else {
        memset(&lf, 0, sizeof(lf));
        lstrcpyW(lf.lfFaceName, L"Segoe UI");
        lf.lfHeight = -MulDiv(9, (int)dpi, 72);
    }
    lf.lfWeight = weight;
    lf.lfHeight = MulDiv(lf.lfHeight, percent, 100);
    lf.lfQuality = CLEARTYPE_QUALITY;
    return CreateFontIndirectW(&lf);
}

static ARGB QueryAccentColor(void)
{
    DWORD argb = 0; BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&argb, &opaque)) && (argb & 0x00FFFFFF) != 0)
        return 0xFF000000 | (argb & 0x00FFFFFF);
    return 0xFF0078D4; // Windows default blue
}

static BOOL CALLBACK SetFontProc(HWND hChild, LPARAM lParam)
{
    SendMessageW(hChild, WM_SETFONT, (WPARAM)lParam, TRUE);
    return TRUE;
}

static BOOL CALLBACK ThemeChildProc(HWND hChild, LPARAM lParam)
{
    DarkMode_ApplyToControl(hChild, (BOOL)lParam);
    return TRUE;
}

static void RebuildFonts(HWND hDlg, OptionsDlgData* data)
{
    if (data->hFontUi)    DeleteObject(data->hFontUi);
    if (data->hFontLabel) DeleteObject(data->hFontLabel);
    data->hFontUi    = CreateUiFont(data->dpi, FW_NORMAL, 100);
    data->hFontLabel = CreateUiFont(data->dpi, FW_SEMIBOLD, 115);
    EnumChildWindows(hDlg, SetFontProc, (LPARAM)data->hFontUi);

    // Owner-drawn combobox: item heights must be set by hand, from the font.
    HDC hdc = GetDC(hDlg);
    HFONT hOld = SelectObject(hdc, data->hFontUi);
    TEXTMETRICW tm;
    GetTextMetricsW(hdc, &tm);
    SelectObject(hdc, hOld);
    ReleaseDC(hDlg, hdc);
    HWND hCombo = GetDlgItem(hDlg, IDC_DRAG_MODE_COMBO);
    SendMessageW(hCombo, CB_SETITEMHEIGHT, (WPARAM)-1, tm.tmHeight + Px(data, 10)); // selection field
    SendMessageW(hCombo, CB_SETITEMHEIGHT, 0,          tm.tmHeight + Px(data, 8));  // list items
}

static void ApplyTheme(HWND hDlg, OptionsDlgData* data)
{
    data->dark   = DarkMode_IsEnabled();
    data->theme  = data->dark ? &THEME_DARK : &THEME_LIGHT;
    data->accent = QueryAccentColor();

    if (data->hBrushBg) DeleteObject(data->hBrushBg);
    data->hBrushBg = CreateSolidBrush(ToColorRef(data->theme->bg));

    DarkMode_ApplyToWindow(hDlg, data->dark);
    EnumChildWindows(hDlg, ThemeChildProc, (LPARAM)data->dark);
    if (data->hTip) DarkMode_ApplyToControl(data->hTip, data->dark);

    RedrawWindow(hDlg, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
}

// ---------------------------------------------------------------------------
// Preview geometry (shared by drawing and hit testing)
// ---------------------------------------------------------------------------

static BOOL ComputeTransform(OptionsDlgData* data, const RECT* rc)
{
    if (data->count == 0) return FALSE;

    int bbLeft = data->rects[0].xMin, bbTop = data->rects[0].yMin;
    int bbRight = data->rects[0].xMax, bbBottom = data->rects[0].yMax;
    for (SIZE_T i = 1; i < data->count; i++) {
        if (data->rects[i].xMin < bbLeft)   bbLeft   = data->rects[i].xMin;
        if (data->rects[i].yMin < bbTop)    bbTop    = data->rects[i].yMin;
        if (data->rects[i].xMax > bbRight)  bbRight  = data->rects[i].xMax;
        if (data->rects[i].yMax > bbBottom) bbBottom = data->rects[i].yMax;
    }
    int bbW = bbRight - bbLeft, bbH = bbBottom - bbTop;
    if (bbW <= 0 || bbH <= 0) return FALSE;

    int padding = Px(data, 16);
    int ctrlW = (rc->right - rc->left) - padding * 2;
    int ctrlH = (rc->bottom - rc->top) - padding * 2;
    if (ctrlW <= 0 || ctrlH <= 0) return FALSE;

    double scaleX = (double)ctrlW / bbW, scaleY = (double)ctrlH / bbH;
    double scale = (scaleX < scaleY) ? scaleX : scaleY;
    int scaledW = (int)(bbW * scale), scaledH = (int)(bbH * scale);

    data->scale  = scale;
    data->offX   = rc->left + padding + (ctrlW - scaledW) / 2;
    data->offY   = rc->top  + padding + (ctrlH - scaledH) / 2;
    data->bbLeft = bbLeft;
    data->bbTop  = bbTop;
    return TRUE;
}

static int MapX(const OptionsDlgData* d, int x) { return d->offX + (int)((x - d->bbLeft) * d->scale); }
static int MapY(const OptionsDlgData* d, int y) { return d->offY + (int)((y - d->bbTop)  * d->scale); }

// Distance in pixels from pt (preview client coords) to contour edge idx,
// or INT_MAX when pt is not beside the edge's span.
static int EdgeDistancePx(const OptionsDlgData* data, SIZE_T idx, POINT pt)
{
    me_Edge e = data->contourEdges[idx];
    int ex1 = MapX(data, e.x1), ey1 = MapY(data, e.y1);
    int ex2 = MapX(data, e.x2), ey2 = MapY(data, e.y2);
    int slack = Px(data, 3);
    if (ex1 == ex2) {
        int lo = min(ey1, ey2) - slack, hi = max(ey1, ey2) + slack;
        return (pt.y >= lo && pt.y <= hi) ? abs(pt.x - ex1) : INT_MAX;
    } else {
        int lo = min(ex1, ex2) - slack, hi = max(ex1, ex2) + slack;
        return (pt.x >= lo && pt.x <= hi) ? abs(pt.y - ey1) : INT_MAX;
    }
}

// Index of the contour edge within the hit threshold of pt, or -1.
static int HitTestEdge(const OptionsDlgData* data, POINT pt)
{
    if (data->contourCount == 0 || data->scale <= 0.0) return -1;
    int bestIdx = -1, bestDist = Px(data, 7);
    for (SIZE_T i = 0; i < data->contourCount; i++) {
        int d = EdgeDistancePx(data, i, pt);
        if (d < bestDist) { bestDist = d; bestIdx = (int)i; }
    }
    return bestIdx;
}

static ARGB EdgeColor(const Theme* t, EdgeState s)
{
    return s == EDGE_DELAYED ? t->edgeDelayed : s == EDGE_NOWRAP ? t->edgeNoWrap : t->edgeWrap;
}

// ---------------------------------------------------------------------------
// GDI+ drawing helpers
// ---------------------------------------------------------------------------

static GpPath* RoundRectPath(REAL x, REAL y, REAL w, REAL h, REAL r)
{
    GpPath* path = NULL;
    if (GdipCreatePath(FillModeAlternate, &path) != Ok) return NULL;
    REAL d = r * 2;
    if (d > w) d = w;
    if (d > h) d = h;
    GdipAddPathArc(path, x,         y,         d, d, 180, 90);
    GdipAddPathArc(path, x + w - d, y,         d, d, 270, 90);
    GdipAddPathArc(path, x + w - d, y + h - d, d, d,   0, 90);
    GdipAddPathArc(path, x,         y + h - d, d, d,  90, 90);
    GdipClosePathFigure(path);
    return path;
}

static void DrawRoundedLine(GpGraphics* g, ARGB color, REAL width, int x1, int y1, int x2, int y2)
{
    GpPen* pen = NULL;
    if (GdipCreatePen1(color, width, UnitPixel, &pen) != Ok) return;
    GdipSetPenStartCap(pen, LineCapRound);
    GdipSetPenEndCap(pen, LineCapRound);
    GdipDrawLine(g, pen, (REAL)x1, (REAL)y1, (REAL)x2, (REAL)y2);
    GdipDeletePen(pen);
}

static GpGraphics* BeginGdipDraw(HDC hdc)
{
    GpGraphics* g = NULL;
    if (GdipCreateFromHDC(hdc, &g) != Ok) return NULL;
    GdipSetSmoothingMode(g, SmoothingModeAntiAlias);
    GdipSetPixelOffsetMode(g, PixelOffsetModeHalf);
    GdipSetTextRenderingHint(g, TextRenderingHintAntiAliasGridFit);
    return g;
}

// ---------------------------------------------------------------------------
// Monitor preview
// ---------------------------------------------------------------------------

static void DrawMonitorPreview(OptionsDlgData* data, const DRAWITEMSTRUCT* dis)
{
    const Theme* t = data->theme;
    RECT rc = dis->rcItem;
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return;

    // Double buffer: everything is drawn into a memory bitmap and blitted once.
    HDC hdcMem = CreateCompatibleDC(dis->hDC);
    HBITMAP hBmp = CreateCompatibleBitmap(dis->hDC, w, h);
    HBITMAP hOldBmp = SelectObject(hdcMem, hBmp);
    RECT local = { 0, 0, w, h };

    HBRUSH hBg = CreateSolidBrush(ToColorRef(t->bg));
    FillRect(hdcMem, &local, hBg);
    DeleteObject(hBg);

    GpGraphics* g = BeginGdipDraw(hdcMem);
    if (g) {
        // Card
        REAL radius = (REAL)Px(data, 8);
        GpPath* card = RoundRectPath(0.5f, 0.5f, (REAL)w - 1, (REAL)h - 1, radius);
        if (card) {
            GpSolidFill* fill = NULL; GpPen* border = NULL;
            GdipCreateSolidFill(t->card, &fill);
            GdipCreatePen1(t->cardBorder, 1.0f, UnitPixel, &border);
            GdipFillPath(g, (GpBrush*)fill, card);
            GdipDrawPath(g, border, card);
            GdipDeleteBrush((GpBrush*)fill);
            GdipDeletePen(border);
            GdipDeletePath(card);
        }

        if (ComputeTransform(data, &local)) {
            // Monitors
            GpFont* font = NULL;
            GpStringFormat* fmt = NULL;
            GpSolidFill* textBrush = NULL;
            {
                LOGFONTW lf;
                GetObjectW(data->hFontLabel, sizeof(lf), &lf);
                GdipCreateFontFromLogfontW(hdcMem, &lf, &font);
                GdipCreateStringFormat(0, LANG_NEUTRAL, &fmt);
                GdipSetStringFormatAlign(fmt, StringAlignmentCenter);
                GdipSetStringFormatLineAlign(fmt, StringAlignmentCenter);
                GdipCreateSolidFill(0xFFFFFFFF, &textBrush);
            }
            int gap = Px(data, 2);
            REAL monRadius = (REAL)Px(data, 6);
            GpPen* monPen = NULL;
            GdipCreatePen1(t->monBorder, (REAL)Px(data, 2), UnitPixel, &monPen);

            for (SIZE_T i = 0; i < data->count; i++) {
                int left   = MapX(data, data->rects[i].xMin) + gap;
                int top    = MapY(data, data->rects[i].yMin) + gap;
                int right  = MapX(data, data->rects[i].xMax) - gap;
                int bottom = MapY(data, data->rects[i].yMax) - gap;
                GpRectF r = { (REAL)left, (REAL)top, (REAL)(right - left), (REAL)(bottom - top) };
                if (r.Width <= 0 || r.Height <= 0) continue;

                GpPath* mon = RoundRectPath(r.X, r.Y, r.Width, r.Height, monRadius);
                GpLineGradient* grad = NULL;
                GdipCreateLineBrushFromRect(&r, t->monTop, t->monBottom, LinearGradientModeVertical, WrapModeTileFlipXY, &grad);
                if (mon) {
                    if (grad) GdipFillPath(g, (GpBrush*)grad, mon);
                    if (monPen) GdipDrawPath(g, monPen, mon);
                    GdipDeletePath(mon);
                }
                if (grad) GdipDeleteBrush((GpBrush*)grad);

                WCHAR label[4];
                wsprintfW(label, L"%d", (int)(i + 1));
                if (font && fmt && textBrush)
                    GdipDrawString(g, label, -1, font, &r, fmt, (GpBrush*)textBrush);
            }
            if (monPen) GdipDeletePen(monPen);
            if (textBrush) GdipDeleteBrush((GpBrush*)textBrush);
            if (fmt) GdipDeleteStringFormat(fmt);
            if (font) GdipDeleteFont(font);

            // Contour edges (hovered edge gets a soft glow underneath)
            REAL edgeW = (REAL)Px(data, 3);
            for (SIZE_T i = 0; i < data->contourCount; i++) {
                me_Edge e = data->contourEdges[i];
                ARGB c = EdgeColor(t, data->edgeState[i]);
                int x1 = MapX(data, e.x1), y1 = MapY(data, e.y1);
                int x2 = MapX(data, e.x2), y2 = MapY(data, e.y2);
                if ((int)i == data->hoverIdx)
                    DrawRoundedLine(g, WithAlpha(c, 0x60), edgeW * 3.0f, x1, y1, x2, y2);
                DrawRoundedLine(g, c, edgeW, x1, y1, x2, y2);
            }

            // Live cursor dot with a contrasting ring
            POINT cur;
            GetCursorPos(&cur);
            REAL r = (REAL)Px(data, 4);
            REAL cx = (REAL)MapX(data, cur.x), cy = (REAL)MapY(data, cur.y);
            GpSolidFill* dot = NULL; GpPen* ring = NULL;
            GdipCreateSolidFill(t->cursorDot, &dot);
            GdipCreatePen1(t->cursorRing, 1.0f, UnitPixel, &ring);
            if (dot)  GdipFillEllipse(g, (GpBrush*)dot, cx - r, cy - r, r * 2, r * 2);
            if (ring) GdipDrawEllipse(g, ring, cx - r, cy - r, r * 2, r * 2);
            if (dot)  GdipDeleteBrush((GpBrush*)dot);
            if (ring) GdipDeletePen(ring);
        }
        GdipDeleteGraphics(g);
    }

    BitBlt(dis->hDC, rc.left, rc.top, w, h, hdcMem, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
}

// ---------------------------------------------------------------------------
// Legend
// ---------------------------------------------------------------------------

static void DrawEdgeLegend(OptionsDlgData* data, const DRAWITEMSTRUCT* dis)
{
    const Theme* t = data->theme;
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;

    FillRect(hdc, &rc, data->hBrushBg);

    HFONT hOldFont = SelectObject(hdc, data->hFontUi);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, ToColorRef(t->subtle));

    struct { ARGB clr; const WCHAR* label; } items[] = {
        { t->edgeWrap,    L"Wrap" },
        { t->edgeDelayed, L"Delayed" },
        { t->edgeNoWrap,  L"No Wrap" },
    };

    GpGraphics* g = BeginGdipDraw(hdc);
    int x = rc.left + Px(data, 2);
    int cy = (rc.top + rc.bottom) / 2;
    int lineLen = Px(data, 18), gap = Px(data, 6);
    REAL lineW = (REAL)Px(data, 3);

    for (int i = 0; i < 3; i++) {
        if (g) DrawRoundedLine(g, items[i].clr, lineW, x, cy, x + lineLen, cy);
        x += lineLen + gap;
        SIZE sz;
        int len = (int)wcslen(items[i].label);
        GetTextExtentPoint32W(hdc, items[i].label, len, &sz);
        TextOutW(hdc, x, cy - sz.cy / 2, items[i].label, len);
        x += sz.cx + gap * 2;
    }
    if (g) GdipDeleteGraphics(g);
    SelectObject(hdc, hOldFont);
}

// ---------------------------------------------------------------------------
// Combobox (owner-drawn so the read-only face follows the palette; the
// themed border/button come from DarkMode_CFD / CFD)
// ---------------------------------------------------------------------------

static void DrawComboItem(OptionsDlgData* data, const DRAWITEMSTRUCT* dis)
{
    const Theme* t = data->theme;
    BOOL isField  = (dis->itemState & ODS_COMBOBOXEDIT) != 0;
    BOOL selected = (dis->itemState & ODS_SELECTED) != 0;

    ARGB bg   = isField ? t->card : (selected ? data->accent : t->bg);
    ARGB text = (!isField && selected) ? 0xFFFFFFFF : t->text;

    HBRUSH hBg = CreateSolidBrush(ToColorRef(bg));
    FillRect(dis->hDC, &dis->rcItem, hBg);
    DeleteObject(hBg);

    if (dis->itemID != (UINT)-1) {
        WCHAR buf[64] = L"";
        SendMessageW(dis->hwndItem, CB_GETLBTEXT, dis->itemID, (LPARAM)buf);
        HFONT hOld = SelectObject(dis->hDC, data->hFontUi);
        SetBkMode(dis->hDC, TRANSPARENT);
        SetTextColor(dis->hDC, ToColorRef(text));
        RECT rc = dis->rcItem;
        rc.left += Px(data, 8);
        DrawTextW(dis->hDC, buf, -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(dis->hDC, hOld);
    }
}

// ---------------------------------------------------------------------------
// Slider custom draw (Windows 11 style: thin rounded channel, round thumb)
// ---------------------------------------------------------------------------

static LRESULT SliderCustomDraw(OptionsDlgData* data, NMCUSTOMDRAW* cd)
{
    const Theme* t = data->theme;
    if (cd->dwDrawStage == CDDS_PREPAINT)
        return CDRF_NOTIFYITEMDRAW;
    if (cd->dwDrawStage != CDDS_ITEMPREPAINT)
        return CDRF_DODEFAULT;

    if (cd->dwItemSpec == TBCD_TICS)
        return CDRF_SKIPDEFAULT; // no tick marks

    GpGraphics* g = BeginGdipDraw(cd->hdc);
    if (!g) return CDRF_DODEFAULT;

    if (cd->dwItemSpec == TBCD_CHANNEL) {
        RECT thumb;
        SendMessageW(cd->hdr.hwndFrom, TBM_GETTHUMBRECT, 0, (LPARAM)&thumb);
        int thumbCx = (thumb.left + thumb.right) / 2;
        int chH = Px(data, 4);
        int cy = (cd->rc.top + cd->rc.bottom) / 2;
        REAL y = (REAL)(cy - chH / 2), hgt = (REAL)chH;
        GpPath* full = RoundRectPath((REAL)cd->rc.left, y, (REAL)(cd->rc.right - cd->rc.left), hgt, hgt / 2);
        GpPath* done = RoundRectPath((REAL)cd->rc.left, y, (REAL)max(thumbCx - cd->rc.left, chH), hgt, hgt / 2);
        GpSolidFill* bTrack = NULL; GpSolidFill* bDone = NULL;
        GdipCreateSolidFill(t->track, &bTrack);
        GdipCreateSolidFill(data->accent, &bDone);
        if (full && bTrack) GdipFillPath(g, (GpBrush*)bTrack, full);
        if (done && bDone)  GdipFillPath(g, (GpBrush*)bDone, done);
        if (bTrack) GdipDeleteBrush((GpBrush*)bTrack);
        if (bDone)  GdipDeleteBrush((GpBrush*)bDone);
        if (full) GdipDeletePath(full);
        if (done) GdipDeletePath(done);
    } else if (cd->dwItemSpec == TBCD_THUMB) {
        // Erase the default thumb area with the dialog background, then draw a
        // ring + accent dot.
        FillRect(cd->hdc, &cd->rc, data->hBrushBg);
        int size = min(cd->rc.right - cd->rc.left, cd->rc.bottom - cd->rc.top);
        REAL cx = (REAL)((cd->rc.left + cd->rc.right) / 2);
        REAL cy = (REAL)((cd->rc.top + cd->rc.bottom) / 2);
        REAL rOuter = (REAL)size / 2 - 1, rInner = rOuter * 0.55f;
        GpSolidFill* bRing = NULL; GpSolidFill* bDot = NULL; GpPen* pBorder = NULL;
        GdipCreateSolidFill(t->thumbRing, &bRing);
        GdipCreateSolidFill(data->accent, &bDot);
        GdipCreatePen1(t->cardBorder, 1.0f, UnitPixel, &pBorder);
        if (bRing) GdipFillEllipse(g, (GpBrush*)bRing, cx - rOuter, cy - rOuter, rOuter * 2, rOuter * 2);
        if (pBorder) GdipDrawEllipse(g, pBorder, cx - rOuter, cy - rOuter, rOuter * 2, rOuter * 2);
        if (bDot)  GdipFillEllipse(g, (GpBrush*)bDot, cx - rInner, cy - rInner, rInner * 2, rInner * 2);
        if (bRing) GdipDeleteBrush((GpBrush*)bRing);
        if (bDot)  GdipDeleteBrush((GpBrush*)bDot);
        if (pBorder) GdipDeletePen(pBorder);
    }
    GdipDeleteGraphics(g);
    return CDRF_SKIPDEFAULT;
}

// The trackbar erases itself with COLOR_WINDOW (white, even in dark mode)
// before every repaint, which flashes while dragging.  This subclass skips
// the erase and paints the control double-buffered: dialog background first,
// then the control (and our custom draw) into a memory bitmap, blitted once.
#define SLIDER_SUBCLASS_ID 1

static LRESULT CALLBACK SliderSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                           UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    OptionsDlgData* data = (OptionsDlgData*)dwRefData;
    switch (msg) {
    case WM_ERASEBKGND:
        return 1; // background is painted in WM_PAINT

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP hOld = SelectObject(hdcMem, hBmp);
        if (data && data->hBrushBg) FillRect(hdcMem, &rc, data->hBrushBg);
        // Common controls paint into the DC passed as wParam when given one.
        DefSubclassProc(hwnd, WM_PAINT, (WPARAM)hdcMem, 0);
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, hOld);
        DeleteObject(hBmp);
        DeleteDC(hdcMem);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_NCDESTROY:
        RemoveWindowSubclass(hwnd, SliderSubclassProc, uIdSubclass);
        break;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// DPI change: move to the suggested rect and rescale every child + font
// ---------------------------------------------------------------------------

typedef struct { HWND hDlg; UINT oldDpi, newDpi; } DpiScaleCtx;

static BOOL CALLBACK ScaleChildProc(HWND hChild, LPARAM lParam)
{
    DpiScaleCtx* ctx = (DpiScaleCtx*)lParam;
    RECT rc;
    GetWindowRect(hChild, &rc);
    MapWindowPoints(NULL, ctx->hDlg, (POINT*)&rc, 2);
    int x = MulDiv(rc.left, (int)ctx->newDpi, (int)ctx->oldDpi);
    int y = MulDiv(rc.top,  (int)ctx->newDpi, (int)ctx->oldDpi);
    int w = MulDiv(rc.right - rc.left,  (int)ctx->newDpi, (int)ctx->oldDpi);
    int h = MulDiv(rc.bottom - rc.top,  (int)ctx->newDpi, (int)ctx->oldDpi);
    SetWindowPos(hChild, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
    return TRUE;
}

static void OnDpiChanged(HWND hDlg, OptionsDlgData* data, UINT newDpi, const RECT* suggested)
{
    DpiScaleCtx ctx = { hDlg, data->dpi, newDpi };
    if (ctx.oldDpi == 0) ctx.oldDpi = 96;

    // Children first (their rects are read relative to the old client origin),
    // then the frame.
    EnumChildWindows(hDlg, ScaleChildProc, (LPARAM)&ctx);
    SetWindowPos(hDlg, NULL, suggested->left, suggested->top,
                 suggested->right - suggested->left, suggested->bottom - suggested->top,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    data->dpi = newDpi;
    RebuildFonts(hDlg, data);
    RedrawWindow(hDlg, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
}

// ---------------------------------------------------------------------------
// Interaction
// ---------------------------------------------------------------------------

static void HandlePreviewClick(HWND hDlg, OptionsDlgData* data)
{
    HWND hPreview = GetDlgItem(hDlg, IDC_MONITOR_PREVIEW);
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hPreview, &pt);

    int idx = HitTestEdge(data, pt);
    if (idx < 0) return;

    EdgeState s = data->edgeState[idx];
    EdgeState next = (s == EDGE_WRAP) ? EDGE_DELAYED : (s == EDGE_DELAYED) ? EDGE_NOWRAP : EDGE_WRAP;
    data->edgeState[idx] = next;
    CycleEdgeState(data->contourEdges[idx]);
    InvalidateRect(hPreview, NULL, FALSE);
}

// Called on every timer tick: track which edge is under the cursor so the
// preview can highlight it, and refresh the live cursor dot.
static void OnTrackTimer(HWND hDlg, OptionsDlgData* data)
{
    HWND hPreview = GetDlgItem(hDlg, IDC_MONITOR_PREVIEW);
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hPreview, &pt);
    RECT rc;
    GetClientRect(hPreview, &rc);
    int idx = PtInRect(&rc, pt) ? HitTestEdge(data, pt) : -1;
    if (idx != data->hoverIdx) {
        data->hoverIdx = idx;
        SetCursor(LoadCursorW(NULL, idx >= 0 ? IDC_HAND : IDC_ARROW));
    }
    InvalidateRect(hPreview, NULL, FALSE);
}

static void CreateHintTooltip(HWND hDlg, OptionsDlgData* data)
{
    HWND hTip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, NULL,
                                WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
                                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                hDlg, NULL, GetModuleHandleW(NULL), NULL);
    if (!hTip) return;
    data->hTip = hTip;
    SendMessageW(hTip, TTM_SETMAXTIPWIDTH, 0, Px(data, 280));
    SendMessageW(hTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 15000);

    static WCHAR dragHint[] =
        L"While the mouse button is down, this setting takes precedence over each "
        L"edge's Wrap / Delayed setting. Edges set to \"No Wrap\" never wrap.";
    static WCHAR previewHint[] =
        L"Click an edge to cycle it: Wrap \x2192 Delayed \x2192 No Wrap.";

    struct { int id; WCHAR* text; } tools[] = {
        { IDC_DRAG_MODE_LABEL, dragHint },
        { IDC_DRAG_MODE_COMBO, dragHint },
        { IDC_MONITOR_PREVIEW, previewHint },
    };
    for (size_t i = 0; i < sizeof(tools) / sizeof(tools[0]); i++) {
        TOOLINFOW ti;
        memset(&ti, 0, sizeof(ti));
        ti.cbSize   = sizeof(ti); // comctl32 v6 via the manifest
        ti.uFlags   = TTF_IDISHWND | TTF_SUBCLASS;
        ti.hwnd     = hDlg;
        ti.uId      = (UINT_PTR)GetDlgItem(hDlg, tools[i].id);
        ti.lpszText = tools[i].text;
        SendMessageW(hTip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
    }
}

static void DestroyDialogResources(HWND hDlg, OptionsDlgData* data)
{
    KillTimer(hDlg, IDT_CURSOR_TRACK);
    if (!data) return;
    if (data->hBrushBg)   { DeleteObject(data->hBrushBg);   data->hBrushBg = NULL; }
    if (data->hFontUi)    { DeleteObject(data->hFontUi);    data->hFontUi = NULL; }
    if (data->hFontLabel) { DeleteObject(data->hFontLabel); data->hFontLabel = NULL; }
}

// ---------------------------------------------------------------------------
// Dialog procedure
// ---------------------------------------------------------------------------

static INT_PTR CALLBACK OptionsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    OptionsDlgData* data = (OptionsDlgData*)GetWindowLongPtr(hDlg, GWLP_USERDATA);

    switch (uMsg) {
    case WM_INITDIALOG: {
        data = (OptionsDlgData*)lParam;
        SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)data);
        data->hoverIdx = -1;
        data->dpi = GetDpiForWindow(hDlg);
        if (data->dpi == 0) data->dpi = 96;

        // Snapshot monitors and contour edges
        data->count = 0;
        if (g_monitor_rects_desktop && g_monitor_count_desktop > 0) {
            SIZE_T n = min(g_monitor_count_desktop, (SIZE_T)MAX_PREVIEW_MONITORS);
            for (SIZE_T i = 0; i < n; i++) data->rects[i] = g_monitor_rects_desktop[i];
            data->count = n;
        }
        data->contourCount = 0;
        if (g_desktop_contour && g_desktop_contour->size > 0) {
            SIZE_T n = min(g_desktop_contour->size, (SIZE_T)MAX_CONTOUR_EDGES);
            for (SIZE_T i = 0; i < n; i++) {
                data->contourEdges[i] = g_desktop_contour->edges[i];
                data->edgeState[i] = GetEdgeState(g_desktop_contour->edges[i]);
            }
            data->contourCount = n;
        }

        // Windows 11: Mica behind the title bar (no-op elsewhere)
        {
            int backdrop = DWMSBT_MAINWINDOW;
            DwmSetWindowAttribute(hDlg, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
        }

        // Delay slider (200–1000 ms, snaps to 100)
        {
            HWND hSlider = GetDlgItem(hDlg, IDC_DELAY_SLIDER);
            SendMessageW(hSlider, TBM_SETRANGE, TRUE, MAKELPARAM(200, 1000));
            SendMessageW(hSlider, TBM_SETTICFREQ, 100, 0);
            SendMessageW(hSlider, TBM_SETLINESIZE, 0, 100);
            SendMessageW(hSlider, TBM_SETPAGESIZE, 0, 100);
            SendMessageW(hSlider, TBM_SETPOS, TRUE, (LPARAM)g_edge_delay_ms);
            SetWindowSubclass(hSlider, SliderSubclassProc, SLIDER_SUBCLASS_ID, (DWORD_PTR)data);
            WCHAR buf[32];
            wsprintfW(buf, L"Delay: %lu ms", g_edge_delay_ms);
            SetDlgItemTextW(hDlg, IDC_DELAY_LABEL, buf);
        }

        // "While dragging" dropdown — order matches DragWrapMode
        {
            HWND hCombo = GetDlgItem(hDlg, IDC_DRAG_MODE_COMBO);
            static const WCHAR* modes[] = { L"Delayed Wrap", L"Wrap instantly", L"No Wrap" };
            for (int i = 0; i < 3; i++)
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)modes[i]);
            SendMessageW(hCombo, CB_SETCURSEL, (WPARAM)g_drag_wrap_mode, 0);
        }

        CreateHintTooltip(hDlg, data);
        RebuildFonts(hDlg, data);
        ApplyTheme(hDlg, data);

        // Hide focus rectangles until the user navigates with the keyboard
        // (Windows clears this automatically on Tab / arrow keys).
        SendMessageW(hDlg, WM_CHANGEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS | UISF_HIDEACCEL), 0);

        SetTimer(hDlg, IDT_CURSOR_TRACK, CURSOR_TRACK_MS, NULL);
        return TRUE;
    }

    case WM_TIMER:
        if (data && wParam == IDT_CURSOR_TRACK) {
            OnTrackTimer(hDlg, data);
            return TRUE;
        }
        break;

    case WM_DPICHANGED:
        if (data) {
            OnDpiChanged(hDlg, data, HIWORD(wParam), (const RECT*)lParam);
            return TRUE;
        }
        break;

    case WM_SETTINGCHANGE:
        // Live light/dark switch (also fires for accent colour changes)
        if (data && lParam && lstrcmpiW((LPCWSTR)lParam, L"ImmersiveColorSet") == 0) {
            ApplyTheme(hDlg, data);
            return TRUE;
        }
        break;

    case WM_ERASEBKGND:
        if (data && data->hBrushBg) {
            RECT rc;
            GetClientRect(hDlg, &rc);
            FillRect((HDC)wParam, &rc, data->hBrushBg);
            SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 1);
            return TRUE;
        }
        break;

    case WM_DRAWITEM:
        if (data) {
            DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
            if (dis->CtlID == IDC_MONITOR_PREVIEW) { DrawMonitorPreview(data, dis); return TRUE; }
            if (dis->CtlID == IDC_EDGE_LEGEND)     { DrawEdgeLegend(data, dis);     return TRUE; }
            if (dis->CtlID == IDC_DRAG_MODE_COMBO) { DrawComboItem(data, dis);      return TRUE; }
        }
        break;

    case WM_NOTIFY:
        if (data) {
            NMHDR* hdr = (NMHDR*)lParam;
            if (hdr->idFrom == IDC_DELAY_SLIDER && hdr->code == NM_CUSTOMDRAW) {
                SetWindowLongPtr(hDlg, DWLP_MSGRESULT, SliderCustomDraw(data, (NMCUSTOMDRAW*)lParam));
                return TRUE;
            }
        }
        break;

    case WM_CTLCOLORDLG:
        if (data && data->hBrushBg) return (INT_PTR)data->hBrushBg;
        break;

    case WM_CTLCOLORSTATIC:
        if (data && data->hBrushBg) {
            HDC hdc = (HDC)wParam;
            int id = GetDlgCtrlID((HWND)lParam);
            BOOL subtle = (id == IDC_LAYOUT_HINT);
            SetTextColor(hdc, ToColorRef(subtle ? data->theme->subtle : data->theme->text));
            SetBkMode(hdc, TRANSPARENT);
            return (INT_PTR)data->hBrushBg;
        }
        break;

    case WM_CTLCOLORLISTBOX:
        if (data && data->dark && data->hBrushBg) {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, ToColorRef(data->theme->text));
            SetBkColor(hdc, ToColorRef(data->theme->bg));
            return (INT_PTR)data->hBrushBg;
        }
        break;

    case WM_HSCROLL:
        if (data && (HWND)lParam == GetDlgItem(hDlg, IDC_DELAY_SLIDER)) {
            DWORD raw = (DWORD)SendMessageW((HWND)lParam, TBM_GETPOS, 0, 0);
            DWORD pos = ((raw + 50) / 100) * 100; // snap to 100s
            if (pos < 200) pos = 200;
            if (pos > 1000) pos = 1000;
            SendMessageW((HWND)lParam, TBM_SETPOS, TRUE, (LPARAM)pos);
            // The trackbar only invalidates the thumb's old/new rects, but our
            // channel fill depends on the thumb position: repaint all of it
            // (no erase: the subclass paints the background itself).
            InvalidateRect((HWND)lParam, NULL, FALSE);
            g_edge_delay_ms = pos;
            SaveDelayMs();
            WCHAR buf[32];
            wsprintfW(buf, L"Delay: %lu ms", pos);
            SetDlgItemTextW(hDlg, IDC_DELAY_LABEL, buf);
            return TRUE;
        }
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL) {
            DestroyDialogResources(hDlg, data);
            EndDialog(hDlg, 0);
            return TRUE;
        }
        if (data && LOWORD(wParam) == IDC_MONITOR_PREVIEW && HIWORD(wParam) == STN_CLICKED) {
            HandlePreviewClick(hDlg, data);
            return TRUE;
        }
        if (LOWORD(wParam) == IDC_DRAG_MODE_COMBO && HIWORD(wParam) == CBN_SELCHANGE) {
            LRESULT sel = SendMessageW((HWND)lParam, CB_GETCURSEL, 0, 0);
            if (sel >= DRAG_WRAP_DELAYED && sel <= DRAG_WRAP_NONE) {
                g_drag_wrap_mode = (DragWrapMode)sel;
                SaveDragWrapMode();
            }
            return TRUE;
        }
        break;

    case WM_CLOSE:
        DestroyDialogResources(hDlg, data);
        EndDialog(hDlg, 0);
        return TRUE;
    }

    return FALSE;
}

void ShowOptionsDialog(HWND hwndParent)
{
    extern HINSTANCE hInst;

    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_BAR_CLASSES | ICC_WIN95_CLASSES };
    InitCommonControlsEx(&icc);

    ULONG_PTR gdipToken = 0;
    GdiplusStartupInput gdipIn = { 1, NULL, FALSE, FALSE };
    BOOL gdipOk = (GdiplusStartup(&gdipToken, &gdipIn, NULL) == Ok);

    OptionsDlgData data;
    memset(&data, 0, sizeof(data));
    DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_OPTIONS), hwndParent, OptionsDlgProc, (LPARAM)&data);

    if (gdipOk) GdiplusShutdown(gdipToken);
}
