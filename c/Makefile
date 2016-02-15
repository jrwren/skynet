CFLAGS+=-Ichan/src -pthread
LIBS+=chan
LDFLAGS+=-Lchan/.libs -lchan -lthread

#skynet: skynet.o
#	$(LD)  $(LDFLAGS) skynet.o chan/.libs/libchan.a -o skynet

skynet:
	cc -Ichan/src  -Lchan/.libs -lchan  skynet.c chan/.libs/libchan.a -pthread -o skynet
