/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include "Credentials.h"
#include "main.h"

#include <Arduino.h>
#include <SPIFFS.h>

#include "lvgl.h"
#include "app_hal.h"

#include "WiFi.h"
#include "FTPClient.h"
#include "TinyXML.h"

void display_file_system_information() {
  Serial.println("File system:");
  if (SPIFFS.begin()) {
// Display files
#ifdef ARDUINO_ARCH_ESP32
      File root_directory = SPIFFS.open("/");
      File file = root_directory.openNextFile();
      while (file) {
        Serial.printf("%s (%d bytes)\n", file.path(), file.size());
        file = root_directory.openNextFile();
      }
#endif

    // Get SPIFFS informations
    size_t totalKBytes=0;
    size_t usedKBytes=0;
#ifdef ARDUINO_ARCH_ESP32
      totalKBytes=SPIFFS.totalBytes()>>10;
      usedKBytes=SPIFFS.usedBytes()>>10;
#endif

    // Display space used
    Serial.printf("Total: %d KBytes\n",totalKBytes);
    Serial.printf("Used: %d KBytes\n",usedKBytes);
    Serial.printf("Free: %d KBytes\n",totalKBytes-usedKBytes);
  }
}

bool connect_wifi() {
  int wifi_connect_count = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wifi hotspot...");
  while (WiFi.status()!=WL_CONNECTED) { 
    Serial.print("o");
    delay(500); 
    wifi_connect_count++;
    if (wifi_connect_count > 20) {
      Serial.println("Failed to connect to Wifi hotspot...");
      return false;
    }
  }

  // Debug trace
  Serial.println("\nConnected");
  //WiFi.disconnect();
  return true;
}



void show_ftp_results_simple() {
  Serial.println("FTP test result:");
  fs::File spiffs_file = SPIFFS.open(SPIFFS_FILE_PATH, "r");
  if (spiffs_file) {
    Serial.println("File content:");
    while (spiffs_file.available()) {
      Serial.write(spiffs_file.read());
    }
    spiffs_file.close();
  } else {
    Serial.println("Failed to open file in SPIFFS");
  }
}

void fetchXMLFromBomFTP() {
  BomParseResult result;
  BomParse bom(result);

  bom.fetchXML();
}

void parse_ftp_results(BomParseResult& result) {
  BomParse bom(result);
  bom.parseXML();

  Serial.printf("Region: %s\n", result.regionName);
  for (int i = 0; i < result.periodCount; i++) {
    Serial.printf("Period %d: Min=%d, Max=%d, Precis=%s, Precip Probability=%s, Precip Range=%s\n",
                  i,
                  result.periods[i].tempMin,
                  result.periods[i].tempMax,
                  result.periods[i].precis,
                  result.periods[i].precipProbability,
                  result.periods[i].precipRange);
  }
}


void setup() {
  Serial.begin(115200);
  lv_init();
  hal_setup();
  lv_obj_t *loading_label = lv_show_loading();
  hal_loop();

  display_file_system_information();
  if (!connect_wifi()) {
    Serial.println("WiFi not connected, cannot start test");
    return;
  }
  Serial.printf("Local IP: %s\n", WiFi.localIP().toString().c_str());
  
  BomParseResult result;
  //fetchXMLFromBomFTP();
  parse_ftp_results(result);

  //lv_demo_widgets();
  // button_demo(result.regionName);
  // lv_show_bom_results(result);
  lv_obj_delete(loading_label);
  lv_show_bom_results_table(result);
}

void loop() {
  hal_loop();  /* Do not use while loop in this function */
}

lv_obj_t *lv_show_loading() {
  lv_obj_t * label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "Loading...");
  lv_obj_center(label);
  return label;
}

#if 0
static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target_obj(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        const char * user_data = (const char *)lv_event_get_user_data(e);
        lv_label_set_text_fmt(label, "%s: %d,%d", user_data, wifi_connect_count, cnt);
        Serial.printf("Button clicked: %s %d, user_data: %s\n", user_data, cnt, user_data);
        //connect_wifi();
    }
}

/**
 * Create a button with a label and react on click event.
 */
void lv_example_get_started_2(const char *labelStr)
{
    lv_obj_t * btn = lv_button_create(lv_screen_active());     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, (void *)labelStr);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, labelStr);                     /*Set the labels text*/
    lv_obj_center(label);
}

void button_demo(char *regionName) {
  lv_example_get_started_2(regionName);
}

static lv_obj_t* list1;

static void lv_list_event_handler(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* obj = lv_event_get_target_obj(e);
  LV_UNUSED(obj);
  if (code == LV_EVENT_CLICKED) {
    LV_LOG_USER("Clicked: %s", lv_list_get_button_text(list1, obj));
  }
}
#endif

void lv_show_bom_results(BomParseResult& result ) {
  /*Create a list*/
  lv_obj_t* bomResultsList = lv_list_create(lv_screen_active());
  lv_obj_set_size(bomResultsList, 320, 220);
  lv_obj_center(bomResultsList);
  char label[48];

  lv_obj_t* btn;

  lv_list_add_text(bomResultsList, result.regionName);
  for (int i = 0; i < result.periodCount; i++) {
    snprintf(label, sizeof(label) - 1, "%d:%s: %d->%d :%s of %s", 
      i, result.periods[i].precis, result.periods[i].tempMin, result.periods[i].tempMax, result.periods[i].precipProbability, result.periods[i].precipRange);
    lv_list_add_text(bomResultsList, label);
  }
}

void lv_show_bom_results_table(BomParseResult& result) {
  int periodCount = result.periodCount;
  lv_obj_t* table = lv_table_create(lv_screen_active());

  /*Set a smaller height to the table. It'll make it scrollable*/
  // lv_obj_set_size(table, LV_SIZE_CONTENT, 200);
  lv_obj_set_size(table, 310, 230);

  lv_table_set_column_width(table, 0, 150);
  lv_table_set_row_count(table, result.periodCount);
  lv_table_set_column_count(table, 6);

  lv_table_set_column_width(table, 0, 20);
  lv_table_set_column_width(table, 1, 80);
  lv_table_set_column_width(table, 2, 30);
  lv_table_set_column_width(table, 3, 30);
  lv_table_set_column_width(table, 4, 40);
  lv_table_set_column_width(table, 5, 90);

  lv_obj_set_style_pad_ver(table, 1, LV_PART_ITEMS);
  lv_obj_set_style_pad_hor(table, 3, LV_PART_ITEMS);
  /*Don't make the cell pressed, we will draw something different in the event*/
  lv_obj_remove_style(table, NULL, LV_PART_ITEMS | LV_STATE_PRESSED);

  uint32_t i;
  for (i = 0; i < periodCount; i++) {
    lv_table_set_cell_value_fmt(table, i, 0, "%d", i + 1);
    lv_table_set_cell_value_fmt(table, i, 1, "%s", result.periods[i].precis);
    lv_table_set_cell_value_fmt(table, i, 2, "%d", result.periods[i].tempMin);
    lv_table_set_cell_value_fmt(table, i, 3, "%d", result.periods[i].tempMax);
    lv_table_set_cell_value_fmt(table, i, 4, "%s", result.periods[i].precipProbability);
    lv_table_set_cell_value_fmt(table, i, 5, "%s", result.periods[i].precipRange);
  }

  lv_obj_align(table, LV_ALIGN_CENTER, 0, 0);
}
