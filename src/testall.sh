


if test ${1} = single
then
    echo ${2}
    ./build/compiler ${2} > ${2/.sy/.log}
    arm-linux-gnueabihf-gcc -march=armv7 ${2/.sy/.s} ./libsysy.a --static -o ${2/.sy/.bin}
    if test -e ${2/.sy/.in}
    then
        qemu-arm ${2/.sy/.bin} < ${2/.sy/.in} > ${2/.sy/.myout}
        echo -e \\n$? >> ${2/.sy/.myout}
    else
        qemu-arm ${2/.sy/.bin} > ${2/.sy/.myout}
        echo -e \\n$? >> ${2/.sy/.myout}
    fi
    
    diff -Bb ${2/.sy/.myout} ${2/.sy/.out}
elif test ${1} = all
then
    cnt=-1
    sz=100
    if [ -n ${2} ]
    then
        sz=$((${2}))
    fi
    for file in ./tests/hidden_functional/*.sy
    do
        cnt=`expr ${cnt} + 1`
        if [ ${cnt} -lt ${sz} ]
        then
            continue
        fi
        echo ${file}
        ./build/compiler ${file} > ${file/.sy/.log}
        arm-linux-gnueabihf-gcc -march=armv7 ${file/.sy/.s} ./libsysy.a --static -o ${file/.sy/.bin}
        if test -e ${file/.sy/.in}
        then
            ${file/.sy/.bin} < ${file/.sy/.in} > ${file/.sy/.myout}
            echo -e \\n$? >> ${file/.sy/.myout}
        else    
            ${file/.sy/.bin} > ${file/.sy/.myout}
            echo -e \\n$? >> ${file/.sy/.myout}
        fi
        diff -Bb ${file/.sy/.myout} ${file/.sy/.out}
        
    done
elif test ${1} = gcc
then
    cnt=-1
    sz=100
    if [ -n ${2} ]
    then
        sz=$((${2}))
    fi
    for file in ./tests/debug/*.c
    do
        cnt=`expr ${cnt} + 1`
        if [ ${cnt} -lt ${sz} ]
        then
            continue
        fi
        echo ${file/.c/.s}
        arm-linux-gnueabihf-gcc -march=armv7 -S ${file} -o ${file/.c/.s} -O2
        arm-linux-gnueabihf-gcc -march=armv7 ${file/.c/.s} ./libsysy.a --static -o ${file/.c/.bin}
        if test -e ${file/.c/.in}
        then
            qemu-arm ${file/.c/.bin} < ${file/.c/.in} > ${file/.c/.myout}
            echo -e \\n$? >> ${file/.c/.myout}
        else    
            qemu-arm ${file/.c/.bin} > ${file/.c/.myout}
            echo -e \\n$? >> ${file/.c/.myout}
        fi
        diff -Bb ${file/.c/.myout} ${file/.c/.out}
        
    done
elif test ${1} = run-single
then
    if test -e ${2/.bin/.in}
    then
        qemu-arm ${2} < ${2/.bin/.in} > ${2/.bin/.myout}
        echo $? >> ${2/.bin/.myout}
    else
        qemu-arm ${2} > ${2/.bin/.myout}
        echo $? >> ${2/.bin/.myout}
    fi
    diff ${2/.bin/.myout} ${2/.bin/.out}
elif test ${1} = run-all
then
    for file in ./tests/hidden_functional/*.bin
    do
        echo ${file}
        if test -e ${file/.bin/.in}
        then
            qemu-arm ${file} < ${file/.bin/.in} > ${file/.bin/.myout}
            echo $? >> ${file/.bin/.myout}
        else
            qemu-arm ${file} > ${file/.bin/.myout}
            echo $? >> ${file/.bin/.myout}
        fi
        diff ${file/.bin/.myout} ${file/.bin/.out}
    done
fi