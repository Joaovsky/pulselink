#include "can_writer.hpp"
#include <cstring>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

bool CanWriter::open(const std::string& ifname) {
    fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd_ < 0) return false;
    struct ifreq ifr{};
    std::strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);
    if (ioctl(fd_, SIOCGIFINDEX, &ifr) < 0) return false;
    struct sockaddr_can addr{};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    return bind(fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == 0;
}

bool CanWriter::send(uint32_t can_id, const std::vector<uint8_t>& data) {
    struct can_frame frame{};
    frame.can_id = can_id;
    frame.can_dlc = static_cast<uint8_t>(data.size());
    std::memcpy(frame.data, data.data(), data.size());
    return write(fd_, &frame, sizeof(frame)) == static_cast<ssize_t>(sizeof(frame));
}

void CanWriter::close_fd() { if (fd_ >= 0) { ::close(fd_); fd_ = -1; } }
