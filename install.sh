#!/bin/bash

./build.sh

if [ -f /boot/grub/grub.cfg ]; then
    echo "Copying kernel..."
    sudo cp kernel/Veonter.kernel /boot/
    echo "Copying grub configuration..."
    sudo cp 49_veonter /etc/grub.d/
    echo "Updating configuration"

    if command update-grub >/dev/null 2>&1; then
        sudo update-grub
    else
        sudo grub-mkconfig -o temp_config.cfg

        echo "Copying configuration to /boot/..."

        sudo cp /boot/grub/grub.cfg $HOME/grub.cfg.bak

        sudo mv temp_config.cfg /boot/grub/grub.cfg

        echo "Configuration backup at $HOME/grub.cfg.bak"
    fi

    echo "Installation finished!";
else
    echo "Installation for non-grub machines is not supported";
    exit 1
fi
