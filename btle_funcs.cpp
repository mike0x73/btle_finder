#include "btle_funcs.h"

static int8_t cc2400_rssi_to_dbm( const int8_t rssi )
{
	/* models the cc2400 datasheet fig 22 for 1M as piece-wise linear */
	if (rssi < -48) {
		return -120;
	}
	else if (rssi <= -45) {
		return 6*(rssi+28);
	}
	else if (rssi <= 30) {
		return (int8_t) ((99*((int)rssi-62))/110);
	}
	else if (rssi <= 35) {
		return (int8_t) ((60*((int)rssi-35))/11);
	}
	else {
		return 0;
	}
}

static uint64_t now_ns( void )
{
	struct timespec ts = { 0, 0 };
	(void) clock_gettime( CLOCK_REALTIME, &ts );
	return (1000000000ull*(uint64_t) ts.tv_sec) + (uint64_t) ts.tv_nsec;
}

static void track_clk100ns(ubertooth_t* ut, const usb_pkt_rx* rx )
{
	/* track clk100ns */
	if (!ut->start_clk100ns) {
		ut->last_clk100ns = ut->start_clk100ns = rx->clk100ns;
		ut->abs_start_ns = now_ns( );
	}
	/* detect clk100ns roll-over */
	if (rx->clk100ns < ut->last_clk100ns) {
		ut->clk100ns_upper += 1;
	}
	ut->last_clk100ns = rx->clk100ns;
}

static uint64_t now_ns_from_clk100ns(ubertooth_t* ut, const usb_pkt_rx* rx )
{
	track_clk100ns( ut, rx );
	return ut->abs_start_ns + 100ull *
	       (uint64_t)((ut->clk100ns_upper<<32)|rx->clk100ns) - ut->start_clk100ns;
}