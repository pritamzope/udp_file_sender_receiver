
# UDP File Sender/Receiver

--------------------------------------------------------------------------------

# What Is It?

A Peer-To-Peer file sender/receiver program using User Datagram Protocol(UDP) with
Cyclic Redundancy Check(CRC) error detection mechanism with acknowledgement procedure.
There is no Client/Server model, both are integrated into one program.

# Compilation

### Dependency

  ##### C Compiler(gcc/clang)

### Step 1: Getting the Source Code

You can download source by command:

    $ git clone https://github.com/pritamzope/udp_file_sender_receiver.git
    $ cd udp_file_sender_receiver

or you can get source via other ways you prefer at <https://github.com/pritamzope/udp_file_sender_receiver>

### Step 2: Compile and Run

Run command:

    $ make
    $ ./udp_file_sendreceive <send/receive> [options]


## Example

Run program udp_file_sendreceive without any options for help.

e.g.:

    $ ./udp_file_sendreceive

It will display following help.

    For sender: ./udp_file_sendreceive send <filename>
    For receiver: ./udp_file_sendreceive receive <ip-address>

For sending a file, use first command and for receiving a file, use second command.
A file sender waits for a receiver to connect.
For example i have a file named 'foobar.txt' which i want to send to another machine.
On terminal run,

    $ ./udp_file_sendreceive send foobar.txt


On another machine or another terminal but different location run,

    $ ./udp_file_sendreceive receive <ip-address-of-the-sender-machine>





