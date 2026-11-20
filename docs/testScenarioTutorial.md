# Kịch Bản Test Tay Cho Bộ 10 File CSV Khắc Nghiệt (RoutingSimulation)

Giải nén toàn bộ các file `.csv` vào thư mục `./data/` của dự án, sau đó chạy chương trình chính `RoutingSimulation` và dùng **Option 6 (Import network topology from CSV)** để nạp từng file theo các kịch bản dưới đây.

---

## 1. `01_mesh_50_routers.csv` — Lưới 50 Router & Đường hầm tắt giá cao
- **Cấu trúc:** Lưới 5×10 gồm 50 router (`10.0.0.0` đến `10.0.4.9`), mỗi cạnh lưới `cost = 10`, cộng thêm 1 đường tắt trực tiếp `10.0.0.0 <-> 10.0.4.9` (`express0`, `cost = 9999`).
- **Các bước test:**
  1. Chọn `6` -> nhập `01_mesh_50_routers.csv`.
  2. Chọn `9` -> nhập `dijkstra`.
  3. Chọn `2` -> gửi từ `10.0.0.0` đến `10.0.4.9`, `TTL = 64`.
     - **Kỳ vọng:** Đi vòng 13 chặng qua lưới (tổng cost `130`), báo `DELIVERED | TTL=51`.
  4. Chọn `9` -> nhập `bfs`.
  5. Chọn `2` -> gửi lại từ `10.0.0.0` đến `10.0.4.9`, `TTL = 64`.
     - **Kỳ vọng:** Đi thẳng 1 chặng qua đường tắt `10.0.0.0 -> 10.0.4.9`, báo `DELIVERED | TTL=63`.
  6. Chọn `7` -> xuất ra `exported_50.csv`, sau đó chọn `6` -> nạp lại `exported_50.csv` và gửi ngược `10.0.4.9 -> 10.0.0.0`.

## 2. `02_static_routing_loop.csv` — Bẫy vòng lặp định tuyến tĩnh
- **Cấu trúc:** 3 router `R1 -> R2 -> R3 -> R1` đều có Static Route trỏ vòng tròn cho đích `R4`.
- **Các bước test:**
  1. Chọn `6` -> nhập `02_static_routing_loop.csv`.
  2. Chọn `9` -> nhập `static`.
  3. Chọn `2` -> gửi từ `R1` đến `R4`, `TTL = 7`.
     - **Kỳ vọng:** Chương trình **không bị treo**, gói tin chạy vòng `R1 -> R2 -> R3 -> R1 -> R2 -> R3 -> R1` rồi báo `FAILED: Router Error: Packet dropped due to expired TTL at Router R1` và dừng sạch sau 3 lần thử.

## 3. `03_cascading_failover.csv` — Sập link dây chuyền -> Mất mạng toàn phần -> Phục hồi
- **Cấu trúc:** 3 tuyến độc lập từ `SRC` đến `DST`:
  - Tuyến chính (`pA`): `SRC -> DST` (1 chặng, cost `10`).
  - Tuyến phụ (`pB`): `SRC -> M1 -> DST` (2 chặng, cost `40`).
  - Tuyến dự phòng (`pC`): `SRC -> M2 -> M3 -> DST` (3 chặng, cost `90`).
- **Các bước test:**
  1. Chọn `6` -> `03_cascading_failover.csv`, chọn `9` -> `dijkstra`.
  2. Gửi `SRC -> DST` (`TTL = 20`) -> **Kỳ vọng:** `SRC -> DST`, `TTL=19`.
  3. Chọn `3` -> tắt `SRC`, `pA`, `0` (**không** bấm hội tụ tay). Gửi `SRC -> DST` (`TTL = 20`) -> **Kỳ vọng:** Tự động reroute sang `SRC -> M1 -> DST`, `TTL=18`.
  4. Chọn `3` -> tắt `SRC`, `pB`, `0`. Gửi `SRC -> DST` (`TTL = 20`) -> **Kỳ vọng:** Tự động reroute sang `SRC -> M2 -> M3 -> DST`, `TTL=17`.
  5. Chọn `3` -> tắt `SRC`, `pC`, `0`. Gửi `SRC -> DST` (`TTL = 20`) -> **Kỳ vọng:** Mất mạng toàn phần, báo `No route to DST` và drop gói tin.
  6. Chọn `3` -> bật lại `SRC`, `pA`, `1`, chọn `8` (Converge). Gửi `SRC -> DST` (`TTL = 20`) -> **Kỳ vọng:** Quay lại đường chính `SRC -> DST`, `TTL=19`.

## 4. `04a_zero_cost_cycle.csv` & `04b_overflow_cost.csv` — Biên chi phí 0 và Tràn số `LLONG_MAX`
- **Với `04a_zero_cost_cycle.csv`:**
  1. Nạp file, chọn `9` -> `dijkstra`, chọn `1` (Print routing tables).
  2. **Kỳ vọng:** Không bị lặp vô hạn dù chu trình toàn `cost = 0`; bảng định tuyến `Z1` có route tới `Z3` với `Hops = 1`, `Cost = 0`.
- **Với `04b_overflow_cost.csv`:**
  1. Nạp file (`O1-O2` cost `9223372036854775802`, `O2-O3` cost `10`), chọn `9` -> `dijkstra`.
  2. **Kỳ vọng:** Chương trình bắt lỗi an toàn và in `Error occurred: Dijkstra distance overflow` thay vì bị tràn số âm.

## 5. `05_parallel_multilinks.csv` — 3 Link song song giữa cùng cặp Router
- **Cấu trúc:** `R1` và `R2` nối với nhau qua 3 cổng song song: `eth0` (cost `100`), `eth1` (cost `5`), `eth2` (cost `50`).
- **Các bước test:**
  1. Nạp file, chọn `9` -> `dijkstra`, chọn `1` -> **Kỳ vọng:** `R1` chọn cổng `eth1` (`Cost = 5`).
  2. Tắt `R1` `eth1` (`0`), chọn `8` -> `1` -> **Kỳ vọng:** Chuyển sang `eth2` (`Cost = 50`).
  3. Tắt `R1` `eth2` (`0`), chọn `8` -> `1` -> **Kỳ vọng:** Chuyển sang `eth0` (`Cost = 100`).
  4. Tắt `R1` `eth0` (`0`), chọn `8` -> `1` -> **Kỳ vọng:** Bảng động của `R1` rỗng.

## 6. `06_poisoned_static_route.csv` — Static Route "độc" đè lên Dynamic Route
- **Cấu trúc:** Có đường động hoạt động tốt `R1 -> R2 -> R3`, nhưng có sẵn Static Route `R1 -> R3` đi qua cổng `eth0` đang `DOWN`.
- **Các bước test:**
  1. Nạp file, chọn `9` -> `dijkstra`.
  2. Chọn `2` -> gửi `R1 -> R3` (`TTL = 10`) -> **Kỳ vọng:** Bị thất bại do `Interface eth0 of router R1 is unavailable` (chứng minh Static ưu tiên hơn Dynamic).
  3. Chọn `5` -> xóa static route của `R1` tới đích `R3`.
  4. Chọn `2` -> gửi lại `R1 -> R3` (`TTL = 10`) -> **Kỳ vọng:** Thành công qua `R1 -> R2 -> R3`, `DELIVERED | TTL=8`.

## 7. `07_hostile_corrupted.csv` — File CSV rác & lỗi định dạng hỗn tạp
- **Cấu trúc:** Chứa xuống dòng Windows `
`, dòng trống, `#` comment, trùng router, cost âm, cost chữ, tràn số, trạng thái `MAYBE`, thiếu/thừa cột, `nextIP` rỗng, router/interface không tồn tại.
- **Các bước test:**
  1. Chọn `6` -> nhập `07_hostile_corrupted.csv`.
  2. Chọn `1` (Print routing tables).
  3. **Kỳ vọng:** Chương trình **không crash**, chỉ nạp đúng 2 router (`R1`, `R2`), 1 link (`eth0`), 1 static route hợp lệ (`R1 -> R2`), và ghi đầy đủ dòng `(WARNING)` vào `./data/network_events.txt`.

## 8. `08_self_and_isolated.csv` — Gửi tới chính mình, Router cô lập & Đổi chiến lược lỗi
- **Cấu trúc:** `R1 <-> R2` và 1 router cô lập `ISLAND`.
- **Các bước test:**
  1. Nạp file, chọn `9` -> `dijkstra`.
  2. Gửi `R1 -> R1` (`TTL = 42`) -> **Kỳ vọng:** Giao ngay lập tức, `DELIVERED | TTL=42`.
  3. Gửi `R1 -> ISLAND` (`TTL = 10`) -> **Kỳ vọng:** Báo `FAILED: No route to ISLAND`.
  4. Chọn `9` -> nhập `bgp_invalid` -> **Kỳ vọng:** Báo `Error occurred: Unknown routing strategy: bgp_invalid`, sau đó gửi `R1 -> R2` vẫn thành công bình thường.

## 9. `09_reload_lifecycle.csv` — Kiểm tra nạp lại liên tiếp & nhập sai tên file
- **Cấu trúc:** Chuỗi 3 router `A <-> B <-> C`.
- **Các bước test:**
  1. Nạp `09_reload_lifecycle.csv`, chọn `9` -> `dijkstra`.
  2. Chọn `6` -> nhập `khong_ton_tai.csv` -> **Kỳ vọng:** Báo lỗi `Cannot open csv file`, nhưng gửi `A -> C` vẫn chạy bình thường (mạng cũ không bị xóa).
  3. Chọn `6` -> nạp lại `09_reload_lifecycle.csv` nhiều lần liên tiếp rồi gửi `A -> C` (`TTL = 10`) -> **Kỳ vọng:** Luôn giữ nguyên thuật toán `dijkstra`, `DELIVERED | TTL=8` mà không cần chọn lại Option 9.
