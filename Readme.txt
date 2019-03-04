SETUP:

https://engineering.facile.it/blog/eng/write-filesystem-fuse/              #refer this link to install fuse

configure the makefile with your imp.c and mountpoint

EXECUTION:

terminal 1:(mounting)

cd Desktop/project
touch bin.bin      # for first time mounting ; do not execute this if you want persistence and make sure this file already exists
make

terminal 2:(after mounting)

cd Desktop/mymount
cd mymount

#operations

touch b.txt
mkdir a
cd a
touch a.txt
cd ..
rmdir a
stat b.txt
chmod 777 b.txt
echo "dcs" > b.txt
cat b.txt
cp b.txt c.txt

terminal 3:(to unmount)
cd Desktop
fusermount -u mymount

note: in makefile, 
./ex -f -d /home/$(username)/Desktop/mymount
here, -d stands for debug mode 
if you don't want logs, remove this flag




