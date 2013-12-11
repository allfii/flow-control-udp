1. Compile Transmitter:
	gcc -o transmitter client.c DieWithMessage.c AddressUtility.c -lpthread

2. Compile Receiver:
	gcc -o receiver Control.c -lpthread

3. Run program:
	a. Open two terminal such as terminal A and terminal B
	b. On terminal A, run receiver first with command:
		./receiver [port-number]
	   note: [port-number] must be filled with port number binded by receiver
	c. On terminal B, run transmitter with command:
		./transmitter [ip-address-receiver] [port-number-receiver] [file txt]
	   note: [ip-address-receiver] must be filled with ip address where receiver run
		[port-number-receiver] must be filled with port number binded by receiver
		[file txt] must be filled with filename .txt which want to be sent and must be placed in the same directory of program. File .txt only contains one line.

4. Example
	On terminal A, write
		./receiver 4848
	On terminal B, write
		./transmitter 127.0.0.1 4848 text.txt
	
5. This program is contributed by
	a. Hartono Sulaiman Wijaya
	b. Alfian Ramadhan			
	c. Septu Jamasoka			
