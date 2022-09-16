#include "c_vector/vector/vector.h"
#include "log/log.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <stdbool.h>
#include <stdio.h>
#define PLIST_INFO_ICNS_PROPERTY_NAME    "CFBundleIconFile"

static char *extract_xml_text_property(char *xml_file_name, char *xml_text_property_name){
  bool             in_requested_prop = false;
  xmlTextReaderPtr reader            = xmlReaderForFile(xml_file_name, NULL, 0);

  if (reader != NULL) {
    int ret = xmlTextReaderRead(reader);
    while (ret == 1) {
      xmlChar *value;
      value = xmlTextReaderConstValue(reader);
      if (value != NULL && strlen(value) > 4) {
        if (in_requested_prop == true) {
          in_requested_prop = false;
          return(value);
        }else if (strcmp(xml_text_property_name, value) == 0) {
          in_requested_prop = true;
        }
      }
      ret = xmlTextReaderRead(reader);
    }
    xmlFreeTextReader(reader);
  }
  return(NULL);
}

char *get_app_list_icon_file_name(char *xml_file_path){
  char *ret                  = NULL;
  char *extracted_text_value = extract_xml_text_property(xml_file_path, PLIST_INFO_ICNS_PROPERTY_NAME);

  asprintf(&ret, "%s", extracted_text_value);
  xmlCleanupParser();
  return(ret);
}
