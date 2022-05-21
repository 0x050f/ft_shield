# ft_shield

`ft_shield` is a simple trojan horse that copy itself into `/bin` when launching as root (after a beautiful print of my 42 login `lmartin`) then add the created program as a service and launch it. If the binary is in `/bin` (like the new created program), it will open the port 4242 and ask for a password (`4242` here). If the password is correct the user access to a little program shell that can allow him to launch a real shell on the target machine.

## Compilation
```
make
```

## Usage
```
sudo ./ft_shield
```
Now you can enjoy:
```
$ nc localhost 4242
Password: 4242
$> shell
whoami
root
exit
$>
```