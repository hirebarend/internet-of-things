#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "Barend";
const char *password = "0766542813";

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }

  Serial.println(WiFi.localIP());

  if (!sensor.begin())
  {
    Serial.println("Failed to connect to sensor");
  }
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Not connected to WiFi");

    delay(3000);

    return;
  }

  // Wait for 50 seconds
  for (int i = 0; i < 50 * 2; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
  }

  double milliVolts = getMilliVolts();

  sendMetric("milliVolts", milliVolts);
}

double getMilliVolts() {
  int max = 0;
  
  for (int i = 0 ; i <= 200 ; i++) 
  {
    int value = analogRead(A1);  
     
    if(value >= max){
      max = value;
    }
  }
  
  double milliVolts = (max - 512) * (2500 / 512);

  return milliVolts;
}

void sendMetric(String name, double value)
{
  String host = "https://function-app-977.azurewebsites.net/api/internet-of-things?name=" + name + "&value=" + value;
  
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
}
