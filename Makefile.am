EXTRA_DIST = README BUGS tst tst_sdp
SUBDIRS = osip parser fsm test scripts doc conf windows

check:
	@echo " ****** starting tests! ********"
	./$(top_srcdir)/tst ./$(top_srcdir)/conf/torture_msgs -v -c > ./osip-chk.log 2>&1
	./$(top_srcdir)/tst_sdp ./$(top_srcdir)/conf/torture_sdps -v -c > ./osipsdp-chk.log 2>&1

	@echo " ******              RESULTS                 ********"

	@echo " **SIP* NUMBER OF SUCCESS (87 are expected) ********"
	@more osip-chk.log | grep "==========" | grep OK | wc -l
	@echo " **SIP* NUMBER OF FAILURE (6 are expected)  ********"
	@more osip-chk.log | grep "==========" | grep FA | wc -l

	@echo " **SDP* NUMBER OF SUCCESS (15 are expected)  ********"
	@more osipsdp-chk.log | grep "==========" | grep OK | wc -l
	@echo " **SDP* NUMBER OF FAILURE (0 are expected)   ********"
	@more osipsdp-chk.log | grep "==========" | grep FA | wc -l
	@echo ""
	@echo "In case you have a doubt, send the generated"
	@echo "log file with your comment to <jack@atosc.org>."

rpm:
	rpm -ba libosip.spec

deb:
	dpkg-buildpackage -rfakeroot


indent:
	indent */*.c
	indent */*.h
	rm -rf */*.c~
	rm -rf */*.h~

