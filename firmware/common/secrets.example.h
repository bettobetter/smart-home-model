#pragma once
// ============================================================
// secrets.example.h  -  FILE MAU, duoc phep commit len GitHub.
//
// Cach dung:
//   1. Copy file nay thanh secrets.h (cung thu muc).
//   2. Dien thong tin that vao secrets.h.
//   3. secrets.h da nam trong .gitignore -> KHONG bao gio commit.
// ============================================================

// --- WiFi (moi nguoi dung WiFi cua minh khi lam o nha) ---
#define WIFI_SSID      "TEN_WIFI"
#define WIFI_PASSWORD  "MAT_KHAU_WIFI"

// --- MQTT broker ---
// Khi lam tu xa: dung broker chung cua nhom (vi du HiveMQ Cloud / EMQX).
// Khi demo: doi sang Mosquitto tren laptop hoac hotspot dien thoai.
#define MQTT_HOST      "broker.example.com"
#define MQTT_PORT      1883          // 1883: khong ma hoa. Broker cloud thuong dung 8883 (TLS)
#define MQTT_USER      ""            // de trong neu broker khong yeu cau
#define MQTT_PASS      ""
