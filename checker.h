
int array_sum(short *arr, size_t n) {
    int sum = 0;
    int i;
    for (i = 0; i < n; i++)
        sum += arr[i];
    return sum;
}

ssize_t generate_output(int sum, short *arr, size_t size, char *buf) {
    int i;
    char res_buff[500];
    char ch_buff[17];

    res_buff[0] = '\0';

    for (i=0; i<size; i++) {
        sprintf(ch_buff, " %i", arr[i]);
        strcat(res_buff, ch_buff);
    }
    
    return sprintf(buf, "%i%s", sum, res_buff);
}