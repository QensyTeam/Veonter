#!/bin/sh

fallocate -l 64M disk.img
sudo mkfs.fat -F 32 disk.img
