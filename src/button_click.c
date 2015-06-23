#include <pebble.h>
	
enum {
	KEY_BUTTON_EVENT = 0,
	BUTTON_EVENT_UP = 1,
	BUTTON_EVENT_DOWN = 2,
	BUTTON_EVENT_SELECT = 3,
	KEY_VIBRATION = 4
};

static Window *window;
static TextLayer *text_layer;

void send_int(uint8_t key, uint8_t cmd) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
      
    Tuplet value = TupletInteger(key, cmd);
    dict_write_tuplet(iter, &value);
      
    app_message_outbox_send();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
	send_int(KEY_BUTTON_EVENT, BUTTON_EVENT_SELECT);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Adding Person");
	send_int(KEY_BUTTON_EVENT, BUTTON_EVENT_UP);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Recognising");
	send_int(KEY_BUTTON_EVENT, BUTTON_EVENT_DOWN);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
    Tuple *t = dict_read_first(iter);
    if(t) {
    	vibes_short_pulse();
			text_layer_set_text(text_layer, t->value->cstring);
		}
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  text_layer = text_layer_create(GRect(0, 0, 144, 168));
  text_layer_set_text(text_layer, "Nellie");
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	text_layer_set_overflow_mode(text_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
	
	//Register AppMessage events
	app_message_register_inbox_received(in_received_handler);           
	app_message_open(512, 512);    //Large input and output buffer sizes
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}