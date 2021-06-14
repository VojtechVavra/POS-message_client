# POS-message_client_sockets
POS socket programming task


Zadání:
1. TCP klient - odesílání zpráv na server
2. UDP server - příjem zpráv

* Max 255 znaků (1 bajt), ukončeno znakem '\n' (LN je včetně těch 255 znaků)
* 4 byte - IP adresa odesílatele (int, formát LO-HI)
* 4 byte - HTONL(time_t)
* 1 byte - délka následujícího textového řetězce (0 - 255)

IP adresa|čas odeslání|délka textu|textový řetězec|

127 | 0 | 0 | 1 | 0x4A | 0xAE | 0x1A | 0x30 | 3 | SSS
------------- | -------------  | -------------  | ------------- | ------------- | -------------  | -------------  | -------------  | -------------  | -------------
IP adresa  | IP adresa   | IP adresa   | IP adresa  | čas odeslání  | čas odeslání   | čas odeslání   | čas odeslání  | délka textu   | textový řetězec

