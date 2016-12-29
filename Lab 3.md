# Assignment 3: Replicating the Graph Store#

----

## Introduction ##

In this lab, we'll replicate the graph store you built in Lab 1 and Lab 2. 
We'll use a simple hybrid of the chain replication and primary backup protocols. 
Clients will send each update to the primary replica, which will forward the update to a backup replica, 
which in turn will forward the update to the next replica in the chain, and so on. 
Acknowledgments will travel up the chain back to the primary, which will respond to the client.

Reads are satisfied at the tail of the chain. 
We will also support reads from the middle of the chain. 
Each replica only applies the update to its own state (and sends back an ack to its predecessor in the chain) 
when it receives the acknowledgment from its successor.

For this lab, you can choose to either build above the non-durable Lab 1 version or 
the durable Lab 2 version of your code. 
We will not require durability for this lab, so it may be simpler to start from your Lab 1 version.

This lab also need to run on Google Cloud. You need to create two more VM to form a 3-server chain. 

## Changelist ##

## FAQ ##

Design questions:

Q: How does this differ from chain replication? Why not just use chain replication?

A: We send acknowledgments back up the chain; this is easier to implement, 
since the client can interact with the primary server as if the service is unreplicated.

Q: Why is it better than primary / backup?

A: We can get higher throughput for reads, since they can be issued at the tail replica. 

Q: Can we read from the middle of the chain?

A: In conventional Chain Replication, no. In this protocol, yes, 
since acks travel back up the chain and the middle replica only modifies 
its local state upon receiving an ack. As a result, if a replica returns some state, 
that state is guaranteed to be durable (i.e. exists on all replicas). (Is this protocol linearizable?)

Q: What happens to availability if a node fails?

A: The chain gets 'wedged'; updates are no longer possible, reads can continue.

## API Changes ##

The API is identical to previous lab, with one exception: 
the mutator methods should return error code 500 if there's a problem with replication 
(e.g. if a node in the chain is unreachable and the RPC fails).

## Inter-node communication ##

You can use any RPC library for inter-node communication. We suggest you use Apache Thrift.

Installing Thrift is trivial if you use a package management library like Homebrew on a Mac or apt-get on Ubuntu.

To get started, you might want to take a look at this tutorial: https://thrift.apache.org/tutorial/cpp

You have to write a thrift file describing a service with functions for each of the mutating operations we support; run thrift to generate C++ code from the thrift file; and then modify your existing C++ code to invoke the thrift-generated code for performing RPCs. When you build your code, you'll have to link to the thrift library (e.g., on a Mac with thrift installed via Homebrew, the following arguments for g++ might work: "-L/usr/local/lib/ -lthrift" assuming the library is in that directory).

One minor caveat is that Thrift does not support unsigned integers, so just use the signed variants and assume that we won't call the service with a node ID that doesn't fit in a signed integer. 

To enable the inter-VM communication you may need to add [firewall rules](https://console.cloud.google.com/networking/firewalls) to your VM.
![alt tag](https://git.yale.edu/CS426/Fall2016/blob/master/images/Lab3.png)

## Testing Methodology ##

Submit the code to the same repository as lab1 and lab2. Commit your changes, label your commit lab3 with git tag lab3 and perform a git push && git push --tags.

Provide a make file. Your code should run with the following command:

make && ./cs426_graph_server <port> -b ipaddress portnum

Note that we made the device file optional compared to Lab 2 (since providing durability is optional). The -b option allows us to specify the next node in the chain. The portnum is the port your server listening on. 

We'll use a different testing methodology for this lab. Instead of running the code ourselves, we will require you to run the code on your own virtual machines and provide us with the IPs and port numbers of the chain. We will then execute operations against the replicas. To test your code, run the lab 2 tester script against the primary replica.

Submit the code by midnight 10-28 (Monday), leave 3 servers in a chain running from then until 3 PM on 10-29 (Tuesday), and mail me the identities of the servers. We'll run the tests sometime in between.

## Code pointers ##

Use getopt for processing command line arguments; it will make life much easier!

Look for the changelist to see edits to this document.
