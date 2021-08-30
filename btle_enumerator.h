#include <optional>
#include <string>
#include <bluetooth/bluetooth.h>
extern "C" {
	#include <ubertooth.h>
}

typedef struct {
	unsigned char junk[4]; // we're not interested in this
	unsigned char packet_type;
	unsigned char length;
	bdaddr_t btle_device_address;
	unsigned char more_junk[11]; // we're not interested in this
	unsigned char btle_name_size;
	unsigned char name_type;
    char btle_device_name[256];
	
} btle_device;

int running = true;
static void quit(int sig __attribute__((unused)));

void enumerate_btle_devices(const bool check_printable_name, const bool debug, const bool filter_seen);
std::optional<btle_device> sniff(ubertooth_t* ut, const bool debug);
bool contains_non_printable_characters(std::string &item);
