

PI_HOSTNAME="liquid-helium"
GIT_REPO_PATH="~/SpaceSalmon"


scp "$1" "$PI_HOSTNAME":~/quail_firmware.bin

ssh "$PI_HOSTNAME" "sudo bash $GIT_REPO_PATH/pi_upload/quail_mount.sh"

sleep 2

ssh "$PI_HOSTNAME" "sudo python $GIT_REPO_PATH/uf2conv.py -b 0x4000 quail_firmware.bin"

