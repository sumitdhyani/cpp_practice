# How TCP works:

## Establishment of connection:
- A server is listening on a port
- A client sends SYN
- The server sends SYNACK
- The client sends ACK and a 3 way handshake is completed, which marks the establishment of the connection

## Operation
### Sender Side
Sender sends a packet that has 3 parts:
- seq.No
- Checksum
- payload(or application data)
### Receiver Side
The receiver, when receives the data:
- checks the SEQ. No that it is 1 + the last recd. Seq. No
- Then the receiver runs checksum on the payload to ensure that the data is correct and has not been corrupted
- If the SeqNo is as expected and the checksum matches, the receiver sends an ACK, with the next Expected SeqNo(which is 1 + the last recd. seqNo) as a field in the header

# Features of TCP
1. ## Guarenteed, sequential, correct delivery
The receiver stores the last seqNo. received.
So let's say the sender received a packet with SeqNo. 10 and then sends an ACK with SeqNo. 11, which means that the next expected seq. no is 11.

But lets say the packet with SeqNo 11 is lost due to some reason, and the receiver get SeqNo. 12 instead.
Then the receiver will send SeqNo. = 11 in its ack(this is called duplicate ack), indicating that it has lost packet with SeqNo. = 11.
This prompts the sender to send the packet 11 again.

### The send buffer
Now that we know that the sender keeps unacknowledged messages, in case thet are asked to be resent using a duplicate ACK, there needs to be a buffer at sender end that keeps those un-acked messages.

For example let's say that the last packet that was acked was 11, and the packets 12-22 are in send buffer,
waitin for ACK, then if the receiver sends an ACK with seq = 16, then it means that the packets 12-15 will be removed from the send-buffer

There is a limit on the max no. of on-the-fly un-acked packets can be there, this limit is decided in the handshake phase.

If this buffer is full, then any attempt by the application to send data on socket is rejected.

### The Receive buffer
As the receiver receives packets, it keeps those packets in a buffer and in the exact sequence that they were received in.
Unless an application consumes the packets(or the rate of consumption is < the rate at which they are sent), it keeps on increasing in size, unless it reaches a predefined max limit.

### Zero Window Probes
If the sender tries to send some data to the receiver when its receive buffer is full, then it the receiver sends a special message(window size = 0) to the receiver, prompting it to not send any more data.
After after that the sender only sends "Zero window probes"
There is a limit on the max no. of on-the-fly un-acked packets can be there, this limit is decided in the handshake phase.

If this buffer is full, then any attempt by the application to send data on socket is rejected.
