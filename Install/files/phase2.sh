#!/usr/bin/env sh

export ROBOTNAME="%ROBOTNAME%"
export ROOT_DEV="`mount | grep \"/dev/sd\" | grep -v internal | grep -v userdata | cut -c6-8`"

if [ -z "${ROOT_DEV:-""}" ]; then
  echo "fatal:  Failed to detect root device"
  exit 1
fi

cp "/home/root/files/fstab_label" "/etc/fstab"
#cp "/home/root/files/menu.lst_${ROOT_DEV}" "/boot/grub/menu.lst"

export AL_DIR="/opt/naoqi/"
python /home/root/files/changeBodyIDNickName.py "${ROBOTNAME}" "127.0.0.1" || echo "error setting robot name"
/home/root/qitobh

update-rc.d -f alsa-state remove
update-rc.d -f sshd remove
update-rc.d -f openntpd remove
update-rc.d -f bftpd remove
update-rc.d -f webal remove
update-rc.d -f lighttpd remove
update-rc.d -f flash-detect remove
update-rc.d -f "boot-progress-0" remove
update-rc.d -f "boot-progress-10" remove
update-rc.d -f "boot-progress-20" remove
update-rc.d -f "boot-progress-30" remove
update-rc.d -f "boot-progress-40" remove
update-rc.d -f "boot-progress-50" remove
update-rc.d -f "boot-progress-60" remove
update-rc.d -f "boot-progress-70" remove
update-rc.d -f "boot-progress-80" remove
update-rc.d -f "boot-progress-90" remove
update-rc.d -f "boot-progress-100" remove
update-rc.d sshd defaults 90
update-rc.d alsa-state defaults 91

ln -sf "/etc/init.d/bhumand" "/etc/rc0.d/K02bhumand"
ln -sf "/etc/init.d/bhumand" "/etc/rc1.d/K02bhumand"
ln -sf "/etc/init.d/bhumand" "/etc/rc2.d/S58bhumand"
ln -sf "/etc/init.d/bhumand" "/etc/rc3.d/S58bhumand"
ln -sf "/etc/init.d/bhumand" "/etc/rc4.d/S58bhumand"
ln -sf "/etc/init.d/bhumand" "/etc/rc5.d/S58bhumand"
ln -sf "/etc/init.d/bhumand" "/etc/rc6.d/K02bhumand"

/etc/init.d/naoqi stop

rm -Rf /media/internal/*
umount "/media/internal"
rm -Rf "/media/internal"
ln -sf "/media/userdata" "/media/internal"

/etc/init.d/naoqi start

echo "Operation completed"
echo "Don't forget to reboot ${ROBOTNAME}."
