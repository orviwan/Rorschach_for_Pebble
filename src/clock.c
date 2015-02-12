#include "clock.h"
#include <pebble.h>

static Window *s_window;

static int current_frame = 0;

#define TOTAL_FRAMES 10
BitmapLayer *background_layer, *face_layer;
GBitmap *current_image;

static TextLayer *s_time_layer;
static char time_text[] = "00:00";


const int ANIM_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_01,
  RESOURCE_ID_IMAGE_02,
  RESOURCE_ID_IMAGE_03,
  RESOURCE_ID_IMAGE_04,
  RESOURCE_ID_IMAGE_05,
  RESOURCE_ID_IMAGE_06,
  RESOURCE_ID_IMAGE_07,
  RESOURCE_ID_IMAGE_08,
  RESOURCE_ID_IMAGE_09,
  RESOURCE_ID_IMAGE_10
};

void change_face() {
	gbitmap_destroy(current_image);
	if(current_frame >= TOTAL_FRAMES) {
		current_frame = 0;
	}
	current_image = gbitmap_create_with_resource(ANIM_IMAGE_RESOURCE_IDS[current_frame]);
	bitmap_layer_set_bitmap(face_layer, current_image);
	layer_mark_dirty(bitmap_layer_get_layer(face_layer));
	current_frame++;
}


static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, true);

	GBitmap *background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
	background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(background_layer, background);
  layer_add_child(window_get_root_layer(s_window), bitmap_layer_get_layer(background_layer));	
	
	face_layer = bitmap_layer_create(GRect(42, 50, 59, 44));
  layer_add_child(bitmap_layer_get_layer(background_layer), bitmap_layer_get_layer(face_layer));
	
  s_time_layer = text_layer_create(GRect(10, 122, 134, 40));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  layer_add_child(bitmap_layer_get_layer(background_layer), (Layer *)s_time_layer);	
}

static void destroy_ui(void) {
  window_destroy(s_window);
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	if(units_changed & DAY_UNIT) {
	}
  if (units_changed & HOUR_UNIT) {
  }
	if (units_changed & MINUTE_UNIT) {
		if(clock_is_24h_style()) {
			strftime(time_text, sizeof(time_text), "%H:%M", tick_time);
		}
		else {
			strftime(time_text, sizeof(time_text), "%I:%M", tick_time);	
			if (time_text[0] == '0') {
				memmove(&time_text[0], &time_text[1], sizeof(time_text) - 1); //remove leading zero
			}
		}    
		text_layer_set_text(s_time_layer, time_text);
  } 
  if (units_changed & SECOND_UNIT) {
		change_face();
  }  
}

static void handle_window_unload(Window* window) {
    destroy_ui();
}

void show_clock(void) {
    initialise_ui();
    window_set_window_handlers(s_window, (WindowHandlers) {
        .unload = handle_window_unload,
    });
    window_stack_push(s_window, true);

	  //Somebody set us up the CLOCK
    time_t now = time(NULL);
    struct tm *tick_time = localtime(&now);  

    handle_tick(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT + SECOND_UNIT);
    tick_timer_service_subscribe(SECOND_UNIT, handle_tick);

}

void hide_clock(void) {
    window_stack_remove(s_window, true);
}
