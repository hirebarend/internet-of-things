#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const String deviceId = "383";

const char *ssid = "WR7010-2.4G-82E";
const char *password = "12345678";

// const char *ssid = "Barend";
// const char *password = "0766542813";

unsigned int count = 0;
unsigned int interval = 60000;
unsigned long lastMillis = 0;
unsigned int sum = 0;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  while(!Serial)
  {
    delay(1000);
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }

  Serial.println(WiFi.localIP());

  lastMillis = millis();
}

void loop()
{  
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
  }
  
  if (millis() - lastMillis >= interval) {
    unsigned long elapsedMillis = millis() - lastMillis;
    
    lastMillis = millis();

    unsigned int averageAnalogValue = sum / count;

    count = 0;
    sum = 0;

    sendMetric(deviceId + "_" + "analogValue", averageAnalogValue);

    Serial.println(averageAnalogValue);
  }
  
  count += 1;
  sum += getAnalogValue();

  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}

unsigned int getAnalogValue() {
  int max = 0;
  
  for (int i = 0 ; i <= 200 ; i++) 
  {
    int value = analogRead(A0);  
     
    if(value >= max){
      max = value;
    }
  }

  return max;
}

void sendMetric(String name, unsigned int value)
{
  String host = "https://function-app-982.azurewebsites.net/api/metric?name=" + name + "&value=" + value;
  
  WiFiClientSecure wiFiClientSecure;

  wiFiClientSecure.setInsecure();
  
  wiFiClientSecure.connect(host, 443);

  HTTPClient httpClient;

  httpClient.begin(wiFiClientSecure, host);
  
  if (httpClient.GET() != HTTP_CODE_OK)    
  {
    Serial.println("Failed to send metric");

    return;
  }

  Serial.println(name + " " + value);

  httpClient.end();
  wiFiClientSecure.stop();
}
