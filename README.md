# ft_shield

`ft_shield` is a simple **trojan** that copy itself into `/bin` when launching as root then add the created program as a service and launch it. If the binary is in `/bin` (like the new created program), it will open the port 4242 and ask for a password (Default password is given during compilation, you can modify the variable PASSWD in makefile to set your own). If the password is correct the user access to a little program shell that can allow him to launch a real shell on the target machine.

You can also add personalized payloads (elf, python, shellcode, ...).  
You just need to put a `payload` executable file (or shellcode) at the root of the repository. A example is available by running `make payload` to compile the server part and then compile with `make`.

## Demo
![alt text](https://github.com/y3ll0w42/ft_shield/blob/main/img/demo_trojan.png?raw=true)
![alt text](https://github.com/y3ll0w42/ft_shield/blob/main/img/demo_payload.png?raw=true)

## Compilation
```
make # Default mode without payload
make re # Compile with a payload server (like make but for the payload demo)
```

## Usage
```
sudo ./ft_shield
```

## Testing
You can test it inside a docker alpine image inside the `tools` folder.  
Alpine image is using openrc to run services.  
Run those command to get inside the container with your current folder:
```
# Alpine
docker build -t alpine-test ./tools/alpine
docker run -it -v $PWD:/app alpine-test
```