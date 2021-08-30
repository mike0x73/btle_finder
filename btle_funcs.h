extern "C" {
	#include <ubertooth.h>
}

static int8_t cc2400_rssi_to_dbm( const int8_t rssi );
static uint64_t now_ns( void );
static void track_clk100ns(ubertooth_t* ut, const usb_pkt_rx* rx );
static uint64_t now_ns_from_clk100ns(ubertooth_t* ut, const usb_pkt_rx* rx );

