# GIẢI THÍCH KIẾN TRÚC VÀ ĐỐI CHIẾU UML

## 1. Bốn vùng trách nhiệm trong sơ đồ PDF

- **Control Plane:** `RoutingStrategy`, `StaticRouting`, `ShortestPathRouting`, `CostBasedRouting`, `StrategyFactory`.
- **Data Plane:** `Router`, `Interface`, `RoutingTable`, `Route`, `Packet`.
- **Exception Hierarchy:** `std::runtime_error`, `NoRouteException`, `InterfaceDownException`.
- **Management & Persistence:** `Administrator`, `CsvTopologyStorage`, `TextLogger`.

Đã sử dụng các mẫu thiết kế **Strategy**, **Factory** và **Facade** theo sơ đồ gốc.

## 2. Quy trình khởi tạo

1. `Administrator.importNetworkFromCsv(path)` gọi `CsvTopologyStorage.loadTopology()`.
2. Đọc Router, tạo các cặp Interface đối xứng cho LINK, ghép peer bằng `weak_ptr`.
3. Đọc tuyến tĩnh vào mỗi Router.
4. `convergeNetwork()` gọi `Router.updateRoutes(topology)` cho từng Router.
5. Mỗi Router dùng một `RoutingStrategy` để tính bảng tuyến, rồi nạp lại tuyến tĩnh để **ưu tiên tuyến tĩnh**.
6. Chương trình demo thiết lập chế độ `dijkstra` ngay sau khi khởi tạo; `Router` khởi tạo độc lập thì mặc định là `static`.

## 3. Quy trình chuyển tiếp gói tin

1. `Administrator.simulatePacketTransmission(src,dest,payload,ttl)` tạo `Packet`.
2. Nếu Router hiện tại chính là đích, kết thúc thành công.
3. `Router.forwardPacket()` kiểm tra TTL, tìm đúng `destIP` trong `RoutingTable`, lấy cổng ra bằng `interfaceId`.
4. Chỉ tiếp tục nếu cả cổng hiện tại và cổng đối diện đều hoạt động; kiểm tra `nextIP` trùng Router liền kề.
5. Trừ TTL **một lần trên mỗi bước nhảy** rồi trả về IP của Router kế tiếp.
6. Lặp cho đến đích hoặc đến khi có lỗi; `Administrator` ghi log.

**Chú ý:** `forwardPacket()` không thực thi BFS/Dijkstra cho mỗi gói tin. Các thuật toán chỉ tính lại bảng khi có lệnh cập nhật hoặc thay đổi topo.

## 4. Phân biệt BFS và Dijkstra

- `ShortestPathRouting` dùng **BFS**: các cạnh hợp lệ có trọng số giả định 1; `Route.cost` trong chế độ này là **số bước nhảy**.
- `CostBasedRouting` dùng **Dijkstra**: tổng `Interface.linkCost`; giả thiết tất cả `linkCost > 0`.
- BFS cho một nguồn: `O(V+E)` thời gian. Dijkstra dùng `priority_queue`: `O((V+E) log V)` với chi phí cạnh dương. Mỗi lần `convergeNetwork()` gọi thuật toán cho mọi Router, nên tổng chi phí tăng khoảng `V` lần nếu mọi Router dùng cùng thuật toán.
- Bảng định tuyến lưu chính xác `destIP`, `nextIP`, `interfaceId`, `hopCount`, `cost` thay vì lưu một đường đi đầy đủ. Khi chi phí hòa, ưu tiên ít hop rồi next-hop theo thứ tự từ điển trong Dijkstra.

## 5. Những phần mở rộng có chủ ý so với UML

1. `Router.staticRoutes_ : RoutingTable`: tách dữ liệu cấu hình thủ công khỏi bảng được tính lại. Khi tính xong, tuyến tĩnh ghi đè tuyến động có cùng đích. Tuyến tĩnh **không tự sửa** nếu đứt link, có thể khiến việc chuyển tiếp thất bại; đó là mô phỏng chính xác hành vi cấu hình tuyến tĩnh không hợp lệ.
2. `Router.addStaticRoute/removeStaticRoute/getStaticRoutes/getStrategyName`: hỗ trợ lưu, quản lý và hiển thị chế độ định tuyến.
3. `Interface.getPeer()`: cần để kiểm tra đầu đối diện, đồng bộ `linkCost/isUp` và xuất CSV.
4. `Administrator.setLinkCost(...)`: bổ sung kịch bản thay đổi chi phí liên kết đã yêu cầu.
5. `Administrator.getRouter/getRouters`: truy cập chỉ đọc phục vụ kiểm thử và tích hợp.
6. `Types.hpp`: dùng alias `Topology` thay chữ ký dài `unordered_map<string, shared_ptr<Router>>` trong UML. Cùng một kiểu C++.
7. `CsvTopologyStorage` là thành viên riêng (`storage_`) của `Administrator`, việc lưu log/thao tác CSV không trộn vào `Router`.

## 6. Giới hạn và quy ước thực thi

- Link là **vô hướng**: một dòng LINK tạo hai Interface ở hai Router; trạng thái và cost cập nhật đồng thời hai đầu.
- Có thể có nhiều link giữa cùng hai Router nếu tên interface khác nhau; cùng một tên Interface trong một Router là không hợp lệ.
- `weak_ptr` chỉ tham chiếu đến Interface đối diện, không sở hữu nó, tránh hai Interface giữ nhau sống mãi.
- Cho phép Router không có Interface (Router cô lập). Đây là diễn giải rộng hơn ký hiệu `1..*` ở một số quan hệ trên PDF để hỗ trợ mạng không liên thông.
- Chỉ mô phỏng tuyến từ một Router đến **đúng IP Router đích**; không xử lý prefix hay thiết bị ở subnet khác.
- `TextLogger` là lớp ghi tệp thông thường, ghi trực tiếp theo thời gian thực; chương trình dùng một luồng chính.
- Giả thiết đồ thị nhỏ hoặc vừa (đồ án môn học), chưa tối ưu bằng lazy cache. Cập nhật toàn mạng ngay sau mọi thay đổi link là chủ đích cho dễ giải thích và kiểm thử.
- Khi nhập một file CSV mới, trạng thái Router được tạo lại và chiến lược của chúng trở về `static`. Sau đó người dùng có thể chọn lại chế độ bằng menu 2. Menu khởi tạo lúc đầu tự chọn `dijkstra`.

## 7. Câu hỏi phản biện thường gặp

**Tại sao cần RoutingStrategy?** Để Router phụ thuộc giao diện chung thay vì phụ thuộc trực tiếp BFS/Dijkstra, phù hợp đa hình runtime và nguyên tắc mở-đóng.

**Tại sao `unordered_map` thay vì Trie?** Vì đích ở đây là IP định danh Router và tra cứu chính xác, không tìm tiền tố dài nhất; `unordered_map` thường có thời gian trung bình O(1).

**Tại sao dùng `unique_ptr<RoutingStrategy>`?** Vì mỗi Router sở hữu duy nhất thuật toán của mình và có thể thay đổi thuật toán; các đối tượng chiến lược không cần sở hữu chung.

**Tại sao cổng nối nhau bằng `weak_ptr`?** Để tránh chu kỳ sở hữu `shared_ptr` giữa hai Interface.

**Link DOWN có thay đổi đường ngay không?** Có, `Administrator.setLinkStatus()` cập nhật hai đầu và gọi `convergeNetwork()`. Tuyến tĩnh vẫn được giữ nguyên nếu người dùng chưa xóa, vì tuyến tĩnh ưu tiên theo quy ước đã chọn.
