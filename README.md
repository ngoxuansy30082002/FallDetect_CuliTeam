# Fall Detection System

## Mô tả dự án
Hệ thống phát hiện té ngã sử dụng ESP32 với hai module chính: 

- **Fall_Detect**: Source code chính thực hiện việc phát hiện té ngã thông qua cảm biến, xử lý tín hiệu và gửi thông báo cảnh báo qua giao thức MQTT.
- **MQTT_Alarm**: Source code nhận tín hiệu từ MQTT broker, kích hoạt cảnh báo bằng còi và đèn LED.

### Công nghệ sử dụng
- **Vi điều khiển ESP32**: Với khả năng kết nối Wi-Fi, ESP32 đảm bảo xử lý và giao tiếp nhanh chóng.
- **Giao thức MQTT**: Được sử dụng để truyền tín hiệu giữa các module thông qua broker.
- **EMQX Broker**: Broker MQTT mã nguồn mở, hỗ trợ mã hóa MQTTS.
- **Ngôn ngữ lập trình**: C++ (Arduino Framework).

### Chức năng chính
1. **Fall_Detect**:
   - Xử lý dữ liệu từ cảm biến chuyển động để phát hiện té ngã.
   - Gửi tín hiệu cảnh báo đến MQTT broker.

2. **MQTT_Alarm**:
   - Lắng nghe tín hiệu từ MQTT broker.
   - Kích hoạt còi và đèn LED để cảnh báo.

---

## Cấu trúc thư mục

- `Fall_Detect/`: Source code xử lý phát hiện té ngã và gửi tín hiệu qua MQTT.
- `MQTT_Alarm/`: Source code nhận tín hiệu và điều khiển còi, đèn LED.

---

## Thành viên nhóm

| Họ và tên              | Lớp       |
|------------------------|-----------|
| Nguyễn Việt Hùng       | 20KTMT1  |
| Nguyễn Huy Du          | 20KTMT2  |
| Hoàng Khắc Trung Hiếu  | 21KTMT   |
| Ngô Xuân Sỹ            | 20KTMT1  |
| Nguyễn Đức Hoàng       | 20KTMT2  |

---

