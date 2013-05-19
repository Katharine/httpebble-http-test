
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "http.h"

//#define MY_UUID { 0xBD, 0x9D, 0x10, 0xBB, 0xBB, 0xBA, 0x4B, 0xFB, 0xB8, 0xCF, 0x37, 0x5E, 0x64, 0x99, 0x29, 0xA1 }
PBL_APP_INFO_SIMPLE(HTTP_UUID, "Button App", "Demo Corp", 1 /* App version */);


Window window;

TextLayer textLayer;


// Modify these common button handlers

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	DictionaryIterator* dict;
	http_out_get("http://pwdb.kathar.in/pebble/test.json", 17, &dict);
	http_out_send();
	text_layer_set_text(&textLayer, "Sending.");
}

// This usually won't need to be modified

void click_config_provider(ClickConfig **config, Window *window) {
	config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
}

void success(int32_t request_id, int http_status, DictionaryIterator* received, void* context) {
	// Check basic stuff
	if(request_id != 17) {
		text_layer_set_text(&textLayer, "Wrong request_id");
		return;
	}
	if(http_status != 200) {
		text_layer_set_text(&textLayer, "Unexpected http_status");
		return;
	}
	// Check the values.
	Tuple* tuple = dict_find(received, 1);
	if(!tuple) {
		text_layer_set_text(&textLayer, "Key 1 not found.");
		return;
	}
	if(tuple->length != 4 || tuple->type != TUPLE_INT) {
		text_layer_set_text(&textLayer, "Key 1 has wrong type.");
		return;
	}
	if(tuple->value->int32 != 42) {
		text_layer_set_text(&textLayer, "Key 1 has wrong value");
		return;
	}
	tuple = dict_find(received, 2);
	if(!tuple) {
		text_layer_set_text(&textLayer, "Key 2 not found.");
		return;
	}
	if(tuple->length != 1 || tuple->type != TUPLE_UINT) {
		text_layer_set_text(&textLayer, "Key 2 has wrong type.");
		return;
	}
	if(tuple->value->uint8 != 18) {
		text_layer_set_text(&textLayer, "Key 2 has wrong value.");
		return;
	}
	tuple = dict_find(received, 3);
	if(!tuple) {
		text_layer_set_text(&textLayer, "Key 3 not found.");
		return;
	}
	if(tuple->type != TUPLE_CSTRING) {
		text_layer_set_text(&textLayer, "Key 3 has wrong type.");
		return;
	}
	text_layer_set_text(&textLayer, tuple->value->cstring);
}

void failure(int32_t request_id, int http_status, void* context) {
	if(request_id == 0) {
		text_layer_set_text(&textLayer, "Request failed with zero request id");
	}
	else if(request_id == 17) {
		text_layer_set_text(&textLayer, "Request failed with expected request id");
	} else {
		text_layer_set_text(&textLayer, "Request failed with unexpected request id");
	}
}

// Standard app initialisation
void handle_init(AppContextRef ctx) {
	window_init(&window, "Button App");
	window_stack_push(&window, true /* Animated */);
	
	text_layer_init(&textLayer, window.layer.frame);
	text_layer_set_text(&textLayer, "Press select");
	text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));
	layer_add_child(&window.layer, &textLayer.layer);

	// Attach our desired button functionality
	window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
	
	// Set our app ID
	http_set_app_id(-1780059304);
	
	// Set up handlers.
	http_register_callbacks((HTTPCallbacks){
		.success = success,
		.failure = failure
	}, NULL);
}


void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.messaging_info = {
			.buffer_sizes = {
				.inbound = 256,
				.outbound = 256,
			}
		}
	};
	app_event_loop(params, &handlers);
}
