#! /bin/sh

# Compile and install stimserver
sudo gcc -o /usr/local/bin/stimserver stimserver.c -lbcm2835

if [ -f /etc/systemd/system/stimserver.service ]; then
    sudo systemctl stop stimserver.service
    sudo systemctl disable stimserver.service
fi

sudo \cp -f ./stimserver.service /etc/systemd/system/stimserver.service

sudo systemctl enable stimserver.service
sudo systemctl start stimserver.service
