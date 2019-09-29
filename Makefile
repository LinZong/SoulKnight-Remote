CFLAGS := 
LDFLAGS := -static

udp_relay: udp_relay.c
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall udp_relay.c -o udp_relay

clean:
	rm -f udp_relay
