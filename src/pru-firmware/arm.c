/* Shared buffer between ARM and PRU0 */
extern volatile char shared_buf[100];

int main()
{
   /* Invoke PRU */

   /* Perform a read from the shared buffer */
   return shared_buf[10];
}
   
