#include "ui/page.hpp"
#include "pages.hpp"
#include "graphics.hpp"

#include "images/milker_icon_0.h"
#include "images/milker_icon_1.h"
#include "images/milker_icon_2.h"
#include "images/milker_icon_3.h"

using namespace UI;
static const char* TAG = "PAGE:HOME";

static struct {
    int speed = 0;
    int icon_step = 2;
} state;

static graphics_image_t *MILKER_ICONS[4] = {
    &MILKER_ICON_0,
    &MILKER_ICON_1,
    &MILKER_ICON_2,
    &MILKER_ICON_3,
};

// label top = 0, bototm = 4
// large top = 7, bottom = 17
// bar top = 19, bottom = 32

static void draw_pressure_graph(m1k_hal_display_t *display, int min, int max, int lmark, int rmark, int value) {
    auto left = m1k_hal_get_display_left() + 23;
    auto width = m1k_hal_get_display_width() - 23;
    auto top = 19;

    // Down Arrow
    display->drawVLine(left + 2, top, 4);
    display->drawTriangle(left, top + 4, left + 5, top + 4, left + 2, top + 7);

    // Rect
    auto bar_left = left + 7;
    auto bar_width = width - 16;
    auto bar_center_x = bar_left + (bar_width / 2);
    double fill_space = floor(bar_width / 2.0);

    display->drawFrame(bar_left, top, bar_width, 7);
    display->drawVLine(bar_center_x, top, 7);

    if (value < 0) {
        double fill_width = floor( abs((double) value / min) * fill_space );
        display->drawBox(bar_center_x - fill_width, top + 2, fill_width, 3);
    } else {
        double fill_width = floor( abs((double) value / max) * fill_space );
        display->drawBox(bar_center_x, top + 2, fill_width + 1, 3);
    }

    // Up Arrow
    auto rtri_left = left + width - 7;
    display->drawVLine(rtri_left + 2, top + 3, 4);
    display->drawTriangle(rtri_left - 1, top + 3, rtri_left + 5, top + 3, rtri_left + 2, top - 1);

    // Lower Text
    display->setFont(u8g2_font_4x6_mf);
    const char *unit_center = "0KPA";
    auto unit_center_x = bar_center_x - (display->getStrWidth(unit_center) / 2) + 1;
    auto unit_top = top + 13;
    display->drawStr(unit_center_x, unit_top, unit_center);

    char unit_left[10] = "";
    snprintf(unit_left, 10, "%d", min);
    display->drawStr(bar_left, unit_top, unit_left);

    char unit_right[10] = "";
    snprintf(unit_right, 10, "%d", max);
    auto unit_right_x = bar_left + bar_width - display->getStrWidth(unit_right) + 1;
    display->drawStr(unit_right_x, unit_top, unit_right);
}

static void draw_speed(m1k_hal_display_t *display, int speed, bool selected) {
    auto left = m1k_hal_get_display_left() + 23;
    auto width = m1k_hal_get_display_width() - 23;
    auto center = left + width / 2;

    display->setFont(u8g2_font_4x6_mf);
    const char *label = "SPEED";
    auto label_width = display->getStrWidth(label);
    display->drawStr(center - floor(label_width / 2), 5, label);

    display->setFont(u8g2_font_7x14B_tn);
    char value[4] = "";
    snprintf(value, 4, "%d", speed);
    auto value_width = display->getStrWidth(value);
    display->drawStr(center - floor(value_width / 2) - 1, 17, value);

    if (selected) {
        int triangle_top = 9;
        int triangle_tip = 10;
        display->drawTriangle(center - triangle_tip, triangle_top, center - triangle_tip, triangle_top + 6, center - (triangle_tip + 3), triangle_top + 3);
        display->drawTriangle(center + triangle_tip - 1, triangle_top, center + triangle_tip - 1, triangle_top + 6, center + (triangle_tip + 2), triangle_top + 3);
    }
}

static void draw_depth(m1k_hal_display_t *display, int pressure_delta, bool selected) {
    const auto left = m1k_hal_get_display_left() + 23 + 7;

    display->setFont(u8g2_font_4x6_mf);
    const char *label = "DEPTH";
    auto label_width = display->getStrWidth(label);
    display->drawStr(left + 5, 5, label);

    const int plus_minus_top = 10;
    display->drawLine(left + 1, plus_minus_top, left + 1, plus_minus_top + 2);
    display->drawLine(left, plus_minus_top + 1, left + 2, plus_minus_top + 1);
    display->drawLine(left, plus_minus_top + 5, left + 2, plus_minus_top + 5);

    display->setFont(u8g2_font_7x14B_tn);
    const int value_center = left + (label_width / 2) + 5;
    char value[4] = "";
    snprintf(value, 4, "%d", pressure_delta);
    int value_width = display->getStrWidth(value);
    display->drawStr(value_center - floor(value_width / 2) - 1, 17, value);
}

static void render(m1k_hal_display_t* display, Page* page) {
    auto milker_icon = MILKER_ICONS[state.icon_step];

    display->setDrawColor(1);
    graphics_draw_image(0, 0, milker_icon);
    draw_pressure_graph(display, -10, 15, -5, 5, state.speed);
    draw_speed(display, state.speed, true);
    draw_depth(display, 128, false);
}

static void loop(Page* page) {
    
}

static void exit(Page* page) {
    state.speed = 0;
    ESP_LOGE(TAG, "HOME EXIT");
}

static void enter(Page* page) {
    state.speed = 0;
    ESP_LOGE(TAG, "HOME ENTER");
}

static void encoder_change(int difference, Page* page) {
    state.speed += difference;
    page->render();
}

static page_config_t config = {
    .enter_cb = &enter,
    .render_cb = &render,
    .loop_cb = &loop,
    .exit_cb = &exit,
    .button_cb = nullptr,
    .encoder_cb = &encoder_change,
};

Page Pages::Home(&config);