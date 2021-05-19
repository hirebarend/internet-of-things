#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "SSID";
const char *password = "PASSWORD";

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
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Not connected to WiFi");

    delay(3000);

    return;
  }

  // Wait for 25 seconds
  for (int i = 0; i < 25 * 2; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
  }

  double analogValue = getAnalogValue();

  sendMetric("analogValue", analogValue);
}

double getAnalogValue() {
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
