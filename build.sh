gcc main.c -o main.o -c
gcc dict_utils.c -o dict_utils.o -c
gcc print_utils.c -o print_utils.o -c
gcc input_utils.c -o input_utils.o -c
gcc -o main main.o dict_utils.o print_utils.o input_utils.o