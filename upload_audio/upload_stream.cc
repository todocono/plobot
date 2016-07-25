
#include <cstdio>
typedef unsigned char byte;
typedef unsigned short uint16_t;
#include <cstdlib>
#include <cstring>

#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <ctime>
#include <sys/time.h>
#include <string>

using namespace std;

FILE* modem_in = 0, *modem_out = 0;

unsigned long micros() {
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return 1000000 * tv.tv_sec + tv.tv_usec;
}

void sleep_micros(unsigned long m) {
	const unsigned long start_t = micros();
	while (micros() < (start_t + m)) {
	    sched_yield();
	}
}

int main(int argc, char **argv)
{
    ssize_t len;
    unsigned char buf[8192];

    if(argc != 4) {
    	fprintf(stderr, "Usage upload_stream input_file output_modem MODE(XSP,NORMAL)\n");
    	return 1;
    }

    modem_in = fopen(argv[2], "rb");
    if(!modem_in) {
    	fprintf(stderr, "Failed to open modem in %s\n", argv[2]);
    	return 1;
    }
    modem_out = fopen(argv[2], "wb");
    if(!modem_out) {
    	fprintf(stderr, "Failed to open modem out %s\n", argv[2]);
    	return 1;
    }

    // XSP configuration mode when a file begins with a newline
    const string mode = argv[3];
    if(mode == "XSP") {
	    const char out_cfg_mode = 'g';
	    fwrite(&out_cfg_mode, 1, 1, modem_out);
	} else if(mode == "NORMAL") {

	} else {
		fprintf(stderr, "Unknown mode argument: %s\n", mode.c_str());
		return 1;
	}

    FILE *in = fopen(argv[1], "rb");

    if(!in) {
    	fprintf(stderr, "Failed to open input file %s\n", argv[1]);
    	return 1;
    }

	uint16_t pageIndex = 0;
	while (true) {
		char page[256];
		size_t n = fread(&page[0], 1, sizeof(page), in);
		if(n > 0) {
			fwrite(&page[0], 1, sizeof(page), modem_out);

			if(pageIndex % 256 == 0) {
				sleep_micros(200 * 1000);
			} else {
				sleep_micros(5 * 1000);
			}

			if(pageIndex % 8 == 0)
				fprintf(stderr, "Transferred %ikb\n", (int)(pageIndex / 4));
			++pageIndex;
		}
		if(n < sizeof(page)) {
			break;
		}
	}
	return 0;
}
