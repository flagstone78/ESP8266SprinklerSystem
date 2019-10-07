

int timeZone = -6;
const char* host = "calendar.google.com";
String url = "/calendar/ical/j3tsjdb2qe30of3d34uk9l37u4%40group.calendar.google.com/private-6d4f60e4cfa3f8a168b1e08431897c23/basic.ics";

const int httpsPort = 443;
//   Get thumbprint by inspecting calendar.google.com certificate in web browser
//const char fingerprint[] PROGMEM = "c6:69:4a:9d:71:f5:ea:a5:ee:3d:63:ee:9f:bc:c6:27:f7:c3:a8:a6";
//const char fingerprint[] PROGMEM = "75:e0:ab:b6:13:85:12:27:1c:04:f8:5f:dd:de:38:e4:b7:24:2e:fe";
extern const unsigned char caCert[] PROGMEM;
extern const unsigned int caCertLen;

WiFiClientSecure client;
String httpReply;

void DownloadAndParseCalendar(){
  Serial.print("connecting to ");
  Serial.println(host);

  //Serial.printf("Using fingerprint '%s'\n", fingerprint);
  //client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  
  if (client.connected()) {
    httpReply = client.readStringUntil('\n');
    if (httpReply == "\r") {
      Serial.println("headers received");
    }
    //first response
    //httpReply = client.readStringUntil(0);
  }
  
  Serial.print("first reply was:");
  //Serial.println(httpReply);

  while (client.connected() || client.available())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
        Calendar.LoadFromFile(line.c_str());
      }
    }

  // Close the connection
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}

//Wifi setup
WiFiManager wifiManager; //wifi manager creates network for first time setup

void setUpWifiConnection(){
  //wifiManager.resetSettings();//reset wifi settings
  wifiManager.autoConnect("WifiSprinklerSystem"); //Tries to connect to saved networks, if fails it sets up a network

  // Load root certificate in DER format into WiFiClientSecure object
  bool res = client.setCACert_P(caCert, caCertLen);
  if (!res) {
    Serial.println("Failed to load root CA certificate!");
    while (true) {
      yield();
    }
  }
}

time_t now;
void updateGlobalTimeFromInternet(){
  //get time from internet
  Serial.print("Setting time using SNTP");
  configTime(-7 * 3600, 3600, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  
  Serial.println("");
  //print time
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}
