# FSWM100

## setup

1. https://esphome.io/guides/contributing#contributing-to-esphome
1. https://esphome.io/components/external_components
1. https://esphome.io/guides/getting_started_command_line.html
1. https://learn.microsoft.com/en-us/windows/wsl/connect-usb

## quick start

1. install python3
    1. `sudo apt-get install python3-venv`
1. create a virtual environment to contain ESPHome and it’s dependencies.
    1. `python3 -m venv venv`
    1. `source venv/bin/activate`
    1. You may or may not see (venv) at the beginning of your prompt depending on your shell configuration. This indicates that you are in the virtual environment.
1. `pip3 install esphome` or `pip3 install esphome -U` (to upgrade)
1. `esphome --version`
1. `esphome wizard my-test-device-wesp32-eth.yaml`
1. `esphome config my-test-device-wesp32-eth.yaml`
1. `esphome clean my-test-device-wesp32-eth.yaml` (if it fails to find a new class, etc.)
1. `esphome compile my-test-device-wesp32-eth.yaml`
1. `esphome upload my-test-device-wesp32-eth.yaml`

## to update ESPHome

1. `pip3 install esphome -U`
1. compile
1. upload

## VSCode

1. install cpp tools
1. install platform.io

## WSL

1. `apt install usbutils` in WSL
1. https://learn.microsoft.com/en-us/windows/wsl/connect-usb
1. connect the USB device
1. `usbipd list` in admin powershell
1. locate the BUSID you need _(e.g. `12-4   10c4:ea60  Silicon Labs CP210x USB to UART Bridge (COM3)`)_
1. `usbipd bind --busid 12-4` in admin powershell _(this should persist O/S restarts/sessions)_
1. `usbipd attach --wsl --busid 12-4` in powershell _(this may be needed after O/S suspend/resume)_
1. `lsusb` in WSL
1. `ls /dev/tty*USB*` should show `/dev/ttyUSB0` or similar, that should be the device
1. to disconnect from WSL `usbipd detach --busid 12-4` in powershell

## test config and quick compile

1. edit `tests/components/fswm100/common.yaml`, code, etc.
1. run `script/test_build_components -e compile -c fswm100`

## dev and flash

1. create `my-test-device-wesp32-eth.yaml` file at the root directory of the repo
1. enter python virtual `source venv/bin/activate`
1. ensure esphome is available `esphome version`
1. make changes to code/yaml/etc.
1. compile, upload and log, with `esphome run my-test-device-wesp32-eth.yaml`
    1. this will prompt after successful compile, how to flash (USB or OTA)
    1. to skip the prompt, use `esphome run my-test-device-wesp32-eth.yaml --device=/dev/ttyUSB0`
        1. `--no-logs` to prevent automatic logs after upload

## connect to view logs

`esphome logs my-test-device-wesp32-eth.yaml --device=/dev/ttyUSB0`

## clean build

`esphome clean my-test-device-wesp32-eth.yaml`