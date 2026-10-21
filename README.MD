# ĐỒ ÁN LẬP TRÌNH HƯỚNG ĐỐI TƯỢNG – CHỦ ĐỀ 20
## Mô phỏng Router và định tuyến mạng (C++20)

Dự án được triển khai từ sơ đồ lớp UML trong PDF do nhóm cung cấp. Mỗi Router quản lý các cổng mạng, bảng định tuyến và một chiến lược tính đường. Administrator là bộ điều phối chương trình.

### 1. Các chức năng hoàn thiện

- Đọc / xuất topo mạng từ một tệp CSV.
- Định tuyến tĩnh, BFS theo **số bước nhảy**, Dijkstra theo **tổng linkCost**.
- Lưu tuyến tĩnh độc lập, cho phép tuyến tĩnh **ưu tiên** tuyến do BFS/Dijkstra sinh ra.
- Mô phỏng chuyển tiếp gói tin theo bảng định tuyến; có `ttl`, báo lỗi khi không có tuyến hoặc liên kết hỏng.
- Thay đổi trạng thái/chi phí liên kết ở **cả hai đầu** và tự tính lại bảng định tuyến.
- Ghi nhật ký `network_events.txt`, menu dòng lệnh, kiểm thử CTest.

### 2. Yêu cầu

- Trình biên dịch hỗ trợ C++20: GCC 11+, Clang 14+ hoặc MSVC tương thích.
- CMake >= 3.16.
- Windows 11 + VS Code với `C/C++` và `CMake Tools` hoặc Linux/macOS.

### 3. Biên dịch và chạy

**Windows / VS Code (mở terminal PowerShell tại thư mục gốc dự án)**:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\router_sim.exe data\network.csv
```

Nếu cấu hình VS Code có CMake/Ninja, hoặc `-G "MinGW Makefiles"` bị lỗi, bỏ tùy chọn `-G` và chạy:

```powershell
cmake -S . -B build
cmake --build build
.\build\router_sim.exe data\network.csv
```

**Linux/macOS**:

```bash
cmake -S . -B build
cmake --build build -j
./build/router_sim data/network.csv
```

Lưu ý: các lệnh chạy từ **thư mục gốc dự án**, nơi chứa `CMakeLists.txt` và `data/`. Nếu gặp lỗi đổi bộ tạo CMake (generator), xóa thư mục `build` rồi cấu hình lại.

**Chạy kiểm thử:**

```bash
ctest --test-dir build --output-on-failure
```

**Trong VS Code:** sau khi cài CMake, mở đúng **thư mục gốc** dự án và nhấn `Ctrl+Shift+B` để cấu hình & biên dịch bằng tác vụ có sẵn. Muốn chạy toàn bộ kiểm thử, chọn `Terminal > Run Task > CMake: Test`. Extension `C/C++ Runner: Run File` **không phù hợp để chạy riêng `main.cpp`**, vì dự án có nhiều file `.cpp` cần liên kết với nhau.

### 4. Cấu trúc thư mục

```text
RoutingSimulation_CPP20/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── .vscode/tasks.json
├── data/
│   └── network.csv
├── docs/
│   ├── PHAN_CONG_NHOM.md
│   ├── GIAI_THICH_KIEN_TRUC.md
│   └── uml_implementation.puml
├── include/network/
│   ├── Types.hpp
│   ├── Exceptions.hpp
│   ├── Route.hpp
│   ├── RoutingTable.hpp
│   ├── Interface.hpp
│   ├── Packet.hpp
│   ├── Router.hpp
│   ├── RoutingStrategy.hpp
│   ├── StrategyFactory.hpp
│   ├── TextLogger.hpp
│   ├── CsvTopologyStorage.hpp
│   └── Administrator.hpp
├── src/
│   ├── core/
│   │   ├── Route.cpp
│   │   ├── RoutingTable.cpp
│   │   ├── Interface.cpp
│   │   ├── Packet.cpp
│   │   └── Router.cpp
│   ├── routing/
│   │   ├── RoutingAlgorithms.cpp
│   │   └── StrategyFactory.cpp
│   ├── management/
│   │   ├── Administrator.cpp
│   │   ├── CsvTopologyStorage.cpp
│   │   └── TextLogger.cpp
│   └── main.cpp
└── tests/
    ├── test_routing.cpp
    └── test_random_graphs.cpp
```

### 5. Định dạng CSV

CSV được **đơn giản hóa**, mỗi dòng là một đối tượng hoặc một liên kết. Không hỗ trợ giá trị có dấu phẩy hoặc ký tự xuống dòng bên trong chuỗi. Các dòng có `#` đầu dòng được bỏ qua.

```csv
ROUTER,10.0.0.1
ROUTER,10.0.0.2
LINK,10.0.0.1,eth0,10.0.0.2,eth0,2,UP
STATIC,10.0.0.1,10.0.0.2,10.0.0.2,eth0,1,2
```

- `ROUTER,ip`: khai báo một Router.
- `LINK,ipA,ifaceA,ipB,ifaceB,cost,UP/DOWN`: liên kết **hai chiều**, hai interface được tạo đồng thời. `cost > 0`, không chấp nhận link một Router về chính nó. Mỗi Router không được trùng `interfaceId`.
- `STATIC,routerIP,destIP,nextIP,interfaceId,hopCount,cost`: tuyến tĩnh; `nextIP` **phải là Router kề** được nối bởi `interfaceId`. Mọi IP phải có bản ghi `ROUTER`. `hopCount >= 1`; `cost >= 0`.

Có thể đặt các dòng `LINK`/`STATIC` trước `ROUTER` vì trình đọc sẽ gom tất cả Router rồi mới dựng liên kết, sau đó mới thêm tuyến tĩnh. Tệp mẫu có 5 Router và 7 liên kết.

> **Giới hạn:** `linkIP` trong `Interface` được diễn giải là **IP Router hàng xóm**, không phải địa chỉ IP riêng của interface. `destIP` phải trùng IP Router (tra cứu chính xác); không mô phỏng subnet, CIDR, ARP, NAT, mạng không dây hay định tuyến thực tế theo LPM.

### 6. Ví dụ minh họa

Từ tệp mẫu, sau khi chạy chương trình, thuật toán mặc định là `dijkstra`:

- Router `10.0.0.2` tới `10.0.0.5`:
  - BFS: đi thẳng `10.0.0.2 → 10.0.0.5`, **1 bước nhảy**.
  - Dijkstra: `10.0.0.2 → 10.0.0.4 → 10.0.0.5`, **chi phí 3**.
- Router `10.0.0.1` tới `10.0.0.5` có **tuyến tĩnh** qua `10.0.0.3`. Vì tuyến tĩnh ưu tiên, tuyến này ghi đè kết quả Dijkstra. Xóa tuyến tĩnh tại Router 1 thì kết quả là `1 → 2 → 4 → 5` với tổng cost 5.

Để gửi thử một gói tin, chọn chức năng `3`, nhập:

```text
Router nguồn: 10.0.0.2
Router đích: 10.0.0.5
Nội dung: Hello
TTL: 4
```

Kết quả mong đợi:

```text
Packet path: 10.0.0.2 -> 10.0.0.4 -> 10.0.0.5 | DELIVERED | TTL=2
```

### 7. Kiểm thử tự động

Tệp `tests/test_routing.cpp` bao gồm kiểm tra: dựng topo, số interface, BFS, Dijkstra, ưu tiên tuyến tĩnh, xóa tuyến tĩnh, đổi chi phí, ngắt/nối liên kết, TTL, `InterfaceDownException`, xuất/nhập CSV, bảo toàn mạng khi CSV lỗi, và mô phỏng gửi gói tin end-to-end. Ngoài ra, `tests/test_random_graphs.cpp` đối chiếu BFS/Dijkstra trên **45 đồ thị ngẫu nhiên** với Floyd–Warshall để kiểm tra tính đúng đắn của chi phí tối ưu.

### 8. Phân công và cách làm nhóm

Đọc `docs/PHAN_CONG_NHOM.md` (các thư mục có người phụ trách và hướng dẫn Git) và `docs/GIAI_THICH_KIEN_TRUC.md` (giải thích các quyết định thiết kế, độ phức tạp, đối chiếu với UML).

### 9. Những phần mở rộng nên cân nhắc nếu còn thời gian

- Hiển thị topo dạng đồ thị trực quan, in đường đi và chi phí theo từng bước.
- Kịch bản tự kiểm thử đầu vào lớn, đo thời gian BFS/Dijkstra.
- Lưu bảng các chiến lược cho từng Router thay vì đồng loạt.
- Bổ sung phân hệ định tuyến theo tiền tố mạng và nhiều đường đi tới một đích.

**Lưu ý khi bảo vệ:** Đây là **mô phỏng học thuật** về đồ thị và định tuyến. Không truyền gói tin Ethernet/IP thật, không mở socket, không hiện thực giao thức OSPF/RIP.
