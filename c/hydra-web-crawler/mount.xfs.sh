#!/bin/bash

#format the partition with:
# mkfs.xfs -b size=512 /dev/xvdb1

echo
echo "Mounting drive b partition 1 as XFS type"
echo "run 'umount *' to unmount this drive"
echo
echo
unlink dirout > /dev/null
mount -t xfs /dev/xvdb1 xfs
ln -s xfs dirout
echo
echo
