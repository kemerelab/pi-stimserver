#! /bin/sh

# Other deployment actions here. Want to install (or bounce) service last.

if [ -f /etc/systemd/system/stimserver.service ]; then
    sudo systemctl stop stimserver.service
    sudo systemctl disable stimserver.service
else
    sudo \cp -f ./stimserver.service /etc/systemd/system/stimserver.service
fi

sudo systemctl enable stimserver.service
sudo systemctl start stimserver.service
