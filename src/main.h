#include "lvgl.h"
#include "BomParse.h"

#define SPIFFS_FILE_PATH "/test.xml"

bool connect_wifi();
lv_obj_t* lv_show_loading();
void lv_show_bom_results(BomParseResult& result);
void lv_show_bom_results_table(BomParseResult& result);
