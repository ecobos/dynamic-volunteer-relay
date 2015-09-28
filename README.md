# volunteer-relay
Internet Censorship Circumvention Network - Man in the Middle Module


## The Issue
Freedom of speech and freedom to information are essential for the development and advancement of the human race. Freedom of speech is what allows the people to readily speak up about injustices, corruption or just provide a dissenting view points, without the fear of prosecution. On the flip side, freedom to information is what allows the people read or view the broadcast of such injustices, corruption or dissenting point of views. Therefore, freedom of speech and to information provides a type of checks-and-balances between the people and the governing body.  

The internet’s ability to connect the masses makes freedom of speech and freedom to information much more important. As of today, there exists governing bodies that censor the content that its governed are authorized to view. Tools like the [Tor Project](https://www.torproject.org/) that aim to circumvent these filters are used today to bypass the infamous [Great Firewall of China](https://en.wikipedia.org/wiki/Great_Firewall). Unfortunately, one major downfall of the Tor Anonymous Network is its [structure](https://www.torproject.org/about/overview). Tor gains its anonymity power from relaying data packets from one host server to N-number of other host servers. However, a side effect of such structures is that a user is not able to access specific types of information like videos because the network latency is too high. 

## The Proposed Solution
To circumvent a government sanctioned filter, we are creating a network that wraps an unauthorized data package within a data package that is seemingly authorized. Our network gains its strength over Tor by only making two hops before the data reaches its destination, thanks to its internal structure. The structure of the network consists of a client application, a man-in-the-middle application and an open-internet proxy. My proposed project is focusing on the man-in-the-middle application, which lies between the client application and the open-internet proxy. I will refer to this module as a volunteer relay. This relay is referred to as a volunteer relay because volunteering users will run the data relaying application on their personal computer. 

Since the relaying application will run on a volunteering user’s personal computer, it needs to have cross platform support. Additionally, we want to eventually give volunteering users the ability to control when our network can use their personal computer for data relaying. To accomplish that, I will be using the [QT platform](http://www.qt.io/developers/) which will allow me to write C/C++ code once and the platform will compile the program for the target operating systems.

## Implementation Details
* Programming language: C/C++
* Platform: [QT](http://www.qt.io/developers/) (Macintosh, Windows and Ubuntu)
* Components involved: There are two other components that have already been created. These components are the client-side application and the open-internet facing proxy. Additionally, Wireshark will be used in testing.  

## Timeline
1. October 2
  * A secure connection from a client to the volunteer relay is established.
  * Handshake/Key exchange occurs through an OpenSSL connection.
2. October 9
  * A secure connection from the volunteer relay to the open-internet facing proxy is established.
  * Handshake/Key exchange occurs through an OpenSSL connection.
3. October 23
  * Secure communication from a client to the open-internet facing proxy is functional.
4. October 30
  * Data packets are properly routed for N-number of clients.
  * HTTP/HTTPS request are tagged in transit to ensure proper routing.  
