Platform independent library for NRF24L01+ transceiver
======================================================
This library has the goal of beeing a high performance and platform independent implementation for the NRF24L01+ transceiver chip of Nordic Semicondictor (https://www.nordicsemi.com/eng/Products/2.4GHz-RF/nRF24L01P)
It has been written from scratch with the goal beeing as easy as possible to use and beeing as portable as possible, using a hardware abstraction layer.
All features of the chip will be supported when done.


Status:
-------
The driver already works fine and is about to 85% done.
This library is NOT recommended for low energy designs yet, 
since it is only optimised for high performance transmissions.
Energy saving features will be implemented in the next iteration.


Features:
---------
- Support for following devices so far:
  - TM4C1294XL (Texas Instruments Tiva C Connected Launchpad)

- All six pipes are fully supported.
- High performance transmission. (~200KB/s @ 2Mbit mode)
- Adjustable size of payload (for each pipe)

Todo:
-----
- Adding a hardware abstraction layer for better portability
- Shockburst (Auto retransmission feature)
- Dynamic payloads
- Interrupt support
- Low energy support (use of all stand by modes)
- Carrier detect / Carrier transmit (for some technical frequency testing, not really important for most users)
