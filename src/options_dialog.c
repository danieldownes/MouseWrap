#include "options_dialog.h"
#include "resource.h"
#include "mouse_wrap.h"
#include "multimonitor_edges.h"
#include "multimonitor_contour.h"
#include <windowsx.h>
#include <uxtheme.h>

// Forward-declare IsDarkTheme from taskbar.c
BOOL IsDarkTheme(void);

// --- Dark mode via uxtheme.dll undocumented ordinals ---

typedef BOOL (WINAPI *AllowDarkModeForWindowFunc)(HWND, BOOL);
typedef BOOL (WINAPI *SetPreferredAppModeFunc)(int);
typedef void (WINAPI *RefreshImmersiveColorPolicyStateFunc)(void);

static AllowDarkModeForWindowFunc pAllowDarkModeForWindow = NULL;
static SetPreferredAppModeFunc pSetPreferredAppMode = NULL;
static RefreshImmersiveColorPolicyStateFunc pRefreshImmersiveColorPolicyState = NULL;

void InitDarkMode(void)
{
    HMODULE hUxTheme = LoadLibraryW(L"uxtheme.dll");
    if (!hUxTheme) return;

    pSetPreferredAppMode = (SetPreferredAppModeFunc)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(135));
    pAllowDarkModeForWindow = (AllowDarkModeForWindowFunc)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(133));
    pRefreshImmersiveColorPolicyState = (RefreshImmersiveColorPolicyStateFunc)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(104));

    if (pSetPreferredAppMode)
        pSetPreferredAppMode(1); // AllowDark
    if (pRefreshImmersiveColorPolicyState)
        pRefreshImmersiveColorPolicyState();
}

// --- Color scheme ---

#define CLR_DARK_BG         RGB(32, 32, 32)
#define CLR_DARK_PREVIEW_BG RGB(30, 30, 30)
#define CLR_DARK_MON_FILL   RGB(56, 100, 170)
#define CLR_DARK_MON_BORDER RGB(40, 80, 140)

#define CLR_LIGHT_PREVIEW_BG RGB(240, 240, 240)
#define CLR_LIGHT_MON_FILL   RGB(76, 128, 200)
#define CLR_LIGHT_MON_BORDER RGB(0, 90, 158)

// Edge colors
#define CLR_EDGE_ENABLED_LIGHT  RGB(0, 180, 0)
#define CLR_EDGE_ENABLED_DARK   RGB(0, 200, 80)
#define CLR_EDGE_DISABLED_LIGHT RGB(200, 0, 0)
#define CLR_EDGE_DISABLED_DARK  RGB(220, 50, 50)

// --- Dialog state ---

#define MAX_PREVIEW_MONITORS 16
#define MAX_CONTOUR_EDGES    128

typedef struct {
    me_Rect  rects[MAX_PREVIEW_MONITORS];
    SIZE_T   count;
    me_Edge  contourEdges[MAX_CONTOUR_EDGES];
    BOOL     edgeDisabled[MAX_CONTOUR_EDGES];
    SIZE_T   contourCount;
    BOOL     dark;
    HBRUSH   hBrushBg;
    // Cached transform for hit testing (set during draw)
    double   scale;
    int      offX, offY;
    int      bbLeft, bbTop;
} OptionsDlgData;

static void DrawMonitorPreview(OptionsDlgData* data, const DRAWITEMSTRUCT* dis)
{
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;

    // Preview background
    COLORREF bgColor = data->dark ? CLR_DARK_PREVIEW_BG : CLR_LIGHT_PREVIEW_BG;
    HBRUSH hBgBrush = CreateSolidBrush(bgColor);
    FillRect(hdc, &rc, hBgBrush);
    DeleteObject(hBgBrush);

    if (data->count == 0) return;

    // Compute bounding box of all monitors
    int bbLeft   = data->rects[0].xMin;
    int bbTop    = data->rects[0].yMin;
    int bbRight  = data->rects[0].xMax;
    int bbBottom = data->rects[0].yMax;
    for (SIZE_T i = 1; i < data->count; i++) {
        if (data->rects[i].xMin < bbLeft)   bbLeft   = data->rects[i].xMin;
        if (data->rects[i].yMin < bbTop)    bbTop    = data->rects[i].yMin;
        if (data->rects[i].xMax > bbRight)  bbRight  = data->rects[i].xMax;
        if (data->rects[i].yMax > bbBottom) bbBottom = data->rects[i].yMax;
    }

    int bbW = bbRight - bbLeft;
    int bbH = bbBottom - bbTop;
    if (bbW <= 0 || bbH <= 0) return;

    int padding = 8;
    int ctrlW = (rc.right - rc.left) - padding * 2;
    int ctrlH = (rc.bottom - rc.top) - padding * 2;
    if (ctrlW <= 0 || ctrlH <= 0) return;

    // Uniform scale preserving aspect ratio
    double scaleX = (double)ctrlW / bbW;
    double scaleY = (double)ctrlH / bbH;
    double scale = (scaleX < scaleY) ? scaleX : scaleY;

    // Center the layout
    int scaledW = (int)(bbW * scale);
    int scaledH = (int)(bbH * scale);
    int offX = rc.left + padding + (ctrlW - scaledW) / 2;
    int offY = rc.top  + padding + (ctrlH - scaledH) / 2;

    // Cache transform for hit testing
    data->scale  = scale;
    data->offX   = offX;
    data->offY   = offY;
    data->bbLeft = bbLeft;
    data->bbTop  = bbTop;

    // Monitor colors
    COLORREF fillColor   = data->dark ? CLR_DARK_MON_FILL   : CLR_LIGHT_MON_FILL;
    COLORREF borderColor = data->dark ? CLR_DARK_MON_BORDER : CLR_LIGHT_MON_BORDER;

    HBRUSH hFillBrush = CreateSolidBrush(fillColor);
    HPEN   hBorderPen = CreatePen(PS_SOLID, 2, borderColor);
    HBRUSH hOldBrush = SelectObject(hdc, hFillBrush);
    HPEN   hOldPen   = SelectObject(hdc, hBorderPen);

    // Font for monitor labels
    HFONT hFont = CreateFontW(-14, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    HFONT hOldFont = SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    int gap = 2;

    for (SIZE_T i = 0; i < data->count; i++) {
        int left   = offX + (int)((data->rects[i].xMin - bbLeft) * scale) + gap;
        int top    = offY + (int)((data->rects[i].yMin - bbTop)  * scale) + gap;
        int right  = offX + (int)((data->rects[i].xMax - bbLeft) * scale) - gap;
        int bottom = offY + (int)((data->rects[i].yMax - bbTop)  * scale) - gap;

        RoundRect(hdc, left, top, right, bottom, 6, 6);

        // Draw monitor number centered
        WCHAR label[4];
        wsprintfW(label, L"%d", (int)(i + 1));
        RECT labelRc = { left, top, right, bottom };
        DrawTextW(hdc, label, -1, &labelRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBorderPen);
    DeleteObject(hFillBrush);

    // Draw contour edges as colored lines over the monitors
    COLORREF enabledColor  = data->dark ? CLR_EDGE_ENABLED_DARK  : CLR_EDGE_ENABLED_LIGHT;
    COLORREF disabledColor = data->dark ? CLR_EDGE_DISABLED_DARK : CLR_EDGE_DISABLED_LIGHT;

    for (SIZE_T i = 0; i < data->contourCount; i++) {
        me_Edge e = data->contourEdges[i];
        COLORREF clr = data->edgeDisabled[i] ? disabledColor : enabledColor;
        HPEN hEdgePen = CreatePen(PS_SOLID, 3, clr);
        HPEN hPrevPen = SelectObject(hdc, hEdgePen);

        int ex1 = offX + (int)((e.x1 - bbLeft) * scale);
        int ey1 = offY + (int)((e.y1 - bbTop)  * scale);
        int ex2 = offX + (int)((e.x2 - bbLeft) * scale);
        int ey2 = offY + (int)((e.y2 - bbTop)  * scale);

        MoveToEx(hdc, ex1, ey1, NULL);
        LineTo(hdc, ex2, ey2);

        SelectObject(hdc, hPrevPen);
        DeleteObject(hEdgePen);
    }
}

static void HandlePreviewClick(HWND hDlg, OptionsDlgData* data)
{
    if (data->contourCount == 0 || data->scale <= 0.0) return;

    HWND hPreview = GetDlgItem(hDlg, IDC_MONITOR_PREVIEW);
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hPreview, &pt);

    int bestIdx = -1;
    int bestDist = 7; // 6px threshold + 1

    for (SIZE_T i = 0; i < data->contourCount; i++) {
        me_Edge e = data->contourEdges[i];

        int ex1 = data->offX + (int)((e.x1 - data->bbLeft) * data->scale);
        int ey1 = data->offY + (int)((e.y1 - data->bbTop)  * data->scale);
        int ex2 = data->offX + (int)((e.x2 - data->bbLeft) * data->scale);
        int ey2 = data->offY + (int)((e.y2 - data->bbTop)  * data->scale);

        int dist = INT_MAX;
        if (ex1 == ex2) {
            // Vertical edge
            int yLo = (ey1 < ey2) ? ey1 : ey2;
            int yHi = (ey1 > ey2) ? ey1 : ey2;
            if (pt.y >= yLo && pt.y <= yHi)
                dist = abs(pt.x - ex1);
        } else {
            // Horizontal edge
            int xLo = (ex1 < ex2) ? ex1 : ex2;
            int xHi = (ex1 > ex2) ? ex1 : ex2;
            if (pt.x >= xLo && pt.x <= xHi)
                dist = abs(pt.y - ey1);
        }

        if (dist < bestDist) {
            bestDist = dist;
            bestIdx = (int)i;
        }
    }

    if (bestIdx >= 0) {
        data->edgeDisabled[bestIdx] = !data->edgeDisabled[bestIdx];
        ToggleEdgeDisabled(data->contourEdges[bestIdx]);
        InvalidateRect(GetDlgItem(hDlg, IDC_MONITOR_PREVIEW), NULL, TRUE);
    }
}

static INT_PTR CALLBACK OptionsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    OptionsDlgData* data;

    switch (uMsg) {
    case WM_INITDIALOG:
        data = (OptionsDlgData*)lParam;
        SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)data);

        data->dark = IsDarkTheme();

        // Copy monitor rects into local array for thread safety
        data->count = 0;
        if (g_monitor_rects_desktop && g_monitor_count_desktop > 0) {
            SIZE_T n = g_monitor_count_desktop;
            if (n > MAX_PREVIEW_MONITORS) n = MAX_PREVIEW_MONITORS;
            for (SIZE_T i = 0; i < n; i++)
                data->rects[i] = g_monitor_rects_desktop[i];
            data->count = n;
        }

        // Copy contour edges into local arrays
        data->contourCount = 0;
        if (g_desktop_contour && g_desktop_contour->size > 0) {
            SIZE_T n = g_desktop_contour->size;
            if (n > MAX_CONTOUR_EDGES) n = MAX_CONTOUR_EDGES;
            for (SIZE_T i = 0; i < n; i++) {
                data->contourEdges[i] = g_desktop_contour->edges[i];
                data->edgeDisabled[i] = IsEdgeDisabled(g_desktop_contour->edges[i]);
            }
            data->contourCount = n;
        }

        // Dark mode per-window
        if (data->dark) {
            if (pAllowDarkModeForWindow)
                pAllowDarkModeForWindow(hDlg, TRUE);
            if (pRefreshImmersiveColorPolicyState)
                pRefreshImmersiveColorPolicyState();

            // Dark title bar via DWMWA_USE_IMMERSIVE_DARK_MODE
            BOOL darkValue = TRUE;
            // Attribute 20 = DWMWA_USE_IMMERSIVE_DARK_MODE
            typedef HRESULT (WINAPI *DwmSetWindowAttributeFunc)(HWND, DWORD, LPCVOID, DWORD);
            HMODULE hDwm = GetModuleHandleW(L"dwmapi.dll");
            if (!hDwm) hDwm = LoadLibraryW(L"dwmapi.dll");
            if (hDwm) {
                DwmSetWindowAttributeFunc pDwmSetAttr =
                    (DwmSetWindowAttributeFunc)GetProcAddress(hDwm, "DwmSetWindowAttribute");
                if (pDwmSetAttr)
                    pDwmSetAttr(hDlg, 20, &darkValue, sizeof(darkValue));
            }

            data->hBrushBg = CreateSolidBrush(CLR_DARK_BG);

            // Theme Close button for dark mode
            HWND hBtn = GetDlgItem(hDlg, IDCANCEL);
            SetWindowTheme(hBtn, L"DarkMode_Explorer", NULL);
        } else {
            data->hBrushBg = NULL;
        }

        return TRUE;

    case WM_DRAWITEM:
        data = (OptionsDlgData*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
        if (data && ((DRAWITEMSTRUCT*)lParam)->CtlID == IDC_MONITOR_PREVIEW) {
            DrawMonitorPreview(data, (DRAWITEMSTRUCT*)lParam);
            return TRUE;
        }
        break;

    case WM_CTLCOLORDLG:
        data = (OptionsDlgData*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
        if (data && data->hBrushBg)
            return (INT_PTR)data->hBrushBg;
        break;

    case WM_CTLCOLORSTATIC:
        data = (OptionsDlgData*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
        if (data && data->dark) {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(255, 255, 255));
            SetBkMode(hdcStatic, TRANSPARENT);
            if (data->hBrushBg)
                return (INT_PTR)data->hBrushBg;
        }
        break;

    case WM_COMMAND:
        data = (OptionsDlgData*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
        if (LOWORD(wParam) == IDCANCEL) {
            if (data && data->hBrushBg)
                DeleteObject(data->hBrushBg);
            EndDialog(hDlg, 0);
            return TRUE;
        }
        if (data && LOWORD(wParam) == IDC_MONITOR_PREVIEW && HIWORD(wParam) == STN_CLICKED) {
            HandlePreviewClick(hDlg, data);
            return TRUE;
        }
        break;

    case WM_CLOSE:
        data = (OptionsDlgData*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
        if (data && data->hBrushBg)
            DeleteObject(data->hBrushBg);
        EndDialog(hDlg, 0);
        return TRUE;
    }

    return FALSE;
}

void ShowOptionsDialog(HWND hwndParent)
{
    extern HINSTANCE hInst;
    OptionsDlgData data;
    memset(&data, 0, sizeof(data));
    DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_OPTIONS), hwndParent, OptionsDlgProc, (LPARAM)&data);
}
