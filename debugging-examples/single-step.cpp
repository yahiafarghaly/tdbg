int main()
{
    /* Remember that the 'next' command shows the current instruction address which
       is stopped by and the instruction which is ready to be executed is shown by 
       the value of RIP register. In this case the RIP points to the stopped instruction or
       in otherwords, ready to run !
    */
    int x, y, z;
    x = 1;
    y = 2;
    z = 3;
    z = x + y;
    x = z - y;
    y = z - x;
    return 0;
}