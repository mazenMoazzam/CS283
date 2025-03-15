1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines when a command's output is fullt received from the server by using a message delimeter
or a protocol-defined market to indicate the end of the transmission. In my implementation, the server in this 
case uses RDSH_EOF_CHAR which marks the end of the response. The client reads data in chunkcs and checks for this
delimeter to know the transmission is complete. Some techniques to handle partial reads is to loop until it encounters
the delimeter or the EOF character, or using fixed-length headers which sends the length of the message before the 
actual data.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol should define the message boundaries by using special characters (a null byte for example for the end of a 
command and RDSH_EOF_CHAR for the end of response), and length prefixing which is sending the length of the message before the actual
data.The challenges that may occur if this is not implemented correctly is TCP may split or combine messages, which can make it difficult
to determine where one message ends and another begins, the client may read only part of a message, which can also lead to incorrect
or incomplete command execution.

3. Describe the general differences between stateful and stateless protocols.

The differences between a stateful and stateless protocol is stateful protocols maintain the state of the server, the server in this
case keeps track of the client's state (session informatio and previous requests or actions), it is easier to implement complex 
interactions and can optimize the performance of the server by reusing network connections. Stateless protocols on the other 
hand signifies that each request from the client is independent, and the server does not retain any information about previous
requests made, an example of this is HTTP.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

The case we would use UDP even though it is unreliable is due to its low latency, UDP is faster than TCP because it does not 
establish a connection or perform error checking, which can save more time. It is also ideal if we are developing more real-time
applications like video-streaming, online gaming and other applications that need real-time data. However, the delivery of the data 
may result in the data packets getting lost and the packets may arrive out of order.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The interface or abstraction that is provided by the operating system to enable applications to use network connections is the Socket 
interface/abstraction. It is the primary interface for network communications and an example is Socket API. This interface allows 
applications to send and receive data over a network using protocols like TCP and UDP as well.
