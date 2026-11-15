/* SPDX-License-Identifier: Apache-2.0 */
/* Synthetic Win32 layout AST. Compile for inspection only. */
#include <stddef.h>

typedef void *Handle;
typedef struct { const char *label; const wchar_t *caption; int x, y, width, height; } Key;
typedef struct { int left, top, right, bottom; } Rect;
typedef struct { int size, mask, min, max; } Scroll;

extern Handle CreateWindowExW(int, const wchar_t *, const wchar_t *, unsigned,
                              int, int, int, int, Handle, int);
extern Handle CreateFontW(int, const wchar_t *);
extern int MulDiv(int, int, int);
extern void SetPropW(Handle, const wchar_t *, const char *);
extern const char *GetPropW(Handle, const wchar_t *);
extern void enqueue_element(int *, const char *, int);
extern void use_rect(Rect);
extern void use_scroll(Scroll);
extern void use_label(const wchar_t *, Rect);
extern void use_size(int, int);

static struct {
    Handle display, buttons[17], stop, error_fields[6];
    int dpi, queue, sent;
} app;

static const Key keys[] = {
    {"7", L"7", 20, 70, 32, 25}, {"8", L"8", 58, 70, 32, 25},
    {"9", L"9", 96, 70, 32, 25}, {"/", L"/", 134, 70, 32, 25},
    {"4", L"4", 20, 101, 32, 25}, {"5", L"5", 58, 101, 32, 25},
    {"6", L"6", 96, 101, 32, 25}, {"*", L"*", 134, 101, 32, 25},
    {"1", L"1", 20, 132, 32, 25}, {"2", L"2", 58, 132, 32, 25},
    {"3", L"3", 96, 132, 32, 25}, {"-", L"-", 134, 132, 32, 25},
    {"0", L"0", 20, 163, 32, 25}, {".", L".", 58, 163, 32, 25},
    {"=", L"=", 96, 163, 32, 25}, {"+", L"+", 134, 163, 32, 25},
    {"ce", L"CE", 172, 70, 32, 25}
};

int scale(int value) { return MulDiv(value, app.dpi, 96); }

void move_control(Handle control, int x, int y, int width, int height)
{
    (void)control;
    use_rect((Rect){x, y, width, height});
}

void layout(void)
{
    (void)CreateFontW(-scale(16), L"Segoe UI");
    move_control(app.display, 20, 15, 184, 35);
    for (int i = 0; i < 17; ++i)
        move_control(app.buttons[i], keys[i].x, keys[i].y, keys[i].width, keys[i].height);
    move_control(app.stop, 20, 206, 75, 28);
    for (int group = 0; group < 2; ++group)
        for (int field = 0; field < 3; ++field)
            move_control(app.error_fields[group * 3 + field], group ? 256 : 20,
                         282 + field * 29, 190, 24);
    Scroll horizontal = {sizeof(Scroll), 0, 0, scale(480) - 1};
    Scroll vertical = {sizeof(Scroll), 0, 0, scale(390) - 1};
    use_scroll(horizontal);
    use_scroll(vertical);
}

void draw_background(int sx, int sy)
{
    Rect pad = {scale(15) - sx, scale(63) - sy, scale(210) - sx, scale(195) - sy};
    const wchar_t *labels[] = {L"Error input", L"Error output"};
    use_rect(pad);
    for (int group = 0; group < 2; ++group) {
        Rect area = {scale(group ? 256 : 20) - sx, scale(254) - sy,
                     scale(group ? 446 : 210) - sx, scale(278) - sy};
        use_label(labels[group], area);
    }
}

void initialize(int dpi)
{
    use_size(MulDiv(480, dpi, 96), MulDiv(390, dpi, 96));
    (void)CreateWindowExW(0, L"FrogFixture", L"Importer Fixture", 0, 0, 0, 480, 390, 0, 0);
}

void window_proc(Handle window)
{
    app.display = CreateWindowExW(0, L"STATIC", L"", 0x5000000D, 0, 0, 1, 1, window, 10);
    for (int i = 0; i < 17; ++i) {
        app.buttons[i] = CreateWindowExW(0, L"BUTTON", keys[i].caption, 0x5001000B, 0, 0, 1, 1, window, 100 + i);
        SetPropW(app.buttons[i], L"QMH.Label.Text", keys[i].label);
    }
    app.stop = CreateWindowExW(0, L"BUTTON", L"STOP", 0x5001000B, 0, 0, 1, 1, window, 20);
    for (int i = 0; i < 6; ++i)
        app.error_fields[i] = CreateWindowExW(0, L"EDIT", i % 3 == 0 ? L"false" : i % 3 == 1 ? L"0" : L"",
            0x40000000 | 0x10000000 | (i >= 3 ? 0x800 : 0x10000), 0, 0, 1, 1, window, 200 + i);
}

void producer_key(Handle source)
{
    const char *label = GetPropW(source, L"QMH.Label.Text");
    enqueue_element(&app.queue, label, ++app.sent);
}

void producer_close(void) {}
void host_abort(void) {}
void receive_snapshot(void) {}
void button_proc(void) {}
void consumer_thread(void) {}
