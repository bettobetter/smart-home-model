#pragma once
// ============================================================
// topics.h  -  "HOP DONG" CHUNG CUA NHOM (duoc commit len GitHub).
//
// Quy tac: KHONG ai tu doi file nay. Muon doi:
//   1. Bao ca nhom.
//   2. Sua docs/api-contract.md truoc.
//   3. Sau do moi sua file nay.
// Vi tri trong repo: firmware/common/topics.h
// ============================================================

// ---------- Tien to + ten phong ----------
#define TOPIC_PREFIX   "uetsh01"     // doi 1 cho nay de tranh trung tren broker cong cong

#define ROOM_KITCHEN   "kitchen"     // Phong 2 - Bep    (B)
#define ROOM_LIVING    "living"      // Phong 1 - Khach  (A, HUB)
#define ROOM_BEDROOM   "bedroom"     // Phong 3 - Ngu    (C)

// ---------- Ham tao topic (noi chuoi luc bien dich) ----------
// Vi du: TOPIC_SENSOR(ROOM_KITCHEN, "gas") -> "uetsh01/kitchen/sensor/gas"
#define TOPIC_SENSOR(room, name)  TOPIC_PREFIX "/" room "/sensor/" name
#define TOPIC_STATE(room, dev)    TOPIC_PREFIX "/" room "/state/"  dev
#define TOPIC_CMD(room, dev)      TOPIC_PREFIX "/" room "/cmd/"    dev
#define TOPIC_STATUS(room)        TOPIC_PREFIX "/" room "/status"

// ---------- Topic dung chung ----------
#define TOPIC_ALERT_GAS           TOPIC_PREFIX "/alert/gas"
#define TOPIC_ALERT_MOTION        TOPIC_PREFIX "/alert/motion"
#define TOPIC_CAMERA_DISCOVERY    TOPIC_PREFIX "/living/camera/discovery"

// ---------- Mau subscribe (danh cho hub) ----------
#define SUB_ALL_SENSOR            TOPIC_PREFIX "/+/sensor/+"
#define SUB_ALL_STATE             TOPIC_PREFIX "/+/state/+"
#define SUB_ALL_STATUS            TOPIC_PREFIX "/+/status"
#define SUB_ALL_ALERT             TOPIC_PREFIX "/alert/#"
// Node tu subscribe lenh cua rieng phong minh, vi du: TOPIC_PREFIX "/kitchen/cmd/#"

// ---------- Chuoi trang thai ----------
#define STATUS_ONLINE             "online"
#define STATUS_OFFLINE            "offline"   // dung lam Last Will

// ---------- Ten thiet bi / cam bien ----------
// sensor: temp, hum, light, gas, motion
// device: light, fan, door, buzzer
// (ghi o day de tranh go sai chinh ta)

// ============================================================
// NGUONG MAC DINH (cac gia tri khoi dau - hieu chinh khi test thuc te)
// ============================================================
#define SENSOR_INTERVAL_MS        3000    // chu ky gui cam bien (2000-5000)

// Quat phong ngu (Auto), co tre (hysteresis)
#define FAN_ON_C                  30.0f
#define FAN_OFF_C                 28.0f

// Den phong ngu (Auto). Quy uoc: light la phan tram 0-100, cang cao cang sang.
// Node tu quy doi tu ADC (neu mach LDR dao chieu thi tu dao o node).
#define LIGHT_ON_BELOW_PCT        30      // toi hon muc nay -> bat den
#define LIGHT_OFF_ABOVE_PCT       40      // sang hon muc nay -> tat den (phai > ON)

// Cua (hub)
#define DOOR_CLOSE_DELAY_MS       5000    // dong sau khi het chuyen dong
#define PIR_WARMUP_MS             60000   // bo qua PIR trong 60 giay dau sau khi cap nguon

// Gas (bep) - CAN HIEU CHINH THUC TE
#define GAS_WARMUP_MS             60000   // lam nong MQ-2, khong bao dong trong thoi gian nay
#define GAS_SAMPLES_TO_ALARM      3       // so mau lien tiep vuot nguong moi bao dong
#define GAS_MARGIN_RAW            400     // nguong = baseline luc khoi dong + gia tri nay (ADC 0-4095)

// Chat luong ket noi
#define MQTT_KEEPALIVE_S          30
#define MQTT_RECONNECT_MS         5000
