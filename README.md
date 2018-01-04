# home-32 #

* install driver macOS 10.13 *

download            https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers

necessario ad ogni riavvio del mac

kextstat | grep silabs
sudo kextload -b com.silabs.driver.CP210xVCPDriver
kextstat | grep silabs
ls /dev/cu*

