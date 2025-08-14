#include <WiFi.h>
#include <HTTPClient.h>
#include <SoftwareSerial.h>

const char* ssid = "TEKNIK";
const char* pass = "perencanaan";
SoftwareSerial DataSerial(16, 17);
String Web_App_URL = "https://script.google.com/macros/s/yourwebappurl";

String Status_Read_Sensor = "";
float pressure;

unsigned long previousMillis = 0;
const long interval = 1000;

String arrData[1];
void setup() {
  Serial.begin(9600);
  DataSerial.begin(9600);
  WiFi.mode(WIFI_STA);
  Serial.println("");
  Serial.println("Connecting to ");
  Serial.println(ssid);
  Serial.println(WiFi.localIP());
  WiFi.begin(ssid, pass);
  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
  Serial.print(".");
  if (connecting_process_timed_out > 0) connecting_process_timed_out--;
  if (connecting_process_timed_out == 0) {
      delay(2000);
      WiFi.reconnect();

    }
  };
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("------------");
  delay(2000);
}

void loop(){
  Getting_Pressure_Sensor_Data();
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval)
  {
  previousMillis = currentMillis;
  String data = "";
  while (DataSerial.available()>0)
  {  
      data += char(DataSerial.read());
    }
  data.trim();
    {
    int index = 0;
    for (int i=0; i<=data.length(); i++)
    {
      char delimiter = '|' ;
      if (data[i] != delimiter)
        arrData[index] += data[i];
      else
        index++;      
    }
      if(index == 1)
    {
      Serial.println("Pressure : " + arrData[0]);
    }
    pressure = arrData[0].toFloat();
    arrData[0] =  "";
     // Create a URL for sending or writing data to Google Sheets.
    String Send_Data_URL = Web_App_URL + "?sts=write";
    Send_Data_URL += "&srs=" + Status_Read_Sensor;
    Send_Data_URL += "&pressure=" + String(pressure);
    Serial.println();
    Serial.println("-------------");
    Serial.println("Send data to Google Spreadsheet...");
    Serial.print("URL : ");
    Serial.println(Send_Data_URL);

    //::::::::::::::::::The process of sending or writing data to Google Sheets.
      // Initialize HTTPClient as "http".
      HTTPClient http;
  
      // HTTP GET Request.
      http.begin(Send_Data_URL.c_str());
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  
      // Gets the HTTP status code.
      int httpCode = http.GET(); 
      Serial.print("HTTP Status Code : ");
      Serial.println(httpCode);
  
      // Getting response from google sheets.
      String payload;
      if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload : " + payload);    
      }
      
      http.end();
    //::::::::::::::::::
    
    Serial.println("-------------");
    delay(1000);
    }
    }
    }
  void Getting_Pressure_Sensor_Data() {
  // Reading Pressure takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

  // Check if any reads failed and exit early (to try again).
  if (isnan(pressure)) {
    Serial.println();
    Serial.println(F("Failed to read from Pressure sensor!"));
    Serial.println();
    Status_Read_Sensor = "Failed";
    pressure = 0.00;
  } else {
    Status_Read_Sensor = "Success";
  }
  }
