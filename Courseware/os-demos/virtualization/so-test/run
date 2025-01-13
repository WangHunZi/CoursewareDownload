export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

for _ in $(seq 1 100)
do
    ./bloat &
done


for _ in $(seq 1 100)
do
   wait
done
