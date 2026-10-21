# PHÂN CÔNG ĐỒ ÁN — 3 THÀNH VIÊN

## Nguyên tắc

Một repository Git chung, 3 nhánh tính năng và nhánh `main` ổn định. Nhóm thống nhất chữ ký các phương thức trong `include/network/` trước khi chỉnh sửa mã. Không chia thành 3 dự án thực thi riêng — các lớp phải liên kết với nhau.

### Thành viên 1: Lõi mô hình mạng — `feature/core`

**Sở hữu tệp:**

- `include/network/Types.hpp`, `Exceptions.hpp`
- `include/network/Route.hpp`, `RoutingTable.hpp`, `Interface.hpp`, `Packet.hpp`, `Router.hpp`
- `src/core/Route.cpp`, `RoutingTable.cpp`, `Interface.cpp`, `Packet.cpp`, `Router.cpp`

**Nhiệm vụ:** xác thực dữ liệu đầu vào cho class; hoàn thiện quản lý Interface theo Router; kiểm tra `Route` và `RoutingTable`; bảo đảm `forwardPacket()` sử dụng đúng `nextIP` và giảm TTL; giải thích `weak_ptr` và vòng tham chiếu. **Không viết BFS/Dijkstra trong Router.**

**Kiểm thử đề xuất:** 1 Router nhiều Interface; thêm trùng id; `Route` rỗng; gói tin TTL=0; tìm `Route` không tồn tại; `Interface` bị DOWN.

### Thành viên 2: Thuật toán & mẫu Strategy — `feature/routing`

**Sở hữu tệp:**

- `include/network/RoutingStrategy.hpp`, `StrategyFactory.hpp`
- `src/routing/RoutingAlgorithms.cpp`, `StrategyFactory.cpp`

**Nhiệm vụ:** triển khai `StaticRouting`, `ShortestPathRouting` (BFS), `CostBasedRouting` (Dijkstra); kiểm tra 2 đầu liên kết còn hoạt động; tạo bảng tuyến dạng next-hop/iface; tránh tràn `long long`; quyết định cách phá hòa tuyến cùng chi phí; phân tích độ phức tạp thuật toán.

**Kiểm thử đề xuất:** BFS và Dijkstra cho tuyến khác nhau; nhiều liên kết; đồ thị không liên thông; liên kết DOWN; đổi cost.

### Thành viên 3: Quản lý, tệp dữ liệu, demo, tích hợp — `feature/management`

**Sở hữu tệp:**

- `include/network/Administrator.hpp`, `CsvTopologyStorage.hpp`, `TextLogger.hpp`
- `src/management/Administrator.cpp`, `CsvTopologyStorage.cpp`, `TextLogger.cpp`
- `src/main.cpp`, `data/`, `tests/`, `docs/`, `README.md`, `CMakeLists.txt`

**Nhiệm vụ:** thiết kế quy ước CSV; kiểm tra dữ liệu khi nhập; quản lý chiến lược toàn mạng; ghi log; bổ sung menu; kiểm thử tích hợp; chuẩn bị kịch bản thuyết trình và đối chiếu UML thực tế.

**Kiểm thử đề xuất:** đọc/ghi CSV vòng khứ hồi; file lỗi không làm mất mạng đã có; đổi chiến lược; thay đổi trạng thái/chi phí liên kết; demo gói tin từ A tới B.

## Chữ ký giao tiếp giữa 3 phần

- Thành viên 1 cung cấp `Router::getInterfaces()`, `Router::getRoutingTable()`, `Router::forwardPacket()`, `Router::updateRoutes()`.
- Thành viên 2 hiện thực `RoutingStrategy::calculateRoutes(Router&, const Topology&)` và `StrategyFactory::createStrategy(type)`.
- Thành viên 3 chỉ gọi các phương thức công khai thông qua `Administrator` và lớp lưu trữ.

**Lưu ý:** `Router::updateRoutes()` (người 1) sẽ gọi `strategy_->calculateRoutes()` (người 2); chức năng `convergeNetwork()` (người 3) gọi `Router::updateRoutes()` của từng Router. Một phần phụ thuộc chưa hoàn thiện có thể tạm dùng bản stub để biên dịch; không tự ý sửa chữ ký công khai của người khác mà chưa thống nhất.

## Quy trình Git nên dùng

**Bước 1 — một người tạo repository, đẩy khung chương trình:**

```bash
git init
git add .
git commit -m "chore: initial routing simulation skeleton"
git branch -M main
git remote add origin <URL_REPOSITORY_CUA_NHOM>
git push -u origin main
```

**Bước 2 — từng thành viên clone và lập nhánh:**

```bash
git clone <URL_REPOSITORY_CUA_NHOM>
cd RoutingSimulation_CPP20
git switch -c feature/core
# Người 2 chọn feature/routing; người 3 chọn feature/management.
```

**Bước 3 — chỉnh sửa đúng vùng được phân công:**

```bash
git status
git add include/network/Router.hpp src/core/Router.cpp
git commit -m "feat(core): implement packet forwarding"
git push -u origin feature/core
```

**Bước 4 — mở Pull Request trên GitHub** từ nhánh của mình vào `main`; thành viên khác kiểm tra thay đổi, chạy biên dịch/CTest và duyệt rồi mới merge. Nên merge lần lượt `feature/core`, `feature/routing`, `feature/management` nếu cả ba bắt đầu từ bản khung chưa hoàn thiện.

**Bước 5 — cập nhật nhánh mới nhất:**

```bash
git switch main
git pull origin main
git switch feature/routing
git merge main
```

Giải quyết xung đột trong VS Code, kiểm thử lại và tiếp tục push. Tránh thay đổi `CMakeLists.txt` cùng lúc. Không đẩy thư mục `build/`, file `.exe`, `.log` hoặc tệp tạo trong lúc kiểm thử (đã có `.gitignore`).

## Chia phần trình bày trước hội đồng

- **Người 1:** UML lõi, đóng gói, composition/association, smart pointer và xử lý gói tin.
- **Người 2:** đa hình Strategy, BFS/Dijkstra, so sánh chi phí/độ phức tạp, mô phỏng khi liên kết DOWN.
- **Người 3:** Facade, Factory, Persistence/CSV, logging, menu CLI, sơ đồ luồng tổng thể và bộ kiểm thử.

## Cách sử dụng bộ mã mẫu

Bộ mã nguồn cung cấp sẵn hiện là **phiên bản tích hợp mẫu chạy được**, không phải bằng chứng ba thành viên đã thực sự phân công hoặc tự viết các phần của mình. Nhóm nên tạo vấn đề (Issues), kiểm tra, hiểu, bổ sung kiểm thử và ghi commit của từng người trước khi nộp đồ án.
