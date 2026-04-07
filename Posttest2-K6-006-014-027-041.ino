#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   
#include <DHT.h>

const char* ssid = "sotobanjar";
const char* password = "12344321";

// ================= TELEGRAM =================
#define BOT_TOKEN "8593382970:AAEX9i4hvwTnRBQWdzzc1vSDy15dYMxVAV4"
#define CHAT_ID_A "1882245704"
#define CHAT_ID_B "1263015746"
#define CHAT_ID_C "6185771640"
#define CHAT_ID_D "5522858176"
#define GROUP_ID  "-5011737545"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// ================= PIN =================
#define LED_A 5
#define LED_B 6
#define LED_C 7
#define LED_D 8
#define LED_UTAMA 9

#define MQ2_PIN 0
#define DHTPIN 1
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// ================= VAR =================
unsigned long lastTime = 0;
int gasThreshold = 1600;
bool gasAlertSent = false;

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);
  pinMode(LED_D, OUTPUT);
  pinMode(LED_UTAMA, OUTPUT);

  WiFi.begin(ssid, password);
  client.setInsecure();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi Connected!");
  dht.begin();
}

// ================= CEK AKSES =================
bool isAuthorized(String chat_id, int ledPin) {
  return (chat_id == CHAT_ID_A && ledPin == LED_A) ||
         (chat_id == CHAT_ID_B && ledPin == LED_B) ||
         (chat_id == CHAT_ID_C && ledPin == LED_C) ||
         (chat_id == CHAT_ID_D && ledPin == LED_D);
}

// ================= HANDLE COMMAND =================
void handleMessage(int i) {
  String chat_id = bot.messages[i].from_id;
  String text = bot.messages[i].text;

  // ===== LED PRIBADI =====
  if (text == "/led_on") {
    if (isAuthorized(chat_id, LED_A)) {
      digitalWrite(LED_A, HIGH);
      bot.sendMessage(GROUP_ID, "Zhorif Menyalakan LED", "");
      return;
    }
    else if (isAuthorized(chat_id, LED_B)) {
      digitalWrite(LED_B, HIGH);
      bot.sendMessage(GROUP_ID, "Ammar Menyalakan LED", "");
      return;
    }
    else if (isAuthorized(chat_id, LED_C)) {
      digitalWrite(LED_C, HIGH);
      bot.sendMessage(GROUP_ID, "Fajar Menyalakan LED", "");
      return;
    }
    else if (isAuthorized(chat_id, LED_D)) {
      digitalWrite(LED_D, HIGH);
      bot.sendMessage(GROUP_ID, "Ghazali Menyalakan LED", "");
      return;
    }
    else {
      bot.sendMessage(GROUP_ID, "Tidak punya akses!", "");
      return;
    }
    bot.sendMessage(chat_id, "LED pribadi ON", "");
  }

  if (text == "/start") {
  String pesan = R"(
Selamat Datang

Untuk menggunakan bot ini, silahkan gunakan perintah berikut;
1. /led_on - menyalakan lampu pribadi
2. /led_off - mematikan lampu pribadi
3. /ruang_on - menyalakan lampu ruang tamu
4. /ruang_off - mematikan lampu ruang tamu
5. /cek_suhu - memeriksa kondisi suhu dan kelembapan saat ini
6. /start - melihat pesan ini
)";
    bot.sendMessage(GROUP_ID, pesan, "");
  }

  if (text == "/led_off") {
    if (isAuthorized(chat_id, LED_A)) {
      digitalWrite(LED_A, LOW);
      bot.sendMessage(GROUP_ID, "Zhorif Mematikan LED", "");
      return;
    }
    else if (isAuthorized(chat_id, LED_B)) {
      digitalWrite(LED_B, LOW);
      bot.sendMessage(GROUP_ID, "Ammar Mematikan LED", "");
      return;
    }
    else if (isAuthorized(chat_id, LED_C)) {
      digitalWrite(LED_C, LOW);
      bot.sendMessage(GROUP_ID, "Fajar Mematikan LED", "");
      return;
    }
    else if (isAuthorized(chat_id, LED_D)) {
      digitalWrite(LED_D, LOW);
      bot.sendMessage(GROUP_ID, "Ghazali Mematikan LED", "");
      return;
    }
    else {
      bot.sendMessage(GROUP_ID, "Tidak punya akses!", "");
      return;
    }
    bot.sendMessage(chat_id, "LED pribadi OFF", "");
  }

  // ===== LED UTAMA =====
  if (text == "/ruang_on") {
    digitalWrite(LED_UTAMA, HIGH);
    bot.sendMessage(GROUP_ID, "Lampu ruang tamu ON", "");
  }

  if (text == "/ruang_off") {
    digitalWrite(LED_UTAMA, LOW);
    bot.sendMessage(GROUP_ID, "Lampu ruang tamu OFF", "");
  }

  // ===== CEK SUHU =====
  if (text == "/cek_suhu") {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    String msg = "Suhu: " + String(t) + "°C\n";
    msg += "Kelembapan: " + String(h) + "%";

    bot.sendMessage(GROUP_ID, msg, "");
  }
}

// ================= LOOP =================
void loop() {

  // ===== TELEGRAM CHECK =====
  if (millis() - lastTime > 500) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      for (int i = 0; i < numNewMessages; i++) {
        handleMessage(i);
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTime = millis();
  }

  // ===== MQ-2 GAS CHECK =====
  int gasValue = analogRead(MQ2_PIN);

  if (gasValue > gasThreshold && !gasAlertSent) {
    bot.sendMessage(GROUP_ID, "⚠️ GAS TERDETEKSI! Segera cek ruangan!", "");
    gasAlertSent = true;
  }

  if (gasValue < gasThreshold) {
    gasAlertSent = false;
  }
  Serial.print("Tekanan gas saat ini: ");
  Serial.println(gasValue);
}