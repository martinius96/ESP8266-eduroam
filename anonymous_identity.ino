#include <ESP8266WiFi.h>

extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
}

static const char* ssid = "eduroam";
static const char* identity = "anonymous@example.com";
static const char* username = "id@example.com";
static const char* password = "password";
char buff[20];
String ip;

uint8_t mac[6] {0x30, 0xAE, 0xA4, 0x0F, 0x89, 0x78};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Free memory before setting wifi connection");
  Serial.println(system_get_free_heap_size());
  delay(500);
  Serial.print("OLD ESP8266 MAC: ");
  Serial.println(WiFi.macAddress()); //This will read MAC Address of ESP

  wifi_set_macaddr(0, const_cast<uint8*>(mac));   //This line changes MAC adderss of ESP8266

  Serial.print("NEW ESP8266 MAC: ");
  Serial.println(WiFi.macAddress()); //This will read MAC Address of ESP
  // WPA2 Connection starts here
  // Setting ESP into STATION mode only (no AP mode or dual mode)
  wifi_set_opmode(STATION_MODE);
  struct station_config wifi_config;
  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char*)wifi_config.ssid, ssid);
  wifi_station_set_config(&wifi_config);
  wifi_station_clear_cert_key();
  wifi_station_clear_enterprise_ca_cert();
  wifi_station_set_wpa2_enterprise_auth(1);
  wifi_station_set_enterprise_identity((uint8*)identity, strlen(identity));
  wifi_station_set_enterprise_username((uint8*)username, strlen(username));
  wifi_station_set_enterprise_password((uint8*)password, strlen(password));
  wifi_station_connect();
  // WPA2 Connection ends here

  // Wait for connection AND IP address from DHCP
  Serial.println();
  Serial.println("Waiting for connection and IP Address from DHCP");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Free memory during connecting to wifi");
    Serial.println(system_get_free_heap_size());
    delay(2000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Free memory after connection sucessful");
  Serial.println(system_get_free_heap_size());
}

void loop() {
}
