## Soul Knight Remote Multiplayer Host UDP broadcast relayer

### A C proram running in Linux layer of Android that relay the multiplayer discovery broadcast package from soul knight to the specified IP address and port in the same **IP POOL**.

## Compile (For Windows)

We have provided a pre-build verison of arm named ```udp_relay_arm```

Need Android NDK toolchain to do a cross-platform compile. 

0. Assume that you have unziped what it contains in [	android-ndk-r16b-windows-x86_64.zip](https://developer.android.com/ndk/downloads/) to **D:\ndk**

1. Place ``` agcc.bat ``` and ```agcc86.bat``` to D:\ndk.
2. Add D:\ndk and D:\ndk\android-ndk-r16b-windows-x86_64\android-ndk-r16b to the ```PATH``` of Environment Variables.

3. Execute ``` agcc.bat -o udp_relay udp_relay.c -std=c99 -O2  ``` in command prompt. If you want to compile a version for x86 platform, replace ```agcc.bat``` to ```agcc86.bat```

## Usage

### Requirement
For user:
1. A server with public IP, and open TCP/UDP:1723,1701 ports, and had configured a PPTP VPN service that all the clients connected can talk to each other and the server.
2. A computer with adb tools and adb-connect drivers of the host device.

For Host device (Host a multiplayer game):
1. A rooted device.
2. Full control to /sdcard/ and /data/local.
3. Open ```USB debugging``` switch in Developer Options.
4. Connect to a local network **with WIFI** wich has a **DHCP server**.
5. Connect to the PPTP VPN server mentioned above.

For Client device (Join in multiplayer game):
1. Connect to the PPTP VPN server mentioned above.

### Step (For Windows)


0. Assume that the host had been connected to a computer in the same local network through cable, and you can see your device through execute ``` adb devices``` in command prompt. **Assume that all of the devices not connect to the VPN server and not running the game. If needed, I will tell you when to do what.**

1. Push ```udp_relay``` binary program to ```/data/local```
2. Change protected bits of ```udp_relay``` to 777.
3. **NOW LET ALL OF THE DEVICE CONNECT TO THE VPN SERVER.**
4. Execute commands above in adb shell with superuser permission.
   ```
    ./udp_relay [-d] [num-of-player-exclude-host] 23333 [nth-player-ip-in-vpn-server]

    -d is for open debug mode. It will show you the details of relaying package.
   ```
   For example. I have 4 devices(host included) need to play together. Their IPs are from 192.168.10.10-192.168.10.13, 192.168.10.10 is the host. So I execute
   ```
   ./udp_relay -d 3 23333 192.168.10.11 192.168.10.12 192.168.10.13
   ```
5. If all is well, you can see the output like:
   ```
   Welcome to Soul Knight remote multiplayer host UDP relay helper by Nemesiss.Lin.
   Debug Mode.
   Will allow 3 players, and you will be the host.
   Port Number : 23333
   Add player : 192.168.10.11
   Add player : 192.168.10.12
   Add player : 192.168.10.13
   Setting host info...
   ```
6. **NOW LET ALL OF THE DEVICES ENTER THE GAME AND HOLD ON "Tap to start" SCREEN**
7. **NOW THE HOST DEVICE CREATE A MULTIPLAYER GAME AND ENTER THE SCREEN OF SELECTING CHARACTERS**
8. Check the adb output, you should see
   ```
   Player 0 -- Relaying a packet of length 125 from <ip-to-the-host-in-local-network>.
   Player 1 -- Relaying a packet of length 125 from <ip-to-the-host-in-local-network>.
   Player 2 -- Relaying a packet of length 125 from <ip-to-the-host-in-local-network>.
   ...
   ```
9. Check the host device, you can see your host ip is **192.168.10.10** in the screen of the lower left corner **instead of the local network ip assigned by DHCP server**.
    
10. **NOW LET OTHER PLAYER ENTER THE "Join Game"**.
11. If all goes well, all of the devices can enter the multiplayer room and the host can start the game.

## Note:
1. Ensure that all of the devices are connected to the VPN server before running ```udp_relay``` program.
2. Ensure that run ```udn_relay``` program before running the game. Or the game cannot bind the 23333 port.
3. If anyone exit the room, it won't be removed from playerlist in the room. Maybe the host should re-create a room. At that time it doesn't need to restart the ```udp_relay```. This bug is caused by the way to discover other players by the soul knight.
4. If **Unable to bind to socket**, use ```ps``` command to find out that whether there is an old ```udp_relay``` process running, if so,  kill it.
5. You can also use some "Terminal Emulator" apps on the host device, then the computer with adb env isn't necessary.
