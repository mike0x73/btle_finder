#include "btle_enumerator.h"
#include <iostream>
#include <thread>
#include <btbb.h>
#include <sstream>
#include "btle_funcs.cpp"
#include <set>
#include <time.h>
#include <signal.h>

static void quit(int sig __attribute__((unused)))
{
	running = false;
}

void enumerate_btle_devices(const bool check_printable_name, const bool debug, const bool filter_seen)
{
    ubertooth_t* ut = ubertooth_init();
    int ubertooth_device = -1;

    if (ut == nullptr)
    {
        std::cout << "Failed to initialize ubertooth" << std::endl;
        ubertooth_stop(ut);
        exit(-1);
    }

	if (ubertooth_connect(ut, ubertooth_device) < 0) {
		std::cout << "Failed to connect to ubertooth device." << std::endl;
        ubertooth_stop(ut);
		exit(-1);
	}

	if (ubertooth_check_api(ut) < 0)
    {
		exit(-1);
    }

	// quit on ctrl-C
	signal(SIGINT, quit);
	signal(SIGQUIT, quit);
	signal(SIGTERM, quit);

    cmd_set_modulation(ut->devh, MOD_BT_LOW_ENERGY);
    uint16_t channel;
    channel = 2402;
    cmd_set_channel(ut->devh, channel);
	cmd_btle_sniffing(ut->devh, 0);
		
	std::set<std::string> seen_addresses;

    while (running) {
        usb_pkt_rx rx;
		bool print_result = true;

		int r = cmd_poll(ut->devh, &rx);
		if (r < 0) {
			std::cout << "USB error" << std::endl;
			break;
		}

		if (r == sizeof(usb_pkt_rx)) {
			fifo_push(ut->fifo, &rx);
			std::optional<btle_device> result = sniff(ut, debug);
			if (result.has_value())
			{
				btle_device device = result.value();	
				
				char address[17];
				ba2str(&(device.btle_device_address), address);

				if (filter_seen)
				{
					print_result = seen_addresses.find(address) == seen_addresses.end();
				}

				if(print_result)
				{
					std::string name = std::string(device.btle_device_name).substr(0, device.btle_name_size - 1);
					if (check_printable_name)
					{
						if(!contains_non_printable_characters(name))
						{
							std::cout << address << " " << name << std::endl; 
							seen_addresses.insert(address);
						}
					}
					else
					{
						std::cout << address << " " << name << std::endl; 
						seen_addresses.insert(address);
					}					
				}			
			}
		}

		std::this_thread::sleep_for(std::chrono::microseconds(5000));
	}

    ubertooth_stop(ut);
}

std::optional<btle_device> sniff(ubertooth_t* ut, const bool debug)
{
	btle_options opts = { .allowed_access_address_errors = 32 };

	lell_packet* pkt;
	int i;
	usb_pkt_rx usb = fifo_pop(ut->fifo);
	usb_pkt_rx* rx = &usb;
	// u32 access_address = 0; // Build warning

	static u32 prev_ts = 0;
	uint32_t refAA;
	int8_t sig, noise;

	uint64_t nowns = now_ns_from_clk100ns( ut, rx );

	/* Sanity check */
	if (rx->channel > (NUM_BREDR_CHANNELS-1))
		return {};

	if (infile == NULL)
		systime = time(NULL);

	/* Dump to sumpfile if specified */
	if (dumpfile) {
		uint32_t systime_be = htobe32(systime);
		fwrite(&systime_be, sizeof(systime_be), 1, dumpfile);
		fwrite(rx, sizeof(usb_pkt_rx), 1, dumpfile);
		fflush(dumpfile);
	}

	lell_allocate_and_decode(rx->data, rx->channel + 2402, rx->clk100ns, &pkt);

	/* do nothing further if filtered due to bad AA */
	if (opts.allowed_access_address_errors < lell_get_access_address_offenses(pkt)) {
		lell_packet_unref(pkt);
		return {};
	}

	/* Dump to PCAP/PCAPNG if specified */
	refAA = lell_packet_is_data(pkt) ? 0 : 0x8e89bed6;
	sig = cc2400_rssi_to_dbm( rx->rssi_max );
	noise = INT8_MIN; // FIXME - keep track of this

	if (ut->h_pcap_le) {
		/* only one of these two will succeed, depending on
		 * whether PCAP was opened with DLT_PPI or not */
		lell_pcap_append_packet(ut->h_pcap_le, nowns,
					sig, noise,
					refAA, pkt);
		// read the above comment: this function may silently fail
		lell_pcap_append_ppi_packet(ut->h_pcap_le, nowns,
		                            rx->clkn_high,
		                            rx->rssi_min, rx->rssi_max,
		                            rx->rssi_avg, rx->rssi_count,
		                            pkt);
	}
	if (ut->h_pcapng_le) {
		lell_pcapng_append_packet(ut->h_pcapng_le, nowns,
		                          sig, noise,
		                          refAA, pkt);
	}

	// rollover
	u32 rx_ts = rx->clk100ns;
	if (rx_ts < prev_ts)
		rx_ts += 3276800000;
	u32 ts_diff = rx_ts - prev_ts;
	prev_ts = rx->clk100ns;
	
	int len = (rx->data[5] & 0x3f) + 6 + 3;
	if (len > 50)
	{
		len = 50;
	}

	lell_packet_unref(pkt);
	fflush(stdout);

	unsigned char * data = rx->data;

	auto device = reinterpret_cast<btle_device*>(rx->data);
	if ((device->packet_type == 0x60))
	{
		if (debug) // dump packet data
		{
			std::cout << std::endl;
			for (i=0; i<len; i++)
			{
				printf("%02x ", rx->data[i]);
			}
			std::cout << std::endl;
		}

		return *device;
	}

	return {};
}

bool contains_non_printable_characters(std::string &item)
{
	for (const auto &character : item)
	{
		if (!isprint(character))
		{
			return true;
		}
	}

	return false;
}
