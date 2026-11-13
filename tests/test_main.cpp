#include <iostream>
#include <memory>
#include <string>
#include <cassert>
#include <stdexcept>
// Bao gồm các tệp tiêu đề của Data Plane
#include "Route.hpp"
#include "RoutingTable.hpp"
#include "Packet.hpp"
#include "Interface.hpp"
#include "Router.hpp"
#include "Exceptions.hpp"

// Hàm tiện ích in tiêu đề kiểm thử
void printTestHeader(const std::string& testName) {
    std::cout << "\n==================================================\n";
    std::cout << ">>> DANG CHAY: " << testName << "\n";
    std::cout << "==================================================\n";
}

// ---------------------------------------------------------------
// Kịch bản 1: Kiểm thử Route và Packet (Value Types & Invariants)
// ---------------------------------------------------------------
void testValueTypes() {
    printTestHeader("TEST 1: Kiem thu Value Types (Route & Packet)");

    // 1.1 Kiểm tra khởi tạo Route hợp lệ
    Route r1{"192.168.2.0", "10.0.0.2", "eth0", 1, 10};
    std::cout << "[PASS] Khoi tao Route hop le: " << r1 << "\n";

    // 1.2 Kiểm tra Invariant của Route: destIP rỗng phải ném ngoại lệ
    try {
        Route rBad{"", "10.0.0.2", "eth0", 1, 10};
        assert(false && "LOI: Khong chan duoc destIP rong!");
    } catch (const std::invalid_argument& e) {
        std::cout << "[PASS] Bat dung loi Route Invariant: " << e.what() << "\n";
    }

    // 1.3 Kiểm tra Packet hợp lệ và giảm TTL
    Packet pkt{"192.168.1.10", "192.168.2.20", "Xin chao Router!", 5};
    assert(pkt.getTTL() == 5);
    pkt.decrementTTL();
    assert(pkt.getTTL() == 4);
    assert(!pkt.isExpired());
    std::cout << "[PASS] Khoi tao va giam TTL goi tin thanh cong. TTL hien tai: " << pkt.getTTL() << "\n";

    // 1.4 Kiểm tra Invariant của Packet: TTL <= 0 phải bị chặn
    try {
        Packet badPkt{"10.0.0.1", "10.0.0.2", "Data", 0};
        assert(false && "LOI: Khong chan duoc TTL khoi tao <= 0!");
    } catch (const std::invalid_argument& e) {
        std::cout << "[PASS] Bat dung loi Packet Invariant: " << e.what() << "\n";
    }
}

// ---------------------------------------------------------------
// Kịch bản 2: Kiểm thử Bảng định tuyến (RoutingTable)
// ---------------------------------------------------------------
void testRoutingTable() {
    printTestHeader("TEST 2: Kiem thu RoutingTable");

    RoutingTable table;
    Route r1{"10.1.1.0", "192.168.1.2", "eth0", 2, 20};
    Route r2{"10.2.2.0", "", "eth1", 1, 5}; // Direct link (nextIP rỗng)

    table.addRoute(r1);
    table.addRoute(r2);

    assert(table.hasRoute("10.1.1.0"));
    assert(table.hasRoute("10.2.2.0"));
    assert(!table.hasRoute("172.16.0.0"));
    std::cout << "[PASS] Kiem tra ton tai tuyen duong hasRoute() chinh xac.\n";

    // Tìm kiếm tuyến hợp lệ
    Route found = table.findRoute("10.1.1.0");
    assert(found.getInterfaceId() == "eth0");
    std::cout << "[PASS] Tim thay tuyen: " << found << "\n";

    // Tìm kiếm tuyến không tồn tại -> Bắt NoRouteException
    try {
        table.findRoute("192.168.99.99");
        assert(false && "LOI: Khong nem NoRouteException khi tim IP la!");
    } catch (const NoRouteException& e) {
        std::cout << "[PASS] Bat dung NoRouteException khi tim IP la: " << e.what() << "\n";
    }

    // Xóa tuyến đường
    table.removeRoute("10.1.1.0");
    assert(!table.hasRoute("10.1.1.0"));
    std::cout << "[PASS] Xoa tuyen duong 10.1.1.0 thanh cong.\n";
}

// ---------------------------------------------------------------
// Kịch bản 3: Kiểm thử Cổng mạng và liên kết đối diện (Interface & Peer)
// ---------------------------------------------------------------
void testInterfaceAndPeer() {
    printTestHeader("TEST 3: Kiem thu Interface & std::weak_ptr Peer");

    // Tạo 2 cổng mạng mô phỏng 2 đầu của sợi cáp
    auto ifaceA = std::make_shared<Interface>("eth0", "10.0.0.2", "eth0", true, 10);
    auto ifaceB = std::make_shared<Interface>("eth0", "10.0.0.1", "eth0", true, 10);

    // Kết nối chéo (Peer connection)
    ifaceA->connectPeer(ifaceB);
    ifaceB->connectPeer(ifaceA);

    // Kiểm tra kết nối thông qua getPeer() (khóa weak_ptr thành shared_ptr)
    auto peerOfA = ifaceA->getPeer();
    assert(peerOfA != nullptr);
    assert(peerOfA->getLinkIP() == "10.0.0.1");
    std::cout << "[PASS] ifaceA nhan dien dung cong doi dien ifaceB voi linkIP: " << peerOfA->getLinkIP() << "\n";

    // Thử nghiệm thay đổi trạng thái và chi phí
    ifaceA->setStatus(false);
    assert(!ifaceA->getStatus());
    ifaceA->setCost(50);
    assert(ifaceA->getCost() == 50);
    std::cout << "[PASS] Cap nhat trang thai va cost cua cong thanh cong.\n";
}

// ---------------------------------------------------------------
// Kịch bản 4: Kiểm thử Router và Chuyển tiếp gói tin (Forwarding)
// ---------------------------------------------------------------
void testRouterForwarding() {
    printTestHeader("TEST 4: Kiem thu Chuyen tiep Router (forwardPacket)");

    // Khởi tạo Router R1 với IP 192.168.1.1
    Router r1{"192.168.1.1"};

    // Thêm các cổng mạng cho R1
    auto if0 = std::make_shared<Interface>("eth0", "10.0.0.2", "eth0", true, 10);
    auto if1 = std::make_shared<Interface>("eth1", "172.16.0.2", "eth0", true, 5);
    r1.addInterface(if0);
    r1.addInterface(if1);

    // Cấu hình bảng định tuyến tĩnh:
    // Tuyến 1: Đến mạng 10.0.0.0/24 trỏ qua NextHop 10.0.0.2 cổng eth0
    r1.getStaticRoutingTable().addRoute(Route{"10.0.0.2", "10.0.0.2", "eth0", 1, 10});
    // Tuyến 2: Kết nối trực tiếp đến IP 172.16.0.2 cổng eth1 (nextIP rỗng)
    r1.getStaticRoutingTable().addRoute(Route{"172.16.0.2", "", "eth1", 1, 5});

    // 4.1 Gói tin gửi đến chính Router R1
    Packet pktToSelf{"192.168.1.50", "192.168.1.1", "Ping R1", 64};
    std::string nextHop1 = r1.forwardPacket(pktToSelf);
    assert(nextHop1 == "192.168.1.1");
    std::cout << "[PASS] Goi tin gui den chinh Router da duoc xu ly tai cho (IP: " << nextHop1 << ")\n";

    // 4.2 Gói tin chuyển tiếp qua trạm kế tiếp (Next Hop)
    Packet pktToNetwork{"192.168.1.50", "10.0.0.2", "Hello Remote", 64};
    std::string nextHop2 = r1.forwardPacket(pktToNetwork);
    assert(nextHop2 == "10.0.0.2");
    assert(pktToNetwork.getTTL() == 63); // Kiểm tra TTL đã tự động giảm 1
    std::cout << "[PASS] Chuyen tiep thanh cong den NextHop: " << nextHop2 << " (TTL con lai: " << pktToNetwork.getTTL() << ")\n";

    // 4.3 Gói tin chuyển tiếp qua liên kết trực tiếp (nextIP rỗng -> trả về destIP)
    Packet pktDirect{"192.168.1.50", "172.16.0.2", "Hello Direct", 10};
    std::string nextHop3 = r1.forwardPacket(pktDirect);
    assert(nextHop3 == "172.16.0.2");
    std::cout << "[PASS] Chuyen tiep lien ket truc tiep thanh cong (Dich: " << nextHop3 << ")\n";
}

// ---------------------------------------------------------------
// Kịch bản 5: Kiểm thử Ngoại lệ khi Cổng mạng bị Down
// ---------------------------------------------------------------
void testInterfaceDownException() {
    printTestHeader("TEST 5: Kiem thu Ngoai le InterfaceDownException");

    Router r1{"192.168.1.1"};
    auto if0 = std::make_shared<Interface>("eth0", "10.0.0.2", "eth0", true, 10);
    r1.addInterface(if0);
    r1.getStaticRoutingTable().addRoute(Route{"10.0.0.2", "10.0.0.2", "eth0", 1, 10});

    // Tắt cổng mạng eth0
    r1.setInterfaceStatus("eth0", false);
    assert(!r1.getInterface("eth0")->getStatus());

    Packet pkt{"192.168.1.100", "10.0.0.2", "Test Data", 10};
    try {
        r1.forwardPacket(pkt);
        assert(false && "LOI: Khong nem InterfaceDownException khi cong bi tat!");
    } catch (const InterfaceDownException& e) {
        std::cout << "[PASS] Bat dung ngoai le InterfaceDownException: " << e.what() << "\n";
    }
}

// ---------------------------------------------------------------
// Kịch bản 6: Kiểm thử Ngoại lệ khi Gói tin hết hạn TTL
// ---------------------------------------------------------------
void testTtlExpiredException() {
    printTestHeader("TEST 6: Kiem thu Ngoai le TtlExpiredException");

    Router r1{"192.168.1.1"};
    auto if0 = std::make_shared<Interface>("eth0", "10.0.0.2", "eth0", true, 10);
    r1.addInterface(if0);
    r1.getStaticRoutingTable().addRoute(Route{"10.0.0.2", "10.0.0.2", "eth0", 1, 10});

    // Gói tin có TTL = 1, khi qua Router r1 sẽ bị giảm về 0 và hủy
    Packet dyingPkt{"192.168.1.100", "10.0.0.2", "Goi tin sap het han", 1};
    try {
        r1.forwardPacket(dyingPkt);
        assert(false && "LOI: Khong nem TtlExpiredException khi TTL = 0!");
    } catch (const std::runtime_error& e) {
        std::cout << "[PASS] Bat dung ngoai le TtlExpiredException: " << e.what() << "\n";
    }
}

// ---------------------------------------------------------------
// HÀM CHÍNH (MAIN)
// ---------------------------------------------------------------
int main() {
    std::cout << "========================================================\n";
    std::cout << "   BAT DAU KIEM THU TOAN DIEN PHAN HE DATA PLANE\n";
    std::cout << "========================================================\n";

    try {
        testValueTypes();
        testRoutingTable();
        testInterfaceAndPeer();
        testRouterForwarding();
        testInterfaceDownException();
        testTtlExpiredException();

        std::cout << "\n========================================================\n";
        std::cout << "   CHUC MUNG! TAT CA CAC KIEM THU DEU DAT CHUAN (100% PASS)\n";
        std::cout << "========================================================\n";
    } catch (const std::exception& e) {
        std::cerr << "\n[THAT BAI] Xay ra loi khong mong muon: " << e.what() << "\n";
        return 1;
    }

    return 0;
}