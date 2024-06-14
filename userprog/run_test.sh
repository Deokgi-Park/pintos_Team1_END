make clean

make

cd build

pintos -T 20 --fs-disk=10 -p tests/userprog/args-single:args -- -q -f run 'args oneagu'

#pintos  -T 10 --fs-disk=10 -p tests/userprog/args-many:echo -- -q -f run 'echo x y z'
#pintos --gdb --fs-disk=10 -p tests/userprog/args-single:args-single -- -q -f run 'args-single oneagu'

stty sane

