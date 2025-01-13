#!/bin/bash

# Look for 'vfat' partitions
echo "vfat Partitions (commonly used for EFI):"

# lsblk
#   - -o NAME,FSTYPE,LABEL,MOUNTPOINT,SIZE,PARTFLAGS: Specifies the
#     columns to display. These include the device name, file system
#     type, label, mount point, size, and partition flags.
#   - -l: Provide a list-view rather than tree.

lsblk -o NAME,FSTYPE,LABEL,MOUNTPOINT,SIZE,PARTFLAGS -l \
    | grep -i "vfat"
