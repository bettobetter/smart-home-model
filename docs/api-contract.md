# API contract – MQTT và web hub

Dự án: Mô hình nhà thông minh ESP32-S3 · Phiên bản **1.0** · 12/10/2026

> **Quy tắc thay đổi:** không ai tự đổi tên topic hoặc định dạng dữ liệu. Muốn đổi thì báo cả nhóm, sửa file này trước, tăng số phiên bản, rồi mới sửa `firmware/common/topics.h` và code.

## 1. Quy ước chung

- **Tiền tố (`<prefix>`)**: `uetsh01`, khai báo ở `topics.h`. Đổi một chỗ là đổi cả hệ thống.
- **Tên phòng (`<room>`)**: `kitchen` (Bếp, B), `living` (Khách/Hub, A), `bedroom` (Ngủ, C).
- **Payload**: JSON một dòng, UTF-8, khóa viết thường. Riêng topic `status` là chuỗi thường.
- **Client ID** MQTT: `<prefix>-<room>` (ví dụ `uetsh01-kitchen`), mỗi board một ID duy nhất.
- **Không đặt** mật khẩu WiFi, địa chỉ hay tài khoản broker trong repo (xem `secrets.example.h`).
- Số thực dùng dấu chấm (`28.5`). Thời gian nếu có thì là `ts` = mili giây từ lúc board khởi động (không bắt buộc).

## 2. Bảng topic

| Topic | Hướng | QoS | Retain | Payload | Ghi chú |
|---|---|---|---|---|---|
| `<prefix>/<room>/sensor/<name>` | node → hub | 0 | Không | `{"v": 28.5}` | `<name>`: `temp`, `hum`, `light`, `gas`, `motion`. Gửi mỗi 2–5 giây; `motion` gửi ngay khi đổi |
| `<prefix>/<room>/state/<device>` | node → hub | 1 | **Có** | `{"on": true, "mode": "auto"}` | `<device>`: `light`, `fan`, `door`, `buzzer`. Gửi mỗi khi trạng thái đổi và ngay sau khi kết nối |
| `<prefix>/<room>/cmd/<device>` | hub → node | 1 | Không | `{"on": true}` hoặc `{"mode": "manual"}` | Có thể gửi cả hai khóa cùng lúc |
| `<prefix>/<room>/status` | node → hub | 1 | **Có** | `online` / `offline` | `offline` đặt làm Last Will; node gửi `online` ngay sau khi kết nối |
| `<prefix>/alert/gas` | bếp → hub | 1 | **Có** | `{"active": true, "v": 850}` | Gửi `active: true` khi báo động, gửi `active: false` khi đã được reset |
| `<prefix>/alert/motion` | hub nội bộ | 0 | Không | `{"detected": true}` | Sự kiện PIR để ghi log và chụp snapshot |
| `<prefix>/living/camera/discovery` | hub → web | 1 | **Có** | `{"ip": "192.168.1.50", "port": 81, "path": "/stream"}` | Hub gửi IP của mình sau khi kết nối WiFi |

**Hub subscribe:** `<prefix>/+/sensor/+`, `<prefix>/+/state/+`, `<prefix>/+/status`, `<prefix>/alert/#`.
**Mỗi node subscribe:** `<prefix>/<room của mình>/cmd/#`.

> **Lưu ý về QoS:** thư viện PubSubClient chỉ **publish** được QoS 0 (subscribe thì tới QoS 1). Nếu dùng PubSubClient, QoS 1 trong bảng chỉ là mức mong muốn, thực tế dựa vào retain và việc publish lại trạng thái khi đổi. Muốn QoS 1 thật khi publish thì dùng thư viện khác như arduino-mqtt. Cả nhóm phải dùng cùng một thư viện, chốt ở tuần 2.

## 3. Định dạng dữ liệu

### 3.1 Cảm biến (`sensor/<name>`)

| name | Phòng | `v` | Đơn vị / quy ước |
|---|---|---|---|
| `temp` | bedroom | số thực | °C |
| `hum` | bedroom | số thực | % độ ẩm tương đối |
| `light` | bedroom | số nguyên 0–100 | % độ sáng, **càng cao càng sáng**. Node tự quy đổi từ ADC (nếu mạch LDR đảo chiều thì đảo ở node) |
| `gas` | kitchen | số nguyên 0–4095 | Giá trị ADC thô, **không phải ppm** |
| `motion` | living | `0` hoặc `1` | `1` = có chuyển động |

### 3.2 Thiết bị (`state/<device>` và `cmd/<device>`)

| Phòng | device | `on` nghĩa là | Có `mode` | Nhận `cmd` |
|---|---|---|---|---|
| bedroom | `light` | đèn sáng | có | có |
| bedroom | `fan` | quạt chạy | có | có |
| kitchen | `fan` | quạt hút chạy | có | có |
| kitchen | `buzzer` | buzzer kêu | không | không (chỉ báo trạng thái) |
| living | `door` | cửa **mở** | có | có |

`mode` nhận `"auto"` hoặc `"manual"`. Ví dụ:

```
state: {"on": true, "mode": "auto"}
cmd bật đèn:        {"on": true}
cmd đổi chế độ:     {"mode": "manual"}
cmd cả hai:         {"mode": "manual", "on": false}
```

## 4. Quy tắc hành vi

1. **Khi kết nối MQTT**, node phải: đặt Last Will (`offline`) trước khi connect → publish `online` (retain) → publish `state` của mọi thiết bị (retain) → subscribe `cmd/#` của phòng mình.
2. **Chế độ Auto:** node bỏ qua lệnh `on` (chỉ nhận lệnh đổi `mode`). **Chế độ Manual:** node thực hiện lệnh `on`.
3. **Cảnh báo gas có ưu tiên cao nhất:** khi đang báo động, buzzer, LED đỏ và quạt hút luôn chạy bất kể `mode`, lệnh tắt từ web bị bỏ qua. Báo động chỉ dừng khi khí xuống dưới ngưỡng **và** có người nhấn nút reset.
4. **Mỗi phòng tự chạy logic Auto**, không phụ thuộc hub hay broker. Mất mạng thì phòng vẫn hoạt động.
5. **Khởi động:** PIR bỏ qua tín hiệu trong `PIR_WARMUP_MS`, MQ-2 không báo động trong `GAS_WARMUP_MS` (xem `topics.h`).
6. **Tùy chọn (mức khá, nhóm chốt trước khi làm):** khi hub nhận `alert/gas` với `active: true`, hub tự mở cửa chính.
7. **Mất kết nối:** node tự kết nối lại mỗi `MQTT_RECONNECT_MS`; hub hiển thị phòng `offline` khi nhận Last Will.

## 5. API web của hub (cho giao diện)

Giao diện (do B làm) chỉ dựa vào các điểm sau, nên có thể làm với dữ liệu giả trước khi hub chạy thật.

| Điểm truy cập | Mô tả |
|---|---|
| `GET /api/status` | Trả về toàn bộ trạng thái (ví dụ bên dưới) |
| `POST /api/cmd` | Gửi lệnh, body: `{"room": "bedroom", "device": "light", "on": true, "mode": "manual"}` (`on` và `mode` đều tùy chọn) |
| `WebSocket /ws` | Hub đẩy lại JSON giống `/api/status` mỗi khi có thay đổi |
| `http://<ip hub>:81/stream` | Stream camera MJPEG (cổng riêng để không làm nghẽn web điều khiển) |

Ví dụ `GET /api/status`:

```json
{
  "rooms": {
    "kitchen": {
      "online": true,
      "sensors": {"gas": 512},
      "devices": {"fan": {"on": false, "mode": "auto"}, "buzzer": {"on": false}}
    },
    "living": {
      "online": true,
      "sensors": {"motion": 0},
      "devices": {"door": {"on": false, "mode": "auto"}}
    },
    "bedroom": {
      "online": true,
      "sensors": {"temp": 28.5, "hum": 61, "light": 35},
      "devices": {"light": {"on": true, "mode": "auto"}, "fan": {"on": false, "mode": "auto"}}
    }
  },
  "camera": {"ip": "192.168.1.50", "port": 81, "path": "/stream"},
  "alert": {"gas": false}
}
```

## 6. Kiểm tra nhanh bằng MQTT Explorer

- Publish `{"v": 31.0}` vào `<prefix>/bedroom/sensor/temp` rồi xem giao diện có hiện 31 °C không.
- Publish `{"on": true}` vào `<prefix>/bedroom/cmd/fan` khi node ở chế độ Manual, node phải bật quạt và publish lại `state`.
- Tắt nguồn một node, topic `<prefix>/<room>/status` phải chuyển thành `offline`.

## 7. Lịch sử thay đổi

| Phiên bản | Ngày | Nội dung |
|---|---|---|
| 1.0 | 12/10/2026 | Bản đầu tiên |
