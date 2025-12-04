#!/bin/sh
SHARED="/shared"
LOCK="$SHARED/.lock"
touch "$LOCK"
echo "Worker: $(hostname)"
n=1
while true; do
    (
        flock -x 200
        i=1
        while [ $i -le 999 ]; do
            num=$(printf "%03d" $i)
            if [ ! -f "$SHARED/file_$num" ]; then
                echo "$(hostname):$n" > "$SHARED/file_$num"
                echo "CREATE file_$num"
                sleep 1
                if [ -f "$SHARED/file_$num" ]; then
                    rm "$SHARED/file_$num"
                    echo "DELETE file_$num"
                fi
                break
            fi
            i=$((i + 1))
        done
    ) 200>"$LOCK"
    n=$((n + 1))
    sleep 1
done
