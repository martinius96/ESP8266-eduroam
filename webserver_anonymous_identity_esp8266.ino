// MY CONNECTION TO 802.1x NETWORK IS BASED
// ON MY MAC ADDRESS SO I NEEDED TO UPDATE IT MANUALLY
#include <ESP8266WiFi.h>
WiFiServer server(80);
String header;
extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
}

static const char* ssid = "eduroam";
static const char* identity = "anonymous@example.com";
static const char* username = "id@example.com";
static const char* password = "pass";
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
  server.begin();
}

void loop(){
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();           
        header += c;
        if (c == '\n') { 
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println("<head><meta charset='UTF-8'><meta name='author' content='Martin Chlebovec'><meta name='viewport' content='width=device-width, initial-scale=1.0'></head><style>body{  display: table;width: 100%;background: #dedede;text-align: center;}*{ -webkit-box-sizing: border-box; /* Safari/Chrome, other WebKit */-moz-box-sizing: border-box;    /* Firefox, other Gecko */box-sizing: border-box;         /* Opera/IE 8+ */}.aa_h2{  font:100 5rem/1 Roboto;  text-transform: uppercase;}table{   background: #fff;}table,thead,tbody,tfoot,tr, td,th{  text-align: center;  margin: auto;  border:1px solid #dedede;  padding: 1rem;  width: 50%;}.table    { display: table; width: 50%; }.tr       { display: table-row;  }.thead    { display: table-header-group }.tbody    { display: table-row-group }.tfoot    { display: table-footer-group }.col      { display: table-column }.colgroup { display: table-column-group }.td, .th   { display: table-cell; width: 50%; }.caption  { display: table-caption }.table,.thead,.tbody,.tfoot,.tr,.td,.th{ text-align: center; margin: auto; padding: 1rem;}.table{  background: #fff;  margin: auto;  border:none;  padding: 0; margin-bottom: 5rem;}.th{font-weight: 700;border:1px solid #dedede;&:nth-child(odd){border-right:none;}}.td{font-weight: 300;border:1px solid #dedede;border-top:none;&:nth-child(odd){border-right:none;}}.aa_htmlTable{background: tomato;padding: 5rem;display: table;width: 100%;height: 100vh;vertical-align: middle;}.aa_css{background: skyblue;padding: 5rem;display: table;width: 100%;height: 100vh;vertical-align: middle;}.aa_ahmadawais{display: table;width: 100%;font: 100 1.2rem/2 Roboto;margin: 5rem auto;}</style><div class='aa_htmlTable'><table width=100%><thead><tr><th>Teplota</th><th>Hodnota</th></tr></thead><tbody><tr><td>Dnu</td><td> °C</td></tr><tr><td>Von</td><td> °C</td></tr></tbody></table></div>");
            client.println();
            break;
          }else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
          }else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
          }
        }
      }
  header = "";
  client.stop();
  Serial.println("");
  }
}
