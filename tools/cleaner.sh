#!/bin/bash

PRG_NAME="ft_shield"

if [ "$EUID" -ne 0 ]
	then echo "Please run as root"
	exit
fi

if command -v systemctl &> /dev/null
then
	systemctl stop ft_shield
	systemctl disable ft_shield
	rm -rf /etc/systemd/system/$PRG_NAME.service
	rm -rf /bin/$PRG_NAME
	systemctl daemon-reload
elif command -v openrc &> /dev/null
then
	service $PRG_NAME stop
	rm -rf /etc/init.d/$PRG_NAME
	rm -rf /bin/$PRG_NAME
fi
