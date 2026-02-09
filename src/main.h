#ifndef MAIN_H
#define MAIN_H

#include "lvgl.h"
#include "BomParse.h"
#include "BomUI.h"

#define SPIFFS_FILE_PATH "/test.xml"

bool connect_wifi();
lv_obj_t* lv_show_loading();
void lv_show_bom_results(BomParseResult& result);
void lv_show_bom_results_table(BomParseResult& result, BomUI& ui);

#endif