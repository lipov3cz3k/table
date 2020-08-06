#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <pins.h>
// #include <WiFiClient.h>
// #include <ESP8266WiFiMulti.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

const char* ssid     = "ESP8266-Access-Point";
const char* password = "123456789";

// ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

int btn_pins[] = {BTN_UP, BTN_DOWN};
int assert_pins[] = {ASSERT_UP, ASSERT_DOWN};

void handleRoot();              // function prototypes for HTTP handlers
void handleNotFound();
void handleTable();
void moveTableUp(int press_second);
void moveTableDown(int press_second);
void stopTable();

void setup(void){
  Serial.begin(9600);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DOWN, INPUT);
  pinMode(ASSERT_UP, OUTPUT);
  pinMode(ASSERT_DOWN, OUTPUT);

  // digitalWrite(BTN_UP, LOW);
  // digitalWrite(BTN_DOWN, LOW);

  // SET AP
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);   // Remove the password parameter, if you want the AP (Access Point) to be open
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.println(WiFi.localIP());

  // Connect to wifi
  // wifiMulti.addAP("sGuest", "XXXX");   // add Wi-Fi networks you want to connect to
  // Serial.println("Connecting ...");
  // while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
  //   delay(250);
  //   Serial.print('.');
  // }
  // Serial.println('\n');
  // Serial.print("Connected to ");
  // Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  // Serial.print("IP address:\t");
  // Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/table", HTTP_GET, handleTable);
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();                    // Listen for HTTP requests from clients
  for(uint8_t i=0; i < ARRAY_SIZE(btn_pins); ++i) {
    digitalWrite(assert_pins[i], digitalRead(btn_pins[i]));
  }
}

void handleRoot() {
  server.send(200, "text/html",
  "<!DOCTYPE html><html>"
  "<head><style>"
  " html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
  " .button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;"
  " text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}"
  "</style></head>"
  "<body>"
  "<h1>ONovy controller</h1>"
  "<button type=\"button\" class=\"button\" onclick=\"loadDoc('up')\">UP</button>"
  "<button type=\"button\" class=\"button\" onclick=\"loadDoc('down')\">DOWN</button>"
  "<script>"
  "  function loadDoc(direction) {"
  "    var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\", \"table?direction=\" + direction, true); xhttp.send(); }"
  "</script>"
  "</body></html>");
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void handleTable() {
  String direction = server.arg("direction");
  int step = server.arg("step").toInt();
  if(step == 0){
    step = 1;
  }

  if( direction == "up" ){
    moveTableUp(step);
    server.send(200);
  }else if( direction == "down" ){
    moveTableDown(step);
    server.send(200);
  }else{
    server.send(404);
  }
}

void moveTableUp(int press_second = 1)
{
  Serial.println("Table up");
  digitalWrite(ASSERT_UP, HIGH);
  digitalWrite(ASSERT_DOWN, LOW);
  delay(press_second*1000);
  digitalWrite(ASSERT_UP, LOW);
  Serial.println("Table stop");
}

void moveTableDown(int press_second = 1)
{
  Serial.println("Table down");
  digitalWrite(ASSERT_UP, LOW);
  digitalWrite(ASSERT_DOWN, HIGH);
  delay(press_second*1000);
  digitalWrite(ASSERT_DOWN, LOW);
  Serial.println("Table stop");
}

void stopTable()
{
  Serial.println("Table stop");
  digitalWrite(ASSERT_DOWN, LOW);
  digitalWrite(ASSERT_UP, LOW);
}