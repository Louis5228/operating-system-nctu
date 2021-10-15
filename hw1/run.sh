rm -rf *.csv

# compile all cpp files
gcc -o multi_process_int.out multi_process_int.c
gcc -o multi_process_string.out multi_process_string.c
gcc -o multi_process_struct.out multi_process_struct.c
gcc -o multi_thread_int.out multi_thread_int.c -lpthread
gcc -o multi_thread_string.out multi_thread_string.c -lpthread
gcc -o multi_thread_struct.out multi_thread_struct.c -lpthread

echo "Start generate csv files."
for((i = 1; i <= 100000000; i *= 10))
do
    for((j = 1; j <= 15; j++))
    do
        ./multi_process_int.out $i $j > /dev/null
        ./multi_process_string.out $i $j > /dev/null
        ./multi_process_struct.out $i $j > /dev/null
        ./multi_thread_int.out $i $j > /dev/null
        ./multi_thread_string.out $i $j > /dev/null
        ./multi_thread_struct.out $i $j > /dev/null
    done
done
echo "Generating csv files done."

echo "Plot figures."
chmod +x plot.py
python3 plot.py