CLP_NRF24
=========

NRF24L01+ driver for the Texas Instruments Tiva C Connected Launchpad.

This implementation has been written from scratch with the goal beeing as easy as possible to use and beeing portable as possible, using a hardware abstraction layer.

Status:
The driver is almost done. Data from another NRF transveicer gets received properly. This has been tested using a r0ket (http://www.r0ket.de/)

Todo:
- Sending packets
- Shockburst
- Interrupt support
- Carrier Detect
- (Maybe some fancy stuff with the different pipes)
- Adding a hardware abstraction layer file for better portability
