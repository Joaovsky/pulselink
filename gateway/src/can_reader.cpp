#include "can_reader.hpp"
#include <cstring>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

bool CanReader::open(const std::string& ifname) {
    fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd_ < 0) return false;

    struct ifreq ifr{};
    std::strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);
    if (ioctl(fd_, SIOCGIFINDEX, &ifr) < 0) return false;

    struct sockaddr_can addr{};
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0)
        return false;
    return true;
}

bool CanReader::read_frame(std::vector<uint8_t>& out) {
    struct can_frame frame{};
    const ssize_t n = read(fd_, &frame, sizeof(frame));
    if (n < static_cast<ssize_t>(sizeof(frame))) return false;
    out.assign(frame.data, frame.data + frame.can_dlc);
    return true;
}

void CanReader::close_fd() {
    if (fd_ >= 0) { ::close(fd_); fd_ = -1; }
}
